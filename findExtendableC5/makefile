compiler=gcc
flags=-std=gnu11 -march=native -Wall -Wno-missing-braces -O3

64bit: findExtendableC5.c ../readGraph/readGraph6.c ../hamiltonicityMethods.c ../bitset.h
	$(compiler) -DUSE_64_BIT -o findExtendableC5 findExtendableC5.c ../readGraph/readGraph6.c ../hamiltonicityMethods.c $(flags)

128bit: findExtendableC5.c ../readGraph/readGraph6.c ../hamiltonicityMethods.c ../bitset.h
	$(compiler) -DUSE_128_BIT -o findExtendableC5-128 findExtendableC5.c ../readGraph/readGraph6.c ../hamiltonicityMethods.c $(flags)

128bitarray: findExtendableC5.c ../readGraph/readGraph6.c ../hamiltonicityMethods.c ../bitset.h
	$(compiler) -DUSE_128_BIT_ARRAY -o findExtendableC5-128a findExtendableC5.c ../readGraph/readGraph6.c ../hamiltonicityMethods.c $(flags)

all: 64bit 128bit 128bit

.PHONY: clean
clean: 
	rm -f findExtendableC5 findExtendableC5-128 findExtendableC5-128a