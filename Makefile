UNAME_S := $(shell uname)

MPICH_DIR=/scratch2/wwu/mpich-4.2.1/install
OMPI_DIR=/usr/local/openmpi-4.1.5

OMPICC=$(OMPI_DIR)/bin/mpicc
OMPICXX=$(OMPI_DIR)/bin/mpicxx
MPICHCC=$(MPICH_DIR)/bin/mpicc
MPICHCXX=$(MPICH_DIR)/bin/mpicxx

ifeq ($(UNAME_S),Darwin)
    CC=clang
    CFLAGS=-ferror-limit=1 # Clang
    #CFLAGS+=-Wno-c2x-extensions
    #CFLAGS+=-Wno-unused-function
    #CFLAGS+=-Wno-incompatible-function-pointer-types
else
    OSID := $(shell grep '^ID=' /etc/os-release | cut -d= -f2)
    ifeq ($(OSID),ubuntu)
        CC=gcc
		CXX=g++
        # CC=clang
		# CXX=clang++
    endif
    ifeq ($(OSID),fedora)
        CFLAGS=-fmax-errors=1 # GCC
        #CFLAGS+= -fsanitize=address
        # these suppress true errors with callbacks
        #CFLAGS+=-Wno-incompatible-pointer-types
        #CFLAGS+=-Wno-cast-function-type
        #CFLAGS+=-Wno-unused-parameter -Wno-unused-variable -Wno-unused-function
    endif
endif
-include Makefile.local

CFLAGS += -ggdb -O0 -Wall -Wextra -fPIC  #-Werror # -Wpedantic 
CXXFLAGS = -ggdb -O0 -Wall -Wextra -fPIC -std=c++17
CFLAGS_MPICH = -DUSE_MPICH
CFLAGS_OMPI = -DUSE_OMPI

SO = $(if $(findstring Darwin,$(UNAME_S)),dylib,so)
RPATH = $(if $(findstring Darwin,$(UNAME_S)),'@rpath','$$ORIGIN')
SOFLAGS = -shared
SOLIBS =

AR      = ar
ARFLAGS = -r

RUNTESTS = test_comm test_comm_c test_pingpong test_pingpong_c test_sendrecv test_type test_allreduce test_bcast

all: libs $(RUNTESTS)

libs: libmpi_wrap.a libmpi_wrap.$(SO) libimpl_mpich.$(SO) libimpl_ompi.$(SO)

test_comm: test_comm.cc libmpi_wrap.$(SO) mpi_wrap.h
	$(CXX) $(CXXFLAGS) $< -L. -Wl,-rpath,$(RPATH) -lmpi_wrap -o $@

test_comm_c: test_comm.c libmpi_wrap.$(SO) mpi_wrap.h
	$(CC) $(CFLAGS) $< -L. -Wl,-rpath,$(RPATH) -lmpi_wrap -o $@

test_pingpong: test_pingpong.cc libmpi_wrap.$(SO) mpi_wrap.h
	$(CXX) $(CXXFLAGS) $< -L. -Wl,-rpath,$(RPATH) -lmpi_wrap -o $@

test_pingpong_c: test_pingpong.c libmpi_wrap.$(SO) mpi_wrap.h
	$(CC) $(CFLAGS) $< -L. -Wl,-rpath,$(RPATH) -lmpi_wrap -o $@

test_sendrecv: test_sendrecv.cc libmpi_wrap.$(SO) mpi_wrap.h
	$(CXX) $(CXXFLAGS) $< -L. -Wl,-rpath,$(RPATH) -lmpi_wrap -o $@

test_type: test_type.cc libmpi_wrap.$(SO) mpi_wrap.h
	$(CXX) $(CXXFLAGS) $< -L. -Wl,-rpath,$(RPATH) -lmpi_wrap -o $@

test_allreduce: test_allreduce.cc libmpi_wrap.$(SO) mpi_wrap.h
	$(CXX) $(CXXFLAGS) $< -L. -Wl,-rpath,$(RPATH) -lmpi_wrap -o $@

test_bcast: test_bcast.cc libmpi_wrap.$(SO) mpi_wrap.h
	$(CXX) $(CXXFLAGS) $< -L. -Wl,-rpath,$(RPATH) -lmpi_wrap -o $@

MPI_H =

IMPL_H =    impl.h

IMPL_FUNCTION_C :=  impl_wrap.cc impl_mpi.cc

IMPL_FUNCTION_O := $(patsubst %.cc,%.o,$(IMPL_FUNCTION_C))
MPICH_FUNCTION_O := $(subst impl,mpich,$(IMPL_FUNCTION_O))
OMPI_FUNCTION_O := $(subst impl,ompi,$(IMPL_FUNCTION_O))

libmpi_wrap.a: mpi_wrap.o
	$(AR) $(ARFLAGS) $@ $<


ifeq ($(UNAME_S),Darwin)
libmpi_wrap.$(SO): WRAPLIBS+=-Wl,-rpath,$(RPATH)
libmpi_wrap.$(SO): WRAPLIBS+=-Wl,libimpl_mpich.dylib
libmpi_wrap.$(SO): WRAPLIBS+=-Wl,libimpl_ompi.dylib
endif
libmpi_wrap.$(SO): SOLIBS+=-ldl
libmpi_wrap.$(SO): mpi_wrap.o | libimpl_mpich.$(SO) libimpl_ompi.$(SO)
	$(CXX) $< $(SOFLAGS) $(SOLIBS) $(WRAPLIBS) -o $@

mpi_wrap.o: mpi_wrap.cc $(MPI_H)
	$(CXX) $(CXXFLAGS) -c $< -o $@

libimpl_mpich.$(SO): $(MPICH_FUNCTION_O)
	$(MPICHCXX) $(SOFLAGS) $(SOLIBS) $^ -o $@

libimpl_ompi.$(SO): $(OMPI_FUNCTION_O)
	$(OMPICXX) $(SOFLAGS) $(SOLIBS) $^ -o $@

mpich_wrap.o: impl_wrap.cc $(IMPL_H)
	$(MPICHCXX) $(CXXFLAGS) $(CFLAGS_MPICH) -c $< -o $@

ompi_wrap.o: impl_wrap.cc $(IMPL_H)
	$(OMPICXX) $(CXXFLAGS) $(CFLAGS_OMPI) -c $< -o $@

ompi_mpi.o: impl_mpi.cc $(IMPL_H)
	$(OMPICXX) $(CXXFLAGS) $(CFLAGS_OMPI) -c $< -o $@

mpich_mpi.o: impl_mpi.cc $(IMPL_H)
	$(MPICHCXX) $(CXXFLAGS) $(CFLAGS_MPICH) -c $< -o $@

check: $(RUNTESTS)
	./test.sh ./testcoll.x
	./test.sh ./testcoll2.x
	./test.sh ./testcomm.x
	./test.sh ./testcart.x
	./test.sh ./testgroup.x
	./test.sh ./testinit.x
	./test.sh ./testops.x
	./test.sh ./testreqs.x
	./test.sh ./testtypes.x
	./test.sh ./testtypes2.x
	./test.sh ./testwin.x
	./test.sh ./testbottom.x
	./test.sh ./testerrh.x

clean:
	-rm -f *.o *.x *.s *.a *.i *.$(SO)
	-rm -rf *.dSYM
	-rm -rf *.btr # backtrace
