/**
 * findExtendableC5.c
 * 
 * Author: Jarne Renders (jarne.renders@kuleuven.be)
 *
 */

#define USAGE \
"Usage: `./findExtendableC5 [-a|-c#,#,#,#,#] [-pv] [-h]`"

#define HELPTEXT \
"Filter graphs that contain an extendable 5-cycle.\n\
\n\
Graphs are read from stdin in graph6 format. Graphs are sent to stdout in graph6\n\
format. If the input graph had a graph6 header, so will the output graph (if it\n\
passes through the filter).\n\
\n\
The order in which the arguments appear does not matter, unless multiple\n\
instances of `-c` are given (the lastmost instance will be chosen).\n\
\n\
    -a, --all\n\
        count all extendable 5-cycles in the graph; cannot be used\n\
        with -c\n\
    -c, --cycle\n\
        specify a cycle for which to check whether it is extendable; if it is\n\
        and -p is present this will be sent to stdout\n\
    -h, --help\n\
        print out help message\n\
    -p, --print\n\
        if a cycle is found it will be sent to stdout; with -a this will be the\n\
        last found, with -c this will be the specified cycle (if it is\n\
        extendable) and without these flags it will be the first found\n\
    -v, --verbose\n\
        verbose mode; for each checked extendable 5-cycle print out the paths\n\
        showing it is one\n"

#include <stdio.h>
#include <stdbool.h>
#include <getopt.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include "../bitset.h"
#include "../hamiltonicityMethods.h"
#include "../readGraph/readGraph6.h"

//	Returns whether or not the graph contains an extendable 5-cycle. A
//	5-cycles gets stored in C5. If verboseFlag is true the paths showing
//	that a 5-cycle is extendable are printed (if an extendable 5-cycle
//	exists). If allFlag is true all paths for all 5-cycles are printed. 
bool containsExtC5(bitset adjacencyList[], int nVertices, int C5[], bool
verboseFlag, bool allFlag);

//	Returns whether or not the given 5-cycle in C5 is extendable.
bool isExtendable(bitset adjacencyList[], int nVertices, int C5[]);

//	Prints the hamiltonian cycles showing that C5 is extendable. If the C5
//	is not an extendable cycle only the hamiltonian cycles which exist
//	get printed.
void printPaths(bitset adjacencyList[], int nVertices, int C5[]);

int main(int argc, char ** argv) {
	int opt;
	int C5[5];
	bool verboseFlag = false;
	bool cycleFlag = false;
	bool printCycleFlag = false;
	bool allFlag = false;
	while (1) {
    	int option_index = 0;
    	static struct option long_options[] = 
    	{
            {"all",                         no_argument, NULL, 'a'},
            {"cycle",                       required_argument, NULL, 'c'},
            {"help",                    no_argument, NULL,  'h'},
        	{"print", 						no_argument, NULL, 'p'},
            {"verbose",                     no_argument, NULL,  'v'},
        	{NULL,      					0, 			 NULL,    0}
    	};

    	opt = getopt_long(argc, argv, "ac:hpv", long_options, &option_index);
    	if (opt == -1) break;
		switch(opt) {
            case 'a':
                allFlag = true;
                break;
            case 'c':
                cycleFlag = true;
                char* string = optarg;
                for(int i = 0; i < 5; i++) {
                    char* token = strsep(&string, ",");
                    if(token == NULL) {
                        fprintf(stderr,
                         "Error: Invalid argument with -c.\n");
                        fprintf(stderr, "%s\n", USAGE);
                        fprintf(stderr,
                         "Use ./findExtendableC5 --help for more detailed instructions.\n");
                        return 1;
                    }
                    C5[i] = atoi(token);
                }
                char* token = strsep(&string, ",");
                if(token != NULL) {
                    fprintf(stderr, "Error: Only add 5 vertices.\n");
                    fprintf(stderr, "%s\n", USAGE);
                    fprintf(stderr,
                     "Use ./findExtendableC5 --help for more detailed instructions.\n");
                    return 1;
                }
                break;
            case 'h':
                fprintf(stderr, "%s\n", USAGE);
                fprintf(stderr, "%s", HELPTEXT);
                return 0;
                break;
            case 'p':
                printCycleFlag = true;
                break;
			case 'v':
				verboseFlag = true;
				break;
      		case ':':
       			fprintf(stderr,"Error: Missing arg for %c\n", optopt);
                fprintf(stderr, "%s\n", USAGE);
                fprintf(stderr,
                     "Use ./findExtendableC5 --help for more detailed instructions.\n");
           		return 1;
		}
	}

    if(allFlag && cycleFlag) {
        fprintf(stderr, "Error: do not use -a and -c simultaneously.\n");
        fprintf(stderr, "%s\n", USAGE);
        fprintf(stderr,
         "Use ./findExtendableC5 --help for more detailed instructions.\n");
        return 1;
    }

    if(MAXVERTICES != 64) {
		fprintf(stderr,
		 "Warning: For graphs up to 64 vertices, the 64 bit version of this program is faster.\n");
	}

	unsigned long long int counter = 0;
	unsigned long long int containExtC5 = 0;
	unsigned long long int skippedGraphs = 0;

	clock_t start = clock();

	char * graphString = NULL;
		size_t size;
		while(getline(&graphString, &size, stdin) != -1) {
			int nVertices = getNumberOfVertices(graphString);
	        if(nVertices == -1 || nVertices > MAXVERTICES) {
	            if(verboseFlag){
	                fprintf(stderr, "Skipping invalid graph!\n");
	            }
	            skippedGraphs++;
	            continue;
	        }
			bitset adjacencyList[nVertices];
	        if(loadGraph(graphString, nVertices, adjacencyList) == -1) {
	            if(verboseFlag){
	                fprintf(stderr, "Skipping invalid graph!\n");
	            }
	            skippedGraphs++;
	            continue;
	        }
			counter++;

			if(verboseFlag || allFlag) {
				fprintf(stderr, "Looking at graph %s", graphString);
				if(cycleFlag) {
					fprintf(stderr, "Checking whether ");
					for(int i = 0; i < 5; i++) {
						fprintf(stderr, "%d ", C5[i]);
					}
					fprintf(stderr, "is an extendable 5-cycle.\n");
				}
			}

			// 	Only test the given cycle.
			if(cycleFlag) {
				if(isExtendable(adjacencyList, nVertices, C5)) {
					containExtC5++;
					if(verboseFlag) {
						printPaths(adjacencyList,nVertices,C5);
					}
					printf("%s",graphString);
					if(printCycleFlag) {
						printf("%d %d %d %d %d\n",
						 C5[0], C5[1], C5[2], C5[3], C5[4]);
					}
				}
				continue;
			}

			if(containsExtC5(adjacencyList, nVertices, C5, verboseFlag,
			 allFlag)) {
				containExtC5++;
				printf("%s",graphString);
				if(printCycleFlag) {
					printf("%d %d %d %d %d\n",
					 C5[0], C5[1], C5[2], C5[3], C5[4]);
				}
			}
			else
				if(verboseFlag)
					fprintf(stderr,
					 "  Does not contain an extendable 5-cycle.\n\n");

		}

	clock_t end = clock();
	double time_spent = (double)(end - start) / CLOCKS_PER_SEC;
	fprintf(stderr,"\rChecked %lld graphs in %f seconds: ", counter, time_spent);
	fprintf(stderr, "%lld contain an extendable five cycle.\n", containExtC5);
	if(skippedGraphs) {
        fprintf(stderr, "Warning: %lld graphs could not be read.\n",
         skippedGraphs);
    }

	return 0;
}

bool containsExtC5(bitset adjacencyList[], int nVertices, int C5[], bool
verboseFlag, bool allFlag) {

	int countExtendableCycles = 0;
	for(int i = 0; i < nVertices; i++) {
		if(size(adjacencyList[i]) != 3) {
			continue;
		}
		forEachAfterIndex(C1, adjacencyList[i], i) {
			if(size(adjacencyList[C1]) != 3) {
				continue;
			}
			forEachAfterIndex(C4, adjacencyList[i], C1) {
				if(size(adjacencyList[C4]) != 3) {
					continue;
				}
				forEachAfterIndex(C2, adjacencyList[C1], i) {
					if(size(adjacencyList[C2]) != 3) {
						continue;
					}
					forEach(C3, intersection(adjacencyList[C2],
					 adjacencyList[C4])) {
						if(size(adjacencyList[C3]) != 3) {
							continue;
						}
						C5[0] = i; C5[1] = C1; C5[2] = C2; C5[3] = C3;
						 C5[4] = C4;
						if(isExtendable(adjacencyList, nVertices, C5)){
							if(verboseFlag) {
								printPaths(adjacencyList,nVertices,C5);
                                fprintf(stderr,
                                 "Extendable 5-cycle:\n %d -> %d -> %d -> %d -> %d\n", 
                                    C5[0], C5[1], C5[2], C5[3], C5[4]);
                            }
							if(!allFlag)
								return true;
							else {
								countExtendableCycles++;
                            }
						}
					}
				}
			}
		}
	}
    if(allFlag) {
        fprintf(stderr, "  Graph contains %d extendable 5-cycles.\n",
         countExtendableCycles);
    }
	return countExtendableCycles;
}

bool isExtendable(bitset adjacencyList[], int nVertices, int C5[]) {
	bitset path = union(singleton(C5[0]), singleton(C5[1]));
	for(int i = 2; i < 5; i++) {
		add(path, C5[i]);
	}

	for(int i = 0; i < 5; i++) { 

		// Check if G - w'_i contains ham cycle without w'_i-2w'_i+2.
		removeElement(adjacencyList[C5[(i+3)%5]], C5[(i+2)%5]);
		removeElement(adjacencyList[C5[(i+2)%5]], C5[(i+3)%5]);
		bool isHam = isHamiltonian(adjacencyList, nVertices, singleton(C5[i]),
         false, false);
		add(adjacencyList[C5[(i+3)%5]], C5[(i+2)%5]);
		add(adjacencyList[C5[(i+2)%5]], C5[(i+3)%5]);
		if(!isHam) {
			return false;
		}

		// Check if G - w''_i contains ham cycle with the path
        // w'_i-2w'_i-1w'_iw'_i+1w'_i+2.
		bitset remainingVertices = complement(union(path, adjacencyList[C5[i]]),
         nVertices);
		if(!canBeHamiltonian(adjacencyList, remainingVertices, C5[(i+3)%5],
         C5[(i+2)%5], nVertices -1 , 5)) {
			return false;
		}
	}
	return true;
}

void printPaths(bitset adjacencyList[], int nVertices, int C5[]) {
	bitset path = union(singleton(C5[0]), singleton(C5[1]));
	for(int i = 2; i < 5; i++) {
		add(path, C5[i]);
	}
	for(int i=0; i < 5; i++) {
		fprintf(stderr, "G - %d: ", C5[i]);
		removeElement(adjacencyList[C5[(i+3)%5]], C5[(i+2)%5]);
		removeElement(adjacencyList[C5[(i+2)%5]], C5[(i+3)%5]);
		isHamiltonian(adjacencyList, nVertices, singleton(C5[i]), false, true);
		add(adjacencyList[C5[(i+3)%5]], C5[(i+2)%5]);
		add(adjacencyList[C5[(i+2)%5]], C5[(i+3)%5]);

		bitset remainingVertices = complement(union(path, adjacencyList[C5[i]]),
         nVertices);
		int pathList[nVertices - 1];
		pathList[0] = C5[(i+2)%5];
		pathList[1] = C5[(i+1)%5];
		pathList[2] = C5[(i+0)%5];
		pathList[3] = C5[(i+4)%5];
		pathList[4] = C5[(i+3)%5];
		int numberOfHamiltonianCycles = 0;
		forEach(neighbour, difference(adjacencyList[C5[i]],path)){
			fprintf(stderr, "G - %d: ", neighbour);
			canBeHamiltonianPrintCycle(adjacencyList, remainingVertices,
             pathList ,C5[(i+3)%5], C5[(i+2)%5], nVertices - 1, 5,
              &numberOfHamiltonianCycles,false, true);
		}
	}
}
