CPPFLAGS=-g -O3 -mavx -fPIC -I$(JAVA_HOME)/include -I$(JAVA_HOME)/include/linux -I$(JAVA_HOME)/include/Darwin
CPPFLAGS_OMP=$(CPPFLAGS) -fopenmp -DOMP


OUTPUT_OPTION=-MMD -MP -o $@

OS = $(shell uname)
ifeq ($(OS), Linux)
	TARGET = IntelPairHmm.h IntelPairHmm.o IntelPairHmm_omp.o
	LIB_TARGET = ../libGKL_pairHMM.so ../libGKL_pairHMM_omp.so
	LINKFLAGS= -shared -static-libgcc -static-libstdc++
else ifeq ($(OS), Darwin)
	TARGET = IntelPairHmm.h IntelPairHmm.o
	LIB_TARGET = ../libGKL_pairHMM.dylib
	LINKFLAGS= -shared -static-libstdc++
else
    $(error "Unsupported OS")
endif

SRC = $(wildcard *.cc)
OBJ = $(SRC:.cc=.o)
DEP = $(SRC:.cc=.d)

all: $(TARGET) $(LIB_TARGET)

$(LIB_TARGET): $(OBJ); $(CXX) $(CPPFLAGS) $(LINKFLAGS) -o $@ $^

IntelPairHmm.h: ../../java/com/intel/gkl/pairhmm/IntelPairHmm.java
	javah -o IntelPairHmm.h -cp ../../java com.intel.gkl.pairhmm.IntelPairHmm

IntelPairHmm_omp.o: $(SRC)
	$(CXX) $(CPPFLAGS_OMP) -c $(SRC) -o $@

-include $(DEP)

clean:
	rm -f $(OBJ) $(DEP) $(TARGET) $(LINK_TARGET)

