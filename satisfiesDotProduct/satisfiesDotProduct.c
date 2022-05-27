/**
 * satisfiesDotProduct.c
 * 
 * Author: Jarne Renders (jarne.renders@kuleuven.be)
 *
 */

#define USAGE \
"Usage: `./satisfiesDotProduct [-1|-2] [-apv] [-e] [-e#,#] [-P#,#] [-P#,#,#,#] [-h]`"


#define HELPTEXT \
"All options can be found by executing `./satisfiesDotProduct -h`.\n\
\n\
Filter graphs satisfying certain conditions involving the dot product and\n\
K2-hamiltonian graphs. Refer to `satifiesDotProduct/README.md` for more details\n\
on these conditions.\n\
\n\
Graphs are read from stdin in graph6 format. Graphs are sent to stdout in graph6\n\
format. If the input graph had a graph6 header, so will the output graph (if it\n\
passes through the filter).\n\
\n\
The order in which the arguments appear does not matter, unless multiple\n\
instances of flags with an optional or required argument are given(the lastmost\n\
instance will be chosen). Concatenating an option after one with an argument\n\
will ignore the latter, e.g.: `-e5,6v` will ignore the `-v` flag.\n\
\n\
    -1, --first\n\
            let the graphs satisfying the first three conditions (i)-(iii) for\n\
            some pair of independent edges pass through the filter; should not\n\
            be used with -2; default\n\
    -2, --last\n\
            let the graphs satsifying the last three conditions (iv)-(vi) for\n\
            some pair of independent edges pass through the filter; should not\n\
            be used with -1\n\
    -a, --all\n\
            count how many pairs of independent edges (-1) or adjacent cubic\n\
            vertices (-2) in this graph satisfy the conditions to be checked;\n\
            if -v is present show all these pairs\n\
    -e, --extra\n\
            only use with -1; graphs will pass through the filter if the first\n\
            three conditions are satisfied and some pair of adjacent cubic\n\
            vertices satisfies the extra conditions; if used as -e#1,#2 require\n\
            that(#1,#2) is a pair of adjacent cubic vertices satisfying the\n\
            extra conditions\n\
    -h, --help\n\
            print help message\n\
    -p, --print\n\
            send a pair of independent edges (-1) or adjacent cubic vertices\n\
            (-2) satisfying the required conditions to stdout; if -a is present\n\
            send all such pairs\n\
    -P, --paths\n\
            requires -P#1,#2,#3,#4 (-1) or -P#1,#2 (-2); only checks whether the\n\
            required conditions are satisfied by the pair of independent edges\n\
            (#1,#2) and (#3,#4) or the pair of adjacent cubic vertices\n\
            (#1,#2); shows all paths necessary in proving it is so\n\
    -v, --verbose\n\
            verbose mode; if a graph passes the filter, show a pair satisfying\n\
            the required conditions; if used with -a show all pairs\n"

#include <stdio.h>
#include <stdbool.h>
#include <getopt.h>
#include <stdlib.h>
#include <time.h>
#include "../bitset.h"
#include "../hamiltonicityMethods.h"
#include "../readGraph/readGraph6.h"

int firstThreeConditions(bitset adjacencyList[], int nVertices, int
extraConditionsVertices[], bool extraConditionsFlag, bool verboseFlag, bool
printFlag, bool allFlag);

int satisfiesFirstThreeConditions(bitset adjacencyList[], int nVertices,int a,
int b, int c, int d, int extraConditionsVertices[], bool extraConditionsFlag,
bool verboseFlag, bool pathFlag);

int lastThreeConditions(bitset adjacencyList[], int nVertices, bool verboseFlag, bool
printFlag, bool allFlag);

bool satisfiesLastThreeConditions(bitset adjacencyList[], int nVertices, int x,
int y, bool verboseFlag, bool pathFlag);

int main(int argc, char ** argv) {
	bool allFlag = false;
	bool verboseFlag = false;
	bool printFlag = false;
	bool firstThreeConditionsFlag = true;
	bool pathFlag = false;
	bool extraConditionsFlag = false;
	int opt;
	int independentEdges[4] = {-1,-1,-1,-1};
	int extraConditionsVertices[2] = {-1,-1};
	int adjacentCubicVertices[2] = {-1,-1};
	while (1) {
    	int option_index = 0;
    	static struct option long_options[] = 
    	{
    		{"first", 						no_argument, NULL, '1'},
        	{"last", 						no_argument, NULL, '2'},
        	{"all", 						no_argument, NULL, 'a'},
        	{"extra",  						optional_argument, NULL, 'e'},
            {"help",                    	no_argument, NULL,  'h'},
        	{"print", 						no_argument, NULL, 'p'},
        	{"paths", 						required_argument, NULL, 'P'},
        	{"verbose", 					no_argument, NULL,  'v'},
    	};

    	opt = getopt_long(argc, argv, "12ae::hpP:v", long_options, &option_index);
    	if (opt == -1) break;
		switch(opt) {
			case '2':
				firstThreeConditionsFlag = false;
				break;
			case 'a':
				allFlag = true;
				break;
			case 'e':
				extraConditionsFlag = true;
				sscanf(optarg ? optarg : "(none)", "%d,%d",
				 &extraConditionsVertices[0], &extraConditionsVertices[1]);
				break;
			case 'h':
                fprintf(stderr, "%s\n", USAGE);
                fprintf(stderr, "%s", HELPTEXT);
                return 0;
                break;
			case 'p':
				printFlag = true;
				break;
			case 'P':
				pathFlag = true;
				verboseFlag = true;
				sscanf(optarg ? optarg : "(none)", "%d,%d", &adjacentCubicVertices[0],
				 &adjacentCubicVertices[1]);
				if (sscanf(optarg ? optarg : "(none)", "%d,%d,%d,%d",
				 &independentEdges[0], &independentEdges[1], &independentEdges[2],
				 &independentEdges[3]) == 4) {
					adjacentCubicVertices[0] = -1;
					adjacentCubicVertices[1] = -1;
				}
				if(adjacentCubicVertices[0] == -1 && independentEdges[0] == -1) {
					fprintf(stderr, "Invalid argument for option -- P: %s.\n",
                     optarg);
                    fprintf(stderr, "%s\n", USAGE);
                    fprintf(stderr,
                     "Use ./satisfiesDotProduct --help for more detailed instructions.\n");
					return 1;
				}
				break;
			case 'v':
				verboseFlag = true;
				break;
			case '?':
                fprintf(stderr, "%s\n", USAGE);
                fprintf(stderr,
                 "Use ./satisfiesDotProduct --help for more detailed instructions.\n");
				return 1;
		}
	}

    if(!firstThreeConditionsFlag && extraConditionsFlag) {
        fprintf(stderr, "Error: Do not use -e with -2.\n");
        fprintf(stderr, "%s\n", USAGE);
        fprintf(stderr,
         "Use ./satisfiesDotProduct --help for more detailed instructions.\n");
        return 1;
    }

    if(MAXVERTICES != 64) {
        fprintf(stderr,
         "Warning: For graphs up to 64 vertices, the 64 bit version of this program is faster.\n");
    }

	clock_t start = clock();
	unsigned long long int counter = 0;
	unsigned long long int firstThreeConditionsCounter = 0;
	unsigned long long int lastThreeConditionsCounter = 0;
    unsigned long long int skippedGraphs = 0;

	char * graphString = NULL;
	size_t size;
	while(getline(&graphString, &size, stdin) != -1) {
		if(verboseFlag || allFlag) {
			fprintf(stderr, "\nLooking at %s",graphString);
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
		if(firstThreeConditionsFlag) {
			if(independentEdges[3] != -1) {
				fprintf(stderr, "\nPaths and cycles for (%d,%d) and (%d,%d):\n",
			 	 independentEdges[0], independentEdges[1],
				 independentEdges[2], independentEdges[3]);
				bool satisfiesFirstThreeCond =
				 satisfiesFirstThreeConditions(adjacencyList,nVertices,
				 independentEdges[0],independentEdges[1],independentEdges[2],
				 independentEdges[3],extraConditionsVertices,extraConditionsFlag,
				 verboseFlag,pathFlag);
				if(satisfiesFirstThreeCond) {
					firstThreeConditionsCounter++;
					if(printFlag) {
						printf("%d %d %d %d\n", independentEdges[0],
						 independentEdges[1],independentEdges[2],
						 independentEdges[3]);
					}
					printf("%s", graphString);
				}
				fprintf(stderr,
				 "a: %d b: %d c: %d d: %d%s satisfy conditions (i) - (iii) and the extra condition.\n",
				  independentEdges[0],independentEdges[1],independentEdges[2],
				  independentEdges[3], satisfiesFirstThreeCond ? "" : " do not");
				continue;
			}
			int numberOfPairs;
			if((numberOfPairs = firstThreeConditions(adjacencyList, nVertices,
			 extraConditionsVertices, extraConditionsFlag, verboseFlag, 
			 printFlag, allFlag))) {
				firstThreeConditionsCounter++;
				if(allFlag) {
					fprintf(stderr,
					 "  The graph contains %d pairs satisfying the first three%s conditions.\n",
					 numberOfPairs, extraConditionsFlag ? " and the extra" : "");
				}
				printf("%s", graphString);
			}
		}
		else {
			if(adjacentCubicVertices[1] != -1) {
				bool satisfiesLastThreeCond =
				 satisfiesLastThreeConditions(adjacencyList,nVertices,
				 adjacentCubicVertices[0],adjacentCubicVertices[1], verboseFlag,
				 pathFlag);
				if(satisfiesLastThreeCond) {
					lastThreeConditionsCounter++;
					if(printFlag) {
						printf("%d %d\n", adjacentCubicVertices[0],
						 adjacentCubicVertices[1]);
					}
					printf("%s", graphString);
				}
				fprintf(stderr, "x: %d y: %d%s satisfy conditions (iv) - (vi).\n",
				 adjacentCubicVertices[0], adjacentCubicVertices[1],
				 satisfiesLastThreeCond ? "" : " do not");
				continue;
			}
			int numberOfPairs;
			if((numberOfPairs = lastThreeConditions(adjacencyList, nVertices,
			 verboseFlag, printFlag, allFlag))) {
				lastThreeConditionsCounter++;
				if(allFlag) {
					fprintf(stderr,
					 "The graph contains %d pairs satisfying the last three conditions.\n", 
					 numberOfPairs);
				}
				printf("%s", graphString);
			}
		}
	}
	clock_t end = clock();
	double time_spent = (double)(end - start) / CLOCKS_PER_SEC;

	fprintf(stderr,"Checked %lld graphs in %f seconds: ", counter, time_spent);
	if(firstThreeConditionsFlag) {
		fprintf(stderr, "%lld contain independent edges satisfying the first three%s conditions.\n",
		 firstThreeConditionsCounter, extraConditionsFlag ? " and the extra" : "");
	}
	else {
		fprintf(stderr, "%lld satisfy the constraints for H.\n",
		 lastThreeConditionsCounter);
	}
    if(skippedGraphs) {
        fprintf(stderr, "Warning: %lld graphs could not be read.\n", skippedGraphs);
    }
	return 0;
}

bool satisfiesCondition1(bitset adjacencyList[], int nVertices, int a, int b,
int c, int d, bool verboseFlag);

bool satisfiesCondition2(bitset adjacencyList[], int nVertices, int a, int b,
int c, int d, bool verboseFlag);

bool satisfiesCondition3(bitset adjacencyList[], int nVertices, int a, int b,
int c, int d, bool verboseFlag);

bool extraCondition(bitset adjacencyList[], int nVertices, int a, int b, int c,
int d, int extraConditionsVertices[], bool verboseFlag, bool pathFlag);

int satisfiesFirstThreeConditions(bitset adjacencyList[], int nVertices,int a,
int b, int c, int d, int extraConditionsVertices[], bool extraConditionsFlag,
bool verboseFlag, bool pathFlag) {

	if(!satisfiesCondition1(adjacencyList,nVertices,a,b,c,d, pathFlag)) {
		return false;
	}
	if(!satisfiesCondition2(adjacencyList,nVertices,a,b,c,d, pathFlag)) {
		return false;
	}
	if(!satisfiesCondition3(adjacencyList,nVertices,a,b,c,d, pathFlag)) {
		return false;
	}
	if(extraConditionsFlag) {
		if(!extraCondition(adjacencyList,nVertices,a,b,c,d,
		 extraConditionsVertices,verboseFlag,pathFlag)) {
			return false;
		}
	}
	return true;
}

int firstThreeConditions(bitset adjacencyList[], int nVertices, int
extraConditionsVertices[], bool extraConditionsFlag, bool verboseFlag, bool
printFlag, bool allFlag) {

	int countPairs = 0;
	bool firstSuitableEdges = true;
	bool extraConditionsVerticesWereGiven = (extraConditionsVertices[0] != -1); 

	// Loop over all independent edges of the graph.
	for(int a = 0; a < nVertices; a++) {
		for(int c = a+1; c < nVertices; c++) {
			if(contains(adjacencyList[a], c)) {
				continue;
			}
			forEachAfterIndex(b, adjacencyList[a], a) {
				if(contains(adjacencyList[c], b)) {
					continue;
				}
				forEachAfterIndex(d, adjacencyList[c], c) {
					if(contains(adjacencyList[a], d) || 
					 contains(adjacencyList[b], d)) {
						continue;
					}

					// At this point ab and cd are independent edges.
					if(!satisfiesFirstThreeConditions(adjacencyList,nVertices,
					 a,b,c,d, extraConditionsVertices, extraConditionsFlag, 
					 verboseFlag, false)) {
						continue;
					}
					countPairs++;
					if(verboseFlag) {
						if(firstSuitableEdges) {
							firstSuitableEdges = false;
							fprintf(stderr,
						 	"  Conditions (i)-(iii)%s are satisfied by:\n",
						 	 extraConditionsFlag ? " and the extra conditions"\
						 	  : "");
						}
						fprintf(stderr, "    (%d,%d) and (%d,%d)", a,b,c,d);
						if(extraConditionsFlag) {
							fprintf(stderr,
							 ", x_G = %d, y_G = %d.", 
							 extraConditionsVertices[0],
							 extraConditionsVertices[1]);
						}
						fprintf(stderr, "\n");
					}
					
					if(!extraConditionsVerticesWereGiven) {
				 		extraConditionsVertices[0] = -1;
				 		extraConditionsVertices[1] = -1;
				 	}

					if(printFlag) {
						printf("%d %d %d %d\n",a,b,c,d);
					}
					//	Do not return if we want all pairs.
					if(!allFlag) {
						return true;
					}
				}
			}
		}
	}
	return countPairs;
}

bool existsHamiltonianPathNotContainingEdge(bitset adjacencyList[], int
nVertices, bitset excludedVertices, int beginning, int end, int edgeBeginning,
int edgeEnd, bool verboseFlag) {
	
	removeElement(adjacencyList[edgeEnd], edgeBeginning);
	removeElement(adjacencyList[edgeBeginning], edgeEnd);
	
	bool containsHamPath = containsHamiltonianPathWithEnds(adjacencyList,
	 nVertices,excludedVertices,beginning,end,false,verboseFlag);

	add(adjacencyList[edgeEnd], edgeBeginning);
	add(adjacencyList[edgeBeginning], edgeEnd);
	return containsHamPath;
}

bool existSpanningPathsNotContainingTwoEdges(bitset adjacencyList[], int
nVertices, bitset excludedNodes, int a, int b, int c, int d, bool verboseFlag)
{

	removeElement(adjacencyList[a],b);
	removeElement(adjacencyList[b],a);
	removeElement(adjacencyList[c],d);
	removeElement(adjacencyList[d],c);
	bool exists = false;

	if (containsDisjointSpanningPathsWithEnds(adjacencyList,nVertices, 
	 excludedNodes,a,c,EMPTY,b,d,EMPTY,false,verboseFlag))
		exists = true;
	else if (containsDisjointSpanningPathsWithEnds(adjacencyList,nVertices, 
	 excludedNodes,a,d,EMPTY,b,c,EMPTY,false,verboseFlag))
		exists = true;

	add(adjacencyList[a],b);
	add(adjacencyList[b],a);
	add(adjacencyList[c],d);
	add(adjacencyList[d],c);

	return exists;
}

bool satisfiesCondition1(bitset adjacencyList[], int nVertices, int a, int b,
int c, int d, bool verboseFlag) {

	if(verboseFlag) {
		fprintf(stderr, "\n(i):\n");
	}

	// Loop over all pairs vw with v < w.
	for(int i = 0; i < nVertices; i++) {
		forEachAfterIndex(nbr, adjacencyList[i], i) { 
			if((i == a && nbr == b) || (i == b && nbr == a) || (i == c && nbr == d) || (i==d && nbr == c)) {
				continue;
			}

			if(verboseFlag) {
				fprintf(stderr, "G - %d - %d:\n", i, nbr);
			}

			// Consider G - v - w
			bitset excludedVertices = union(singleton(i), singleton(nbr));
			if (i != a && i != b && nbr != a && nbr != b) {
				if(existsHamiltonianPathNotContainingEdge(adjacencyList,
				 nVertices, excludedVertices, a, b, c, d, verboseFlag)) {	
					continue;
				}
			}
			if (i != c && i != d && nbr != c && nbr != d) {
				if(existsHamiltonianPathNotContainingEdge(adjacencyList,
				 nVertices, excludedVertices, c, d, a, b, verboseFlag)) {
					continue;
				}
			}
			if(i!=a && i!=b && i!=c && i!=d && nbr!=a && nbr!=b &&
			 nbr!=c && nbr!=d) {
				if(existSpanningPathsNotContainingTwoEdges(adjacencyList,
				 nVertices,union(singleton(i),singleton(nbr)), a,b,c,d,
				  verboseFlag)) {
					continue;
				}
			}
			return false;
		}
	}
	return true;
}

bool containsAllHamPaths(bitset adjacencyList[], int nVertices, int a, int b,
int c, int d, bool verboseFlag) {

	if(verboseFlag) {
		fprintf(stderr, "ac-");
	}
	if(!containsHamiltonianPathWithEnds(adjacencyList,nVertices,EMPTY, a, c,
	 false, verboseFlag)) {
		return false;
	}
	if(verboseFlag) {
		fprintf(stderr, "ad-");
	}
	if(!containsHamiltonianPathWithEnds(adjacencyList,nVertices,EMPTY, a, d,
	 false, verboseFlag)) {
		return false;
	}
	if(verboseFlag) {
		fprintf(stderr, "bc-");
	}
	if(!containsHamiltonianPathWithEnds(adjacencyList,nVertices,EMPTY, b, c,
	 false, verboseFlag)) {
		return false;
	}
	if(verboseFlag) {
		fprintf(stderr, "bd-");
	}
	if(!containsHamiltonianPathWithEnds(adjacencyList,nVertices,EMPTY, b, d,
	 false, verboseFlag)) {
		return false;
	}
	if(verboseFlag) {
		fprintf(stderr, "ab- and cd-paths:\n");
	}
	if(!containsDisjointSpanningPathsWithEnds(adjacencyList, nVertices, EMPTY,
	 a,b, EMPTY ,c,d, EMPTY, false, verboseFlag)) {
		return false;
	}
	return true;
}

bool satisfiesCondition2(bitset adjacencyList[], int nVertices, int a, int b,
int c, int d, bool verboseFlag) {

	if(verboseFlag) {
		fprintf(stderr, "\n(ii):\n");
	}

	// Work with boolean since we need to add edges back to graph at the end.
	bool admitsForAny = true; 
	removeElement(adjacencyList[a],b);
	removeElement(adjacencyList[b],a);
	removeElement(adjacencyList[c],d);
	removeElement(adjacencyList[d],c);

	if(!containsAllHamPaths(adjacencyList, nVertices, a,b,c,d, verboseFlag))
		admitsForAny = false;

	add(adjacencyList[a],b);
	add(adjacencyList[b],a);
	add(adjacencyList[c],d);
	add(adjacencyList[d],c);
	return admitsForAny;
}

bool containsHamCycleWithEdge(bitset adjacencyList[], int nVertices, bitset
excludedVertices, int edgeStart, int edgeEnd, bool verboseFlag) {

	bitset remainingVertices = complement(excludedVertices, nVertices);
	removeElement(remainingVertices, edgeStart);
	removeElement(remainingVertices, edgeEnd);
	if(!verboseFlag) {
		if(canBeHamiltonian(adjacencyList,remainingVertices,edgeStart,edgeEnd,
		 nVertices - size(excludedVertices), 2)) {
			return true;
		}
		return false;
	}
	int pathList[nVertices - size(excludedVertices)];
	pathList[0] = edgeStart;
	pathList[1] = edgeEnd;
	int n = 0;
	if(canBeHamiltonianPrintCycle(adjacencyList,remainingVertices, pathList,
	 edgeStart,edgeEnd,nVertices - size(excludedVertices),2,&n,false,
	 verboseFlag)) {
		return true;
	}
	return false;
}

bool satisfiesCondition3(bitset adjacencyList[], int nVertices, int a, int b,
int c, int d, bool verboseFlag) {

	if(verboseFlag) {
		fprintf(stderr, "\n(iii):\n");
	}

	if(verboseFlag) {
		fprintf(stderr, "G - a:\n cd-");
	}
	if(!containsHamCycleWithEdge(adjacencyList,nVertices,singleton(a), c, d,
	 verboseFlag)) {
		return false;
	}

	if(verboseFlag) {
		fprintf(stderr, "G - b:\n cd-");
	}
	if(!containsHamCycleWithEdge(adjacencyList,nVertices,singleton(b), c, d,
	 verboseFlag)) {
		return false;
	}

	if(verboseFlag) {
		fprintf(stderr, "G - c:\n ab-");
	}
	if(!containsHamCycleWithEdge(adjacencyList,nVertices,singleton(c), a, b,
	 verboseFlag)) {
		return false;
	}

	if(verboseFlag) {
		fprintf(stderr, "G - d:\n ab-");
	}
	if(!containsHamCycleWithEdge(adjacencyList,nVertices,singleton(d), a, b,
	 verboseFlag)) {
		return false;
	}
	return true;
}

bool pairSatisfiesExtraCondition(bitset adjacencyList[], int nVertices, int a,
int b, int c, int d, int xG, int yG, bool verboseFlag) {

	// Check if closed neighbourhood does not intersect a,b,c,d
	bitset closedNbrhdOfExtraConditionsVertices = 
	 union(adjacencyList[xG],
	 adjacencyList[yG]);
	add(closedNbrhdOfExtraConditionsVertices, xG);
	add(closedNbrhdOfExtraConditionsVertices, yG);
	if(contains(closedNbrhdOfExtraConditionsVertices, a) || 
	 contains(closedNbrhdOfExtraConditionsVertices, b) || 
	 contains(closedNbrhdOfExtraConditionsVertices, c) || 
	 contains(closedNbrhdOfExtraConditionsVertices, d)) {
		return false;
	}

	if(verboseFlag) {
		fprintf(stderr, "G - %d:\n", xG);
	}
	if(!existsHamiltonianPathNotContainingEdge(adjacencyList,nVertices, 
	 singleton(xG), a,b,c,d, verboseFlag) &&
	 !existsHamiltonianPathNotContainingEdge(adjacencyList,nVertices, 
	 singleton(xG),c,d,a,b,verboseFlag)) {
		return false;
	}
	if(verboseFlag) {
		fprintf(stderr, "G - %d:\n", yG);
	}
	if(!existsHamiltonianPathNotContainingEdge(adjacencyList,nVertices, 
	 singleton(yG), a,b,c,d, verboseFlag) &&
	 !existsHamiltonianPathNotContainingEdge(adjacencyList,nVertices,
	 singleton(yG),c,d,a,b,verboseFlag)) {
		return false;
	}
	if(verboseFlag) {
		fprintf(stderr, "Disjoint spanning cycles:\n");
	}
	removeElement(adjacencyList[a],b);
	removeElement(adjacencyList[b],a);
	removeElement(adjacencyList[c],d);
	removeElement(adjacencyList[d],c);
	bool containsDisjPaths =
	 containsDisjointSpanningPathsWithEnds(adjacencyList, nVertices, EMPTY, a, b,
	 singleton(xG), c, d, singleton(yG), false, verboseFlag) ||
	 containsDisjointSpanningPathsWithEnds(adjacencyList, nVertices, EMPTY, a, b,
	 singleton(yG), c, d, singleton(xG), false, verboseFlag);
	add(adjacencyList[a],b);
	add(adjacencyList[b],a);
	add(adjacencyList[c],d);
	add(adjacencyList[d],c);		
	return containsDisjPaths;
}

bool extraCondition(bitset adjacencyList[], int nVertices, int a, int b, int c,
int d, int extraConditionsVertices[], bool verboseFlag, bool pathFlag) {

	if(pathFlag) {
		fprintf(stderr, "\nConditions of Lemma 6:\n");
	}

	if(extraConditionsVertices[0] != -1 && extraConditionsVertices[1] != -1) {

		if(pairSatisfiesExtraCondition(adjacencyList,nVertices,a,b,c,d,
		 extraConditionsVertices[0], extraConditionsVertices[1], pathFlag)) {
			if(pathFlag) {
				fprintf(stderr, "Extra conditions satisfied by (%d,%d)\n",
				 extraConditionsVertices[0], extraConditionsVertices[1]);
			}
			return true;
		}
		if(pathFlag) {
			fprintf(stderr, "Extra conditions not satisfied by (%d,%d)\n",
			 extraConditionsVertices[0], extraConditionsVertices[1]);
		}
		return false;
	}

	int pairsSatisfyingCondition = 0;

	// Loop over all pairs of adjacent cubic vertices.
	for(int i = 0; i < nVertices; i++) {
		if(size(adjacencyList[i]) != 3) {
			continue;
		}
		forEachAfterIndex(neighbour, adjacencyList[i], i) {
			if(size(adjacencyList[neighbour]) != 3) {
				continue;
			}

			if(!pairSatisfiesExtraCondition(adjacencyList,nVertices,a,b,c,d,i,
			 neighbour,pathFlag)) {
				continue;
			}

			if(verboseFlag) {
				pairsSatisfyingCondition++;
				extraConditionsVertices[0] = i;
				extraConditionsVertices[1] = neighbour;
			}

			if(pathFlag) {
				fprintf(stderr, "Extra conditions satisfied by (%d,%d)\n", i,
				 neighbour);
			}
			return true;
		}
	}
	return pairsSatisfyingCondition;
}

bool satisfiesCondition4(bitset adjacencyList[], int nVertices, int x, int y,
bool verboseFlag);

bool satisfiesCondition5(bitset adjacencyList[], int nVertices, int x, int y,
bool verboseFlag);

bool satisfiesCondition6(bitset adjacencyList[], int nVertices, int x, int y,
bool verboseFlag);

bool satisfiesLastThreeConditions(bitset adjacencyList[], int nVertices, int x,
int y, bool verboseFlag, bool pathFlag) {

	if(!satisfiesCondition4(adjacencyList,nVertices,x,y, pathFlag)) {
		return false;
	}

	if(!satisfiesCondition5(adjacencyList,nVertices,x,y, pathFlag)) {
		return false;
	}

	if(!satisfiesCondition6(adjacencyList,nVertices,x,y, pathFlag)) {
		return false;
	}
	return true;
}

int lastThreeConditions(bitset adjacencyList[], int nVertices, bool verboseFlag,
bool printFlag, bool allFlag) {

	int countPairs = 0;
	bool firstSuitablePair = true;

	// Loop over all pairs of adjacent cubic vertices.
	for (int x = 0; x < nVertices; x++) {
		if(size(adjacencyList[x])>3) continue;
		forEachAfterIndex(y, adjacencyList[x], x) {
			if(size(adjacencyList[y])>3) continue;

			if(!satisfiesLastThreeConditions(adjacencyList,nVertices,x,y, 
			 verboseFlag, false)) {
				continue;
			}
			countPairs++;

			if(verboseFlag) {
				if(firstSuitablePair) {
					firstSuitablePair = false;
					fprintf(stderr,
					"  Conditions (iv)-(vi) are satisfied by adjacent cubic vertices:\n");
				}
				fprintf(stderr, "    %d, %d\n",x,y);
			}
			if(printFlag) {
				printf("%d %d\n",x,y);
			}
			if(!allFlag) {
				return true;
			}
		}
	}
	return countPairs;
}

bool satisfiesCondition4(bitset adjacencyList[], int nVertices, int x, int y,
bool verboseFlag) {

	if(verboseFlag) {
		fprintf(stderr, "\n(iv):\n");
	}

	// H - x is hamiltonian
	if(verboseFlag) {
		fprintf(stderr, "H - x: hamiltonian cycle:\n");
	}
	bitset excludedVertices = singleton(x);
	if (!(isHamiltonian(adjacencyList, nVertices, excludedVertices, false,
	 verboseFlag))) {
		return false;
	}

	// H - y is hamiltonian
	if(verboseFlag) {
		fprintf(stderr, "H - y: hamiltonian cycle:\n");
	}
	excludedVertices = singleton(y);
	if (!(isHamiltonian(adjacencyList, nVertices, excludedVertices, false,
	 verboseFlag))) {
		return false;
	}

	int a = next(difference(adjacencyList[x], singleton(y)), -1);
	int b = next(difference(adjacencyList[x], singleton(y)), a);
	int c = next(difference(adjacencyList[y], singleton(x)), -1);
	int d = next(difference(adjacencyList[y], singleton(x)), c);

	// Contains disjoint spanning ab and cd-paths
	if(verboseFlag) {
		fprintf(stderr, "H - x - y: disjoint spanning ab- and cd-paths:\n");
	}
	if(!(containsDisjointSpanningPathsWithEnds(adjacencyList, nVertices,
	 union(singleton(x),singleton(y)), a,b, EMPTY, c,d, EMPTY, false,
	 verboseFlag))) {
		return false;
	}

	return true;
}

bool containsHamSTPath(bitset adjacencyList[], int nVertices,int x,int y,int
v,int w, bool verboseFlag) {

	forEach(s, difference(adjacencyList[x], singleton(y))) {
		if( s == v || s == w) continue;
		forEach(t, difference(adjacencyList[y], singleton(x))) {
			if( t == v || t == w) continue;
			bitset excludedVertices = union(singleton(x), singleton(y));
			add(excludedVertices, v);
			add(excludedVertices, w);
			if(containsHamiltonianPathWithEnds(adjacencyList,nVertices,
			 excludedVertices,s,t,false,verboseFlag))
				return true;
		}
	}
	return false;
}

bool containsDisjointSTPaths(bitset adjacencyList[], int nVertices, int x, int
y, int v, int w, bool verboseFlag) {

	int a = next(difference(adjacencyList[x], singleton(y)), -1);
	int b = next(difference(adjacencyList[x], singleton(y)), a);
	int c = next(difference(adjacencyList[y], singleton(x)), -1);
	int d = next(difference(adjacencyList[y], singleton(x)), c);

	bitset excludedVertices = union(singleton(x), singleton(y));
	add(excludedVertices,v);
	add(excludedVertices,w);
	if(containsDisjointSpanningPathsWithEnds(adjacencyList,nVertices,
	 excludedVertices,a,c, EMPTY, b,d, EMPTY, false, verboseFlag))
		return true;
	if(containsDisjointSpanningPathsWithEnds(adjacencyList,nVertices,
	 excludedVertices,a,d, EMPTY, b,c, EMPTY, false, verboseFlag))
		return true;
	return false;
}

bool satisfiesCondition5(bitset adjacencyList[], int nVertices, int x, int y,
bool verboseFlag) {

	if(verboseFlag) {
		fprintf(stderr, "\n(v):\n");
	}
	for(int v = 0; v < nVertices; v ++) {
		if( v == x || v == y ) continue;
		forEachAfterIndex(w,adjacencyList[v], v) {
			if( w == x || w == y ) continue;
			if(verboseFlag) {
				fprintf(stderr, "H - x - y - %d - %d:\n",v,w);
			}
			if(!containsHamSTPath(adjacencyList,nVertices,x,y,v,w,verboseFlag) &&
			 !containsDisjointSTPaths(adjacencyList,nVertices,x,y,v,w,
			  verboseFlag)) {
				return false;
			}
		}
	}
	return true;
}

bool satisfiesCondition6(bitset adjacencyList[], int nVertices, int x, int y,
bool verboseFlag) {

	if(verboseFlag) {
		fprintf(stderr, "\n(vi)\n");
	}
	forEach(s, difference(adjacencyList[x], singleton(y))) {
		bitset excludedVertices = union(singleton(x), singleton(s));
		if(!isHamiltonian(adjacencyList, nVertices, excludedVertices, false,
		 verboseFlag)) {
			return false;
		}
	}

	forEach(t, difference(adjacencyList[y], singleton(x))) {
		bitset excludedVertices = union(singleton(y), singleton(t));
		if(!isHamiltonian(adjacencyList, nVertices, excludedVertices, false,
		 verboseFlag)) {
			return false;
		}
	}
	return true;
}

