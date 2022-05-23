compiler=gcc
flags=-std=gnu11 -march=native -Wall -Wno-missing-braces -O3


64bit: checkCell.c ../readGraph/readGraph6.c ../hamiltonicityMethods.c 
	$(compiler) -DUSE_64_BIT -o checkCell checkCell.c ../readGraph/readGraph6.c ../hamiltonicityMethods.c $(flags)

128bit: checkCell.c ../readGraph/readGraph6.c ../hamiltonicityMethods.c 
	$(compiler) -DUSE_128_BIT -o checkCell-128 checkCell.c ../readGraph/readGraph6.c ../hamiltonicityMethods.c $(flags)

128bitarray: checkCell.c ../readGraph/readGraph6.c ../hamiltonicityMethods.c 
	$(compiler) -DUSE_128_BIT_ARRAY -o checkCell-128a checkCell.c ../readGraph/readGraph6.c ../hamiltonicityMethods.c $(flags)

all: 64bit 128bit 128bitarray

.PHONY: clean
clean:
	rm -f checkCell checkCell-128 checkCell-128a

