CPPFLAGS=-c -O3 -mavx -fPIC -I$(JAVA_HOME)/include -I$(JAVA_HOME)/include/linux -I$(JAVA_HOME)/include/Darwin
LINKFLAGS= -shared
OUTPUT_OPTION=-MMD -MP -o $@
LDLIBS=


OS = $(shell uname)
ifeq ($(OS), Linux)
	TARGET = ftz.o
	LIB_TARGET = ../libGKL_FTZ.so
else ifeq ($(OS), Darwin)
	TARGET = ftz.o
	LIB_TARGET = ../libGKL_FTZ.dylib
else
    $(error "Unsupported OS")
endif


SRC = $(wildcard *.cc)
OBJ = $(SRC:.cc=.o)
DEP = $(SRC:.cc=.d)

$(LIB_TARGET): $(OBJ); $(CXX) $(LINKFLAGS) -o $@ $^

all: javah $(TARGET)

javah:
	javah -o ftz.h -cp ../../java com.intel.gkl.ftz.FTZ


-include $(DEP)

clean:
	rm -f $(OBJ) $(DEP) $(TARGET) $(LIB_TARGET)
