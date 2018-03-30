CC      = gcc
CFLAGS= -O3 -std=c++11
SHARED_LIBRARIES = -L../../../build/native -lgkl_pairhmm_c -lstdc++

all: pairhmm clean

pairhmm: PairHMMUnitTest.o
	$(CC) -o $@ $(SHARED_LIBRARIES) $^ $(CFLAGS)

PairHMMUnitTest.o: PairHMMUnitTest.cpp PairHMMUnitTest.h
	$(CC) -c $(CFLAGS) $<

.PHONY: clean cleanest

clean:
	rm *.o

cleanest: clean
	rm pairhmm