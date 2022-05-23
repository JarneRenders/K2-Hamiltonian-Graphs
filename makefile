compiler=gcc
flags=-std=gnu11 -march=native -Wall -Wno-missing-braces -O3

# The 64-bit version of this program is faster but only supports graphs up to 64 vertices.
64bit: hamiltonicityChecker.c readGraph/readGraph6.c hamiltonicityMethods.c bitset.h
	$(compiler) -DUSE_64_BIT -o hamiltonicityChecker hamiltonicityChecker.c readGraph/readGraph6.c hamiltonicityMethods.c $(flags)

# There are two different implementations of the 128-bit version. The array version generally performs faster.
128bit: hamiltonicityChecker.c readGraph/readGraph6.c hamiltonicityMethods.c bitset.h
	$(compiler) -DUSE_128_BIT -o hamiltonicityChecker-128 hamiltonicityChecker.c readGraph/readGraph6.c hamiltonicityMethods.c $(flags)

128bitarray: hamiltonicityChecker.c readGraph/readGraph6.c hamiltonicityMethods.c bitset.h
	$(compiler) -DUSE_128_BIT_ARRAY -o hamiltonicityChecker-128a hamiltonicityChecker.c readGraph/readGraph6.c hamiltonicityMethods.c $(flags)	

all: 64bit 128bit 128bitarray

.PHONY: clean
clean:
	rm -f hamiltonicityChecker hamiltonicityChecker-128 hamiltonicityChecker-128a

