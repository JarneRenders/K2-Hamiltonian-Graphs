compiler=gcc
flags=-std=gnu11 -march=native -Wall -Wno-missing-braces -O3

64bit: satisfiesDotProduct.c ../readGraph/readGraph6.c ../hamiltonicityMethods.c
	$(compiler) -DUSE_64_BIT -o satisfiesDotProduct satisfiesDotProduct.c ../readGraph/readGraph6.c ../hamiltonicityMethods.c $(flags)

128bit: satisfiesDotProduct.c ../readGraph/readGraph6.c ../hamiltonicityMethods.c
	$(compiler) -DUSE_128_BIT -o satisfiesDotProduct-128 satisfiesDotProduct.c ../readGraph/readGraph6.c ../hamiltonicityMethods.c $(flags)

128bitarray: satisfiesDotProduct.c ../readGraph/readGraph6.c ../hamiltonicityMethods.c
	$(compiler) -DUSE_128_BIT_ARRAY -o satisfiesDotProduct-128a satisfiesDotProduct.c ../readGraph/readGraph6.c ../hamiltonicityMethods.c $(flags)

all: 64bit 128bit 128bitarray

.PHONY: clean
clean:
	rm -f satisfiesDotProduct satisfiesDotProduct-128 satisfiesDotProduct-128a

