CPPFLAGS=-c -O3 -mavx -fPIC -Iisa-l-master/include/ -I$(JAVA_HOME)/include -I$(JAVA_HOME)/include/linux -I$(JAVA_HOME)/include/Darwin
LINKFLAGS= -z noexecstack -shared -fPIC -O3 -static-libgcc -static-libstdc++
OUTPUT_OPTION=-MMD -MP -o $@
LDLIBS=isa-l-master/lib/libisal.a otc_zlib/libz.a
CURDIR=$(shell pwd)

OS = $(shell uname)
ifeq ($(OS), Linux)
    #build isa-l-master
	TARGET = IntelDeflater.o IntelInflater.o
	LIB_TARGET = ../libGKL_compression.so
else ifeq ($(OS), Darwin)
    #build isa-l-master
	TARGET = IntelInflater.o IntelDeflater.o
	LIB_TARGET = ../libGKL_compression.dylib
else
    $(error "Unsupported OS")
endif

SRC = $(wildcard *.cc)
OBJ = $(SRC:.cc=.o)

all: OTC_ZLIB ISAL_LIB javah $(TARGET) $(LIB_TARGET)

OTC_ZLIB: $(EXE);cd otc_zlib;make clean;export CFLAGS="=fPIC";./configure --static;make;cd -

ISAL_LIB: $(EXE);cd isa-l-master;make clean;./autogen.sh;export CFLAGS="-fPIC";./configure --prefix=$(CURDIR)/isa-l-master/usr --libdir=$(CURDIR)/isa-l-master/lib;make;make install;cd -

javah:
	javah -o IntelDeflater.h -cp ../../java com.intel.gkl.compression.IntelDeflater
	javah -o IntelInflater.h -cp ../../java com.intel.gkl.compression.IntelInflater

IntelDeflater.o: IntelDeflater.cc
	$(CXX) $(CPPFLAGS) -c IntelDeflater.cc -o $@

IntelInflater.o: IntelInflater.cc
	$(CXX) $(CPPFLAGS) -c IntelInflater.cc -o $@

$(LIB_TARGET): $(TARGET); $(CXX) $(LINKFLAGS) -o $@ $^ $(LDLIBS)

.PHONY: clean

clean:
	rm -f $(TARGET) $(LIB_TARGET)