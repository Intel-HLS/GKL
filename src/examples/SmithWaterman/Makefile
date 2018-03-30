CC      = g++
CFLAGS= -O3
SHARED_LIBRARIES = -L../../../build/native -lgkl_smithwaterman_c

all: swpairwise clean

swpairwise: SmithWatermanUnitTest.o
	$(CC) -o $@ $(SHARED_LIBRARIES) $^ $(CFLAGS)

SmithWatermanUnitTest.o: SmithWatermanUnitTest.c SmithWatermanUnitTest.h
	$(CC) -c $(CFLAGS) $<

.PHONY: clean cleanest

clean:
	rm *.o

cleanest: clean
	rm swpairwise
