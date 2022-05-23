/**
 * checkCell.c
 * 
 * Author: Jarne Renders (jarne.renders@kuleuven.be)
 *
 */

#define USAGE \
"Usage: `./checkCell [-1|2] [-psSv] [-o#,#,#,#] [-h]`"

#define HELPTEXT \
"Filter graphs that satisfy the conditions for being a suitable cell, a\n\
K1-cell or a K2-cell.\n\
\n\
Graphs are read from stdin in graph6 format. Graphs are sent to stdout in\n\
graph6 format. If the input graph had a graph6 header, so will the output\n\
graph (if it passes through the filter).\n\
\n\
The order in which the arguments appear does not matter, unless multiple\n\
instances of `-o` are given (the lastmost instance will be chosen).\n\
\n\
    -1, --k1cell\n\
            let the graphs which contain some outer vertices that form a\n\
            K1-cell pass through the filter; if -s is present the graph with\n\
            the specified outer vertices is assumed to be suitable; do not\n\
            use with -2\n\
    -2, --k2cell\n\
            let the graphs which contain some outer vertices that form a\n\
            K2-cell pass through the filter; if -s is present the graph with\n\
            the specified outer vertices is assumed to be suitable; do not\n\
            use with -1\n\
    -h, --help\n\
            print out help message\n\
    -o, --outer-vertices\n\
            will only do the checks for the specified outer vertices;\n\
            use -o#,#,#,# where # represents an outer vertex; these need to\n\
            be distinct and in the graph; suitability of all permutations of\n\
            these vertices will be checked; if combining with -s the order\n\
            does matter\n\
    -p, --paths\n\
            print the paths necessary to show that a suitable cell is K1 or\n\
            K2; if it is not all paths will be printed until the point of\n\
            failure; best combined with -v\n\
    -s, --suitable\n\
            skip the suitability check and assume all input graphs contain a\n\
            suitable cell at the specified vertices; this option requires -o\n\
    -S, --statistics\n\
            print out which conditions of the suitability check failed how\n\
            many percent of the time and how much percent of the computation\n\
            they took\n\
    -v, --verbose\n\
            verbose mode; print out which vertices belong to the suitable cell\n\
            and where the point of failure is in the K1- or K2-cell\n\
            computation (if applicable)\n"


#include <stdio.h>
#include <stdbool.h>
#include <getopt.h>
#include <time.h>
#include "../bitset.h"
#include "../hamiltonicityMethods.h"
#include "../readGraph/readGraph6.h"

bool isSuitable(bitset adjacencyList[], int numberOfVertices, int
outerVertices[]);

bool containsSuitableCell(bitset adjacencyList[], int numberOfVertices, bool
verboseFlag, int outerVertices[]);

bool isK1Cell(bitset adjacencyList[], int numberOfVertices, int outerVertices
[], bool verboseFlag, bool pathFlag);

bool isK2Cell(bitset adjacencyList[], int numberOfVertices, int outerVertices
[], bool verboseFlag, bool pathFlag);

void takeStatistics(bitset adjacencyList[], int numberOfVertices, int
outerVertices[], unsigned long long int statistics[], double timeStatistics
[]);


int main(int argc, char ** argv) {
	int outerVertices[4];
	bool outerVerticesDefined = false;
	bool verboseFlag = false;
	bool suitableFlag = false;
	bool k1cellFlag = false;
	bool k2cellFlag = false;
	bool pathFlag = false;
	bool statisticsFlag = false;
	int opt;
	while (1) {
    	int option_index = 0;
    	static struct option long_options[] = 
    	{
    		{"k1cell", 					no_argument, NULL,	'1'},
        	{"k2cell", 					no_argument, NULL,	'2'},
            {"help",                    no_argument, NULL,  'h'},
        	{"outer-vertices", 	  required_argument, NULL, 	'o'},
        	{"paths", 					no_argument, NULL, 	'p'},
        	{"suitable", 				no_argument, NULL,	's'},
        	{"statistics", 				no_argument, NULL,  'S'},
        	{"verbose", 				no_argument, NULL,  'v'},
        	{NULL,      				0, 			 NULL,    0}
    	};

    	opt = getopt_long(argc, argv, "12ho:psSv", long_options, &option_index);
    	if (opt == -1) break;
		switch(opt) {
			case '1':
				k1cellFlag = true;
				break;
			case '2':
				k2cellFlag = true;
				break;
            case 'h':
                fprintf(stderr, "%s\n", USAGE);
                fprintf(stderr, "%s", HELPTEXT);
                return 0;
                break;
        	case 'o':
        		if(!(sscanf(optarg, "%d,%d,%d,%d", &outerVertices[0],
        		 &outerVertices[1], &outerVertices[2], &outerVertices[3]) == 4)) {
        			fprintf(stderr,
                     "Error: invalid input. Example: -o2,3,11,13\n");
                    fprintf(stderr, "%s\n", USAGE);
                    fprintf(stderr,
                     "Use ./checkCell --help for more detailed instructions.\n");
        			return 1;
        		}
        		if((outerVertices[0] == outerVertices[1] || outerVertices[0] == outerVertices[2] ||
        		 outerVertices[0] == outerVertices[3] || outerVertices[1] == outerVertices[2] ||
        		 outerVertices[1] == outerVertices[3] || outerVertices[2] == outerVertices[3])) {
					fprintf(stderr, "Outer vertices need to be distinct.\n");
                    fprintf(stderr,
                     "Use ./checkCell --help for more detailed instructions.\n");
					return 1;
				}
				outerVerticesDefined = true;
        		break;
        	case 'p':
				pathFlag = true;
				break;
			case 's':
				suitableFlag = true;
				break;
			case 'S':
				statisticsFlag = true;
				break;
			case 'v':
				verboseFlag = true;
				break;
		}
	}

	if(k1cellFlag && k2cellFlag) {
		fprintf(stderr, "Error: do not use both -1 and -2.\n");
        fprintf(stderr, "%s\n", USAGE);
        fprintf(stderr,
         "Use ./checkCell --help for more detailed instructions.\n");
		return 1;
	}

	if(!outerVerticesDefined){
		if(suitableFlag) {
			fprintf(stderr, "This option requires -o. \n");
            fprintf(stderr, "%s\n", USAGE);
            fprintf(stderr,
             "Use ./checkCell --help for more detailed instructions.\n");
			return 1;
		}
		fprintf(stderr,
		 "No outer vertices supplied. Computing all suitable combinations.\n");
	}

	if(MAXVERTICES != 64) {
		fprintf(stderr,
		 "Warning: For graphs up to 64 vertices, the 64 bit version of this program is faster.\n");
	}

	unsigned long long int counter = 0;
	unsigned long long int suitableCounter = 0;
	unsigned long long int K1CellCounter = 0;
	unsigned long long int K2CellCounter = 0;
	unsigned long long int skippedGraphs = 0;

	unsigned long long int statistics[5] = {0};
	double timeStatistics[5] = {0};

	clock_t start = clock();


	//	Loop over graphs.
	char * graphString = NULL;
	size_t size;
	while(getline(&graphString, &size, stdin) != -1) {
		if(verboseFlag) {
			fprintf(stderr, "Looking at %s",graphString);
		}
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

		if (outerVerticesDefined) {
			if(outerVertices[0] >= nVertices || outerVertices[1] >= nVertices ||
		 	 outerVertices[2] >= nVertices || outerVertices[3] >= nVertices) {
				fprintf(stderr,
			 	 "Error: Outer vertices need to lie in the graph.\n");
                fprintf(stderr, "%s\n", USAGE);
                fprintf(stderr,
                 "Use ./checkCell --help for more detailed instructions.\n");
				return 1;
			}
		}

		bool suitable;
		if(suitableFlag){
			suitable = true;
		}
		else if (!outerVerticesDefined) {
			for(int i = 0; i < 4; i++) {
				outerVertices[i] = -1;
			}
			suitable = containsSuitableCell(adjacencyList, nVertices,
			 verboseFlag, outerVertices);
			if(statisticsFlag) {
				takeStatistics(adjacencyList,nVertices,outerVertices,
				 statistics, timeStatistics);
			}
			if(suitable && !(k1cellFlag || k2cellFlag)) {
				printf("%s", graphString);
			}
		}
		else {
			suitable = isSuitable(adjacencyList, nVertices, outerVertices);
			if(!k1cellFlag && !k2cellFlag){
				if(suitable) {
					printf("%s", graphString);
				}
			}
		}

		if(!suitable) { 
			if(verboseFlag) {
				fprintf(stderr, "  The graph is not suitable\n");
			}
			continue; 
		}

		// At this point the graph is (assumed to be) suitable and we have
		// fixed four outer vertices.
		suitableCounter++;

		if(!k1cellFlag && !k2cellFlag) {
			continue;
		}

		bool foundCell = false;
		bool foundOuterVertices = true;
		while(!outerVerticesDefined && foundOuterVertices) {

			// Loop over all possible suitable cells in the graph and see if
			// they are K1- or K2-cells.
			if(k1cellFlag && isK1Cell(adjacencyList, nVertices, outerVertices,
			 verboseFlag, pathFlag)) {
				K1CellCounter++;
				foundCell = true;
				printf("%s", graphString);
				if(verboseFlag) {
					fprintf(stderr,"  (G,%d,%d,%d,%d) is a K1-cell.\n\n",
			 	 	 outerVertices[0],outerVertices[1],outerVertices[2],outerVertices[3]);
				}
				else {

					// If not verboseFlag, we are happy with one K1-cell.
					break;
				}
			}
			else if(k2cellFlag && isK2Cell(adjacencyList, nVertices, outerVertices,
				verboseFlag, pathFlag)) {
				K2CellCounter++;
				foundCell = true;
				printf("%s", graphString);
				if(verboseFlag) {
					fprintf(stderr,"  (G,%d,%d,%d,%d) is a K2-cell.\n\n",
			 	 	 outerVertices[0],outerVertices[1],outerVertices[2],outerVertices[3]);
				}
				else {

					// If not verboseFlag, we are happy with one K1-cell.
					break;
				}
			}

			//	If not a K1-cell or a K2-cell or if we want to find all
			//	with -v: find new suitable cell.
			foundOuterVertices = containsSuitableCell(adjacencyList, nVertices,
			 verboseFlag, outerVertices);
		}
		if(verboseFlag && !foundCell) {
			fprintf(stderr, "  The graph is not a %s-cell.\n", k1cellFlag ? "K1" : "K2");
		}
	}
	clock_t end = clock();
	double time_spent = (double)(end - start) / CLOCKS_PER_SEC;

	fprintf(stderr,
	 "Checked %lld graphs in %f seconds. %lld graphs contain a suitable cell.\n",
	  counter, time_spent,suitableCounter);
	if(k1cellFlag) {
		fprintf(stderr, "%lld contain a K1-cell.\n", K1CellCounter);
	}
	if(k2cellFlag) {
		fprintf(stderr, "%lld contain a K2-cell.\n", K2CellCounter);
	}
	if(statisticsFlag) {
		fprintf(stderr,
		 "Suitability property 1.1 and 1.2 failed %lld %% of the time and took %f %% of the time.\n",
		 statistics[1] ? statistics[1]*100/statistics[0] : 0,
		 timeStatistics[1] ? timeStatistics[1]*100/timeStatistics[0] : 0);
		fprintf(stderr,
		 "Suitability property 1.3 and 1.4 failed %lld %% of the time and took %f %% of the time.\n",
		 statistics[2] ? statistics[2]*100/statistics[0] : 0,
		 timeStatistics[2] ? timeStatistics[2]*100/timeStatistics[0] : 0);
		fprintf(stderr,
		 "Suitability property 1.5 failed %lld %% of the time and took %f %% of the time.\n",
		 statistics[3] ? statistics[3]*100/statistics[0] : 0,
		 timeStatistics[3] ? timeStatistics[3]*100/timeStatistics[0] : 0);
		fprintf(stderr,
		 "Suitability property 1.6 failed %lld %% of the time and took %f %% of the time.\n",
		 statistics[4] ? statistics[4]*100/statistics[0] : 0,
		 timeStatistics[4] ? timeStatistics[4]*100/timeStatistics[0] : 0);
	}

	if(skippedGraphs) {
        fprintf(stderr, "Warning: %lld graphs could not be read.\n",
         skippedGraphs);
    }
	return 0;
}

//	Check whether there exists a hamiltonian (a,b)-path in the (sub)graph.
bool isGoodPair(bitset adjacencyList[], int numberOfVertices, bitset
excludedVertices, int a, int b, bool verboseFlag) {
    return containsHamiltonianPathWithEnds(adjacencyList, numberOfVertices,
     excludedVertices, a, b, false, verboseFlag);
}

//	Check whether there exists an ab-path and a cd-path whose vertex sets
//	partition the vertices of the (sub)graph.
bool isGoodPairOfPairs(bitset adjacencyList[], int numberOfVertices, int a,
int b, int c, int d, bitset excludedVertices, bool verboseFlag) {
    if(contains(excludedVertices, a) || contains(excludedVertices, b) ||
     contains(excludedVertices,c) || contains(excludedVertices,d)) {
        if(verboseFlag)
            fprintf(stderr, "Cannot exclude vertices in the pair of pairs!\n");
        return false;
    }
    return (containsDisjointSpanningPathsWithEnds(adjacencyList,
     numberOfVertices, excludedVertices, a, b, EMPTY, c, d, EMPTY, false,
     verboseFlag));
}

//	Permute the three final outerVertices according to permutation. Where
//	permutation is the radix mapping of the Lehmer code of the permutation.
void permute(int outerVertices[], int permutation) {

    for(int n = permutation/2; n > 0; n--){
        int temp = outerVertices[n+1];
        outerVertices[n+1] = outerVertices[n];
        outerVertices[n] = temp;
    }
    if (permutation%2 == 1) {
        int temp = outerVertices[3];
        outerVertices[3] = outerVertices[2];
        outerVertices[2] = temp;
    }
}

//	This method checks whether the graph can satisfy properties 1 or 2 of
//	suitable cells for some permutation of the outer vertices. We check
//	for each pair of outer vertices, whether or not it is good, whilst
//	eliminating the permutations of outer vertices which cannot make the
//	graph a suitable cell.
bool satisfiesProperty1and2(bitset adjacencyList[], int numberOfVertices,
bitset* possiblePermutations, int outerVertices[]) {

	int loopCounter = 0;

    // Property 1.1 and 1.2:
    for(int i = 0; i < 4; i++) {
        for (int j = i+1; j < 4; j++) {

        	if(loopCounter == 0 || loopCounter == 5) { // (0,1) or (2,3)
        		if (isGoodPair(adjacencyList,numberOfVertices, EMPTY,
        		 outerVertices[i], outerVertices[j], false)) {
        			*possiblePermutations = difference(*possiblePermutations, 
        			 union(singleton(3), singleton(5)));
        		}
        		else {
        			*possiblePermutations = intersection(*possiblePermutations,
        			 union(singleton(3), singleton(5)));

        		}
        	}
        	else if(loopCounter == 1 || loopCounter == 4) { // (0,2) or (1,3)
        		if (isGoodPair(adjacencyList,numberOfVertices, EMPTY,
        		 outerVertices[i], outerVertices[j], false)) {
        			*possiblePermutations = difference(*possiblePermutations,
        			 union(singleton(1), singleton(4)));
        		}
        		else {
        			*possiblePermutations = intersection(*possiblePermutations,
        			 union(singleton(1), singleton(4)));

        		}
        	}
        	else { // (0,3) or (1,2)
        		if (isGoodPair(adjacencyList,numberOfVertices, EMPTY,
        		 outerVertices[i], outerVertices[j], false)) {
        			*possiblePermutations = difference(*possiblePermutations,
        			 union(singleton(0), singleton(2)));
        		}
        		else {
        			*possiblePermutations = intersection(*possiblePermutations,
        			 union(singleton(0), singleton(2)));

        		}
        	}

            if(size(*possiblePermutations) == 0) {
                return false;
            }
            loopCounter++;
        }
    }
    return true;
}

//	This method checks whether the graph can satisfy properties 1 or 2 of
//	suitable cells for some permutation of the outer vertices. We check
//	of pair of pairs ((0,1),(2,3)), ((0,2),(1,3)) and ((0,3),
//	(1,2)) whether the pair of pairs is good or not, whilst eliminating
//	the permutations of outer vertices which cannot make the graph a
//	suitable cell.
bool satisfiesProperty3and4(bitset adjacencyList[], int numberOfVertices,
bitset* possiblePermutations, int outerVertices[]) {

    //  Property 1.3 and 1.4
    if(isGoodPairOfPairs(adjacencyList,numberOfVertices,
     outerVertices[0],outerVertices[1],outerVertices[2],outerVertices[3],
     EMPTY, false)) {
        *possiblePermutations =
         intersection(*possiblePermutations, union(singleton(0), singleton(1)));
    }
    else {
        *possiblePermutations =
         difference(*possiblePermutations, union(singleton(0), singleton(1)));
    }
    if(size(*possiblePermutations) == 0) {
        return false;
    }

    if(isGoodPairOfPairs(adjacencyList,numberOfVertices,
     outerVertices[0],outerVertices[2],outerVertices[1],outerVertices[3],
     EMPTY, false)) {
        *possiblePermutations =
         intersection(*possiblePermutations, union(singleton(2), singleton(3)));
    }
        else {
        *possiblePermutations =
         difference(*possiblePermutations, union(singleton(2), singleton(3)));
    }
    if(size(*possiblePermutations) == 0) {
        return false;
    }

    if(isGoodPairOfPairs(adjacencyList,numberOfVertices,
     outerVertices[0],outerVertices[3],outerVertices[1],outerVertices[2],
     EMPTY, false)) {
        *possiblePermutations =
         intersection(*possiblePermutations, union(singleton(4), singleton(5)));
    }
    else {
        *possiblePermutations =
         difference(*possiblePermutations, union(singleton(4), singleton(5)));
    }
    if(size(*possiblePermutations) == 0) {
        return false;
    }
    return true;
}

//	This method checks whether all pairs of outerVertices are bad in G - v
//	with v any outer vertex.
bool satisfiesProperty5(bitset adjacencyList[], int numberOfVertices, int
outerVertices[]) {
    //  Property 1.5(a) and 1.5(b):
    for(int v = 0; v < 4; v++) {
        for(int j = 0; j < 4; j++) {
            for (int k = j+1; k < 4; k++) {
                if (isGoodPair(adjacencyList,numberOfVertices,
                 singleton(outerVertices[v]), outerVertices[j],
                 outerVertices[k], false)) {
                    return false;
                }
            }
        }
    }
    return true;
}

//	This method checks whether the remaining pair of outer vertices is good
//	in G - (v,w) for (v,w) a pair of outer vertices in {(0,2), (0,3),
//	(1,2), (1,3)}. If this does not hold we eliminate the permutations
//	for which it should.
bool satisfiesProperty6(bitset adjacencyList[], int numberOfVertices, bitset*
possiblePermutations, int outerVertices[]) {

    //  Property 1.6:
    if (!isGoodPair(adjacencyList, numberOfVertices, 
     union(singleton(outerVertices[0]), singleton(outerVertices[2])),
     outerVertices[1], outerVertices[3], false)) {
        *possiblePermutations = 
    	 intersection(*possiblePermutations, union(singleton(2),singleton(3)));
        if(size(*possiblePermutations) == 0) {
            return false;
        }
    }
    if (!isGoodPair(adjacencyList, numberOfVertices,
     union(singleton(outerVertices[1]), singleton(outerVertices[3])),
     outerVertices[0], outerVertices[2], false)) {
        *possiblePermutations =
         intersection(*possiblePermutations, union(singleton(2), singleton(3)));
        if(size(*possiblePermutations) == 0) {
            return false;
        }
    }
    if (!isGoodPair(adjacencyList, numberOfVertices,
     union(singleton(outerVertices[0]), singleton(outerVertices[1])),
     outerVertices[2], outerVertices[3], false)) {
        *possiblePermutations =
         intersection(*possiblePermutations, union(singleton(0), singleton(1)));
        if(size(*possiblePermutations) == 0) {
            return false;
        }
    }
    if (!isGoodPair(adjacencyList, numberOfVertices,
     union(singleton(outerVertices[2]), singleton(outerVertices[3])),
     outerVertices[0], outerVertices[1], false)) {
        *possiblePermutations =
         intersection(*possiblePermutations, union(singleton(0), singleton(1)));
        if(size(*possiblePermutations) == 0) {
            return false;
        }
    }

    if (!isGoodPair(adjacencyList, numberOfVertices,
     union(singleton(outerVertices[1]), singleton(outerVertices[2])), 
     outerVertices[0], outerVertices[3], false)) {
        *possiblePermutations =
         intersection(*possiblePermutations, union(singleton(4),singleton(5)));
        if(size(*possiblePermutations) == 0) {
            return false;
        }
    }
    if (!isGoodPair(adjacencyList, numberOfVertices,
     union(singleton(outerVertices[0]), singleton(outerVertices[3])),
     outerVertices[1], outerVertices[2], false)) {
        *possiblePermutations =
         intersection(*possiblePermutations, union(singleton(4), singleton(5)));
        if(size(*possiblePermutations) == 0) {
            return false;
        }
    }
    return true;
}

// Given four outer vertices return whether or not some permutation of these
// makes the graph a suitable cell.
bool isSuitable(bitset adjacencyList[], int numberOfVertices, int
outerVertices[]) { bitset possiblePermutations = complement(EMPTY, 6);

    if(!satisfiesProperty6(adjacencyList, numberOfVertices,
     &possiblePermutations, outerVertices)) {
        return false;
    }

    if(!satisfiesProperty1and2(adjacencyList, numberOfVertices,
     &possiblePermutations, outerVertices)) {
        return false;
    }

    if(!satisfiesProperty3and4(adjacencyList, numberOfVertices,
     &possiblePermutations, outerVertices)) {
        return false;
    }

    if(!satisfiesProperty5(adjacencyList, numberOfVertices, outerVertices)) {
        return false;
    }

    int permutation = next(possiblePermutations, -1); 
    permute(outerVertices, permutation);

    return true;
}

#define minimum(x,y,z) (((x) <= (y)) ?\
							(((x) <= (z)) ? (x) : (z)) :\
							(((y) <= (z)) ? (y) : (z)) ) 
#define maximum(x,y,z) (((x) < (y)) ?\
							(((y) < (z)) ? (z) : (y)) :\
							(((x) < (z)) ? (z) : (x)) )

// Determine whether the given graph contains some combination of four
// vertices for which a permutation of these makes the graph into a suitable
// cell.
bool containsSuitableCell(bitset adjacencyList[], int numberOfVertices, bool
verboseFlag, int outerVertices[]){

	int outerVerticesCopy[4];
	int oldI = outerVertices[0];
	int oldJ = minimum(outerVertices[1], outerVertices[2], outerVertices[3]);
	int oldL = maximum(outerVertices[1], outerVertices[2], outerVertices[3]);
	int oldK = -1;
	for (int i = 1; i < 4; i++) {
		if(outerVertices[i] != oldJ && outerVertices[i] != oldL) {
			oldK = outerVertices[i];
			break;
		}
	}
	// outerVertices[0] is always in the same position, since we only permute
	// the final three. Loop over all increasing combinations of vertices in
	// the graph.
    for(int i = (0 < oldI ? oldI : 0); i < numberOfVertices; i++){
        for(int j = i+1; j < numberOfVertices; j++) {
            for(int k = j+1; k < numberOfVertices; k++) {
                for (int l = k+1; l < numberOfVertices; l++) { 

                	//	Skip if we already saved these outerVertices
                	//	(sadly some iterations are repeated in this way).
                	if(i == outerVertices[0] && j == oldJ && k == oldK && l == oldL) {
                		continue;
                	}
                    outerVerticesCopy[0] = i;
                    outerVerticesCopy[1] = j;
                    outerVerticesCopy[2] = k;
                    outerVerticesCopy[3] = l;
                    if(isSuitable(adjacencyList, numberOfVertices, outerVerticesCopy)){
                        if(verboseFlag) {
                            fprintf(stderr, "a: %d b: %d c: %d d: %d\n",
                             outerVerticesCopy[0], outerVerticesCopy[1],
                             outerVerticesCopy[2], outerVerticesCopy[3]);
                        }

                        //	The copy might be permuted in isSuitable.
                        outerVertices[0] = outerVerticesCopy[0];
	                    outerVertices[1] = outerVerticesCopy[1];
	                    outerVertices[2] = outerVerticesCopy[2];
	                    outerVertices[3] = outerVerticesCopy[3];
                        return true;
                    }
                }
            }
        }
    }
    for(int i = 0; i < 4; i++) {
    	outerVertices[i] = -1;
    }
    return false;
}

//	Count how many times certain properties fail and how long is spent
//	computing these properties. Gets called if -v is present. This shows
//	that checking order 6- > 1,2 -> 3,4 -> 5 is most efficient.
void takeStatistics(bitset adjacencyList[], int numberOfVertices, int
outerVertices[], unsigned long long int statistics[], double timeStatistics
[]) {

	//	Loop over increasing vertices in the graph.
    for(int i = 0; i < numberOfVertices; i++){
        for(int j = i+1; j < numberOfVertices; j++) {
            for(int k = j+1; k < numberOfVertices; k++) {
                for (int l = k+1; l < numberOfVertices; l++) { 
                    outerVertices[0] = i;
                    outerVertices[1] = j;
                    outerVertices[2] = k;
                    outerVertices[3] = l;
                    statistics[0]++;

                    clock_t start = clock();
                    bitset possiblePermutations = complement(EMPTY, 6);
                    if(!satisfiesProperty1and2(adjacencyList, numberOfVertices,
                     &possiblePermutations, outerVertices)) {
                        statistics[1]++;
                    }
                    clock_t end = clock();
                    timeStatistics[0] += (double)(end - start) / CLOCKS_PER_SEC;
                    timeStatistics[1] += (double)(end - start) / CLOCKS_PER_SEC;

                    start = clock();
                    possiblePermutations = complement(EMPTY, 6);
                    if(!satisfiesProperty3and4(adjacencyList, numberOfVertices,
                     &possiblePermutations, outerVertices)) {
                        statistics[2]++;
                    }
                    end = clock();
                    timeStatistics[0] += (double)(end - start) / CLOCKS_PER_SEC;
                    timeStatistics[2] += (double)(end - start) / CLOCKS_PER_SEC;

                    start = clock();
                    possiblePermutations = complement(EMPTY, 6);
                    if(!satisfiesProperty5(adjacencyList, numberOfVertices,
                     outerVertices)) {
                        statistics[3]++;
                    }
                    end = clock();
                    timeStatistics[0] += (double)(end - start) / CLOCKS_PER_SEC;
                    timeStatistics[3] += (double)(end - start) / CLOCKS_PER_SEC;

                    start = clock();
                    possiblePermutations = complement(EMPTY, 6);
                    if(!satisfiesProperty6(adjacencyList, numberOfVertices,
                     &possiblePermutations, outerVertices)) {
                        statistics[4]++;
                    }
                    end = clock();
                    timeStatistics[0] += (double)(end - start) / CLOCKS_PER_SEC;
                    timeStatistics[4] += (double)(end - start) / CLOCKS_PER_SEC;
                }
            }
        }
    }
}

//	Given a suitable cell, it checks whether one of the bad pairs or pairs
//	of pairs turns good in the subgraph G - excludedVertices.
bool badPairBecomesGoodInSubgraph(bitset adjacencyList[], int
numberOfVertices, int a, int b, int c, int d, bitset excludedVertices, bool
verboseFlag, bool pathFlag){

    //1.2
    if(isGoodPair(adjacencyList,numberOfVertices, excludedVertices, a, d,
     pathFlag)) {
        if(pathFlag) fprintf(stderr,"\t1.2 (a,d)\n");
        return true;
    }
    if(isGoodPair(adjacencyList,numberOfVertices,excludedVertices, b, c,
     pathFlag)) {
        if(pathFlag) fprintf(stderr,"\t1.2 (b,c)\n");
        return true;
    }

    //1.4
    if(isGoodPairOfPairs(adjacencyList,numberOfVertices, a,d,b,c,
     excludedVertices, pathFlag)) {
        if(pathFlag) fprintf(stderr,"\t1.4 ((a,d),(b,c))\n");
        return true;
    }
    if(isGoodPairOfPairs(adjacencyList,numberOfVertices,a,c,b,d, 
     excludedVertices, pathFlag)) {
        if(pathFlag) fprintf(stderr,"\t1.4 ((a,c),(b,d))\n");
        return true;
    }

    //1.5(a)
    int array[4] = {a,b,c,d};
    for(int j = 0; j < 4; j++) {
        if(isGoodPair(adjacencyList, numberOfVertices, 
         union(excludedVertices, singleton(array[j])), a,b, pathFlag)) {
            if(pathFlag) fprintf(stderr,"\t1.5 (a,b), outer vertex: %d\n",
             array[j]);
            return true;
        }
        if(isGoodPair(adjacencyList, numberOfVertices, 
         union(excludedVertices, singleton(array[j])), a,c, pathFlag)) {
            if(pathFlag) fprintf(stderr,"\t1.5 (a,c), outer vertex: %d\n",
             array[j]);
            return true;
        }
        if(isGoodPair(adjacencyList, numberOfVertices, 
         union(excludedVertices, singleton(array[j])), b,d, pathFlag)) {
            if(pathFlag) fprintf(stderr,"\t1.5 (b,d), outer vertex: %d\n",
             array[j]);
            return true;
        }
        if(isGoodPair(adjacencyList, numberOfVertices, 
         union(excludedVertices, singleton(array[j])), c,d, pathFlag)) {
            if(pathFlag) fprintf(stderr,"\t1.5 (c,d), outer vertex: %d\n",
             array[j]);
            return true;
        }
    }


    return false;
}

// Determines whether or not the given suitable cell is also a K1-cell.
// outerVertices should be in the correct order!
bool isK1Cell(bitset adjacencyList[], int numberOfVertices, int outerVertices
[], bool verboseFlag, bool pathFlag) {

    for(int i = 0; i < numberOfVertices; i++) {
        if (i == outerVertices[0] || i == outerVertices[1]  ||
         i == outerVertices[2] || i == outerVertices[3]) {
            continue;
        }
        if(pathFlag) fprintf(stderr,"G - %d:\n",i);
        if(badPairBecomesGoodInSubgraph(adjacencyList,numberOfVertices,
         outerVertices[0],outerVertices[1],outerVertices[2],outerVertices[3],
         singleton(i), verboseFlag, pathFlag)) {
            continue;
        }
        if(verboseFlag) {
        	fprintf(stderr,"  No bad pairs became good in G - %d.\n",i);
        }
        return false;
        

    }
    return true;
}

// Determines whether or not the given suitable cell is also a K2-cell.
// outerVertices should be in the correct order!
bool isK2Cell(bitset adjacencyList[], int numberOfVertices, int outerVertices
[], bool verboseFlag, bool pathFlag) {

    //2.1
    for(int i = 0; i < numberOfVertices; i++) {
		if (i == outerVertices[0] || i == outerVertices[1]  || 
		 i == outerVertices[2] || i == outerVertices[3]) {
			continue;
        }
        forEachAfterIndex(neighbour, adjacencyList[i], i) {
            if (neighbour == outerVertices[0] || neighbour == outerVertices[1]
             || neighbour == outerVertices[2] || neighbour == outerVertices[3]) {
                continue;
            }
            if(pathFlag) fprintf(stderr, "%d %d\n", i, neighbour);
            if(badPairBecomesGoodInSubgraph(adjacencyList,numberOfVertices,
             outerVertices[0],outerVertices[1],outerVertices[2],outerVertices[3],
             union(singleton(i),singleton(neighbour)), verboseFlag, pathFlag)) {
                continue;
            }
            if(verboseFlag) {
            	fprintf(stderr, "  Condition 2.1 failed in G - %d - %d.\n",
            	 i,neighbour);
            }
            return false;
        }
    }

    //2.2
    forEach(neighbour, adjacencyList[outerVertices[0]]){
        if(neighbour == outerVertices[1] || neighbour == outerVertices[2]) {
            continue;
        }
        if(pathFlag) fprintf(stderr, "2.2: Inner neighbour: %d\n",
         neighbour);
        if(!isGoodPair(adjacencyList,numberOfVertices,
         union(singleton(outerVertices[0]),singleton(neighbour)),
         outerVertices[1],outerVertices[2],pathFlag)) {
         	if(verboseFlag) {
            	fprintf(stderr, "  Condition 2.2 failed.\n");
            }
            return false;
        }
    }

    //2.3
    forEach(neighbour, adjacencyList[outerVertices[3]]){
        if(neighbour == outerVertices[1] || neighbour == outerVertices[2]) {
            continue;
        }
        if(pathFlag) fprintf(stderr, "2.3: Inner neighbour: %d\n",
         neighbour);
        if(!isGoodPair(adjacencyList,numberOfVertices,
         union(singleton(outerVertices[3]),singleton(neighbour)), 
         outerVertices[1],outerVertices[2],pathFlag)) {
         	if(verboseFlag) {
            	fprintf(stderr, "  Condition 2.2 failed.\n");
            }
            return false;
        }
    }

    //2.4
    forEach(neighbour, adjacencyList[outerVertices[1]]){
        if(neighbour == outerVertices[0] || neighbour == outerVertices[3]) {
            continue;
        }
        if(pathFlag) fprintf(stderr, "2.4: Inner neighbour: %d\n",
         neighbour);
        if(!isGoodPair(adjacencyList,numberOfVertices,
         union(singleton(outerVertices[1]),singleton(neighbour)),
         outerVertices[0],outerVertices[3],pathFlag)) {
         	if(verboseFlag) {
            	fprintf(stderr, "  Condition 2.2 failed.\n");
            }
            return false;
        }
    }

    //2.5
    forEach(neighbour, adjacencyList[outerVertices[2]]){
        if(neighbour == outerVertices[0] || neighbour == outerVertices[3]) {
            continue;
        }
        if(pathFlag) fprintf(stderr, "2.5: Inner neighbour: %d \n",
         neighbour);
        if(!isGoodPair(adjacencyList,numberOfVertices,
         union(singleton(outerVertices[2]),singleton(neighbour)),
         outerVertices[0],outerVertices[3], pathFlag)) {
         	if(verboseFlag) {
            	fprintf(stderr, "  Condition 2.2 failed.\n");
            }
            return false;
        }
    }
    return true;
}