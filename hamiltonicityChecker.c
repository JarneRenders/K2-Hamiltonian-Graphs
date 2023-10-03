/**
 * hamiltonicityChecker.c
 * 
 * Author: Jarne Renders (jarne.renders@kuleuven.be)
 *
 */

#define USAGE \
"\nUsage: `./hamiltonicityChecker [-t] [-1|-2] [-n] [-c] [-v] [-v#] [-v#,#] [-a] [-h] [res/mod]`\n"

#define HELPTEXT \
"Filter graphs satisfying certain hamiltonicity requirements.\n\
\n\
Graphs are read from stdin in graph6 format. Graphs are sent to stdout in\n\
graph6 format. If the input graph had a graph6 header, so will the\n\
output graph (if it passes through the filter).\n\
\n\
The order in which the arguments appear does not matter.\n\
\n\
    -1, --K1-hamiltonian\n\
            let the K1-hamiltonian graphs pass through the filter; if -n\n\
            and -c are not present this will send all hypohamiltonian\n\
            graphs to stdout; cannot be used with -2\n\
    -2, --K2-hamiltonian\n\
            let the K2-hamiltonian graphs pass through the filter; if -n\n\
            and -c are not present this will send all K2-hypohamiltonian\n\
            graphs to stdout; cannot be used with -1\n\
    -a, --all-cycles\n\
            counts all hamiltonian cycles of the graph; if -v is present these\n\
            cycles get printed; if -v together with an optional argument\n\
            is present, this is also done for the corresponding subgraph\n\
    -c, --complement\n\
            reverses which graphs are filtered\n\
    -h, --help\n\
            print help message\n\
    -n, --assume-non-hamiltonian\n\
            let all graphs pass the non-hamiltonicity check; does not check\n\
            whether the graphs are actually non-hamiltonian\n\
    -t, --traceable\n\
            check for hamiltonian paths instead of cycles\n\
    -v, --verbose\n\
            verbose mode; if -a is absent prints one hamiltonian cycle (if one\n\
            exists); if -a is present prints all hamiltonian cycles; if\n\
            entering -v# or -v#1,#2 where # represents vertices of the\n\
            graph, a (or all) hamiltonian cycles of respectively G - #,\n\
            if -1 is present, or G - #1 - #2, if -2 is present, will be printed\n"


#include <stdio.h>
#include <stdbool.h>
#include <getopt.h>
#include <time.h>
#include "bitset.h"
#include "hamiltonicityMethods.h"
#include "readGraph/readGraph6.h"

//  Skip hamiltonicity check if -n or --assume-non-hamiltonian was given.
bool isNonHamOrAssumedNonHam(bitset adjacencyList[], int nVertices, bool
verboseFlag, bool assumeNonHamFlag, bool allCyclesFlag) {
    if(assumeNonHamFlag) {
        return true;
    }
    return !isHamiltonian(adjacencyList,nVertices, EMPTY, allCyclesFlag,
     verboseFlag);
} 

bool isNonTraceableOrAssumedNonTraceable(bitset adjacencyList[], int
nVertices, bool verboseFlag, bool assumeNonHamFlag, bool allCyclesFlag) {
    if(assumeNonHamFlag) {
        return true;
    }
    return !isTraceable(adjacencyList,nVertices, EMPTY, allCyclesFlag,
     verboseFlag);
} 

//  Determine whether the given graph should be written or not assuming it is
//  non-hamiltonian.
bool shouldWriteGraph(bitset adjacencyList[], int nVertices, bool verboseFlag,
bool allCyclesFlag, bool K1flag, bool K2flag, int vertexToCheck, int
vertexPairToCheck[]) {
    if (K1flag) {
        if(isK1Hamiltonian(adjacencyList, nVertices, verboseFlag,
         allCyclesFlag, vertexToCheck)) {
            return true;
        }
        return false;
    }
    if (K2flag) {
        if(isK2Hamiltonian(adjacencyList, nVertices, verboseFlag,
         allCyclesFlag, vertexPairToCheck)) {
            return true;
        }
        return false;
    }

    //  If -1 and -2 are absent we write non-hamiltonian graphs.
    return true;
}

int main(int argc, char ** argv) {
    bool assumeNonHamFlag = false;
    bool K1flag = false;
    bool K2flag = false;
    bool verboseFlag = false;
    bool allCyclesFlag = false;
    bool haveModResPair = false;
    bool complementFlag = false;
    bool traceableFlag = false;
    int mod, res;
    int vertexToCheck = -1;
    int vertexPairToCheck[2] = {-1,-1};
    int opt;
    while (1) {
        int option_index = 0;
        static struct option long_options[] = 
        {
            {"K1",                      no_argument, NULL,  '1'},
            {"K2",                      no_argument, NULL,  '2'},
            {"all-cycles",              no_argument, NULL,  'a'},
            {"complement",              no_argument, NULL,  'c'},
            {"help",                    no_argument, NULL,  'h'},
            {"assume-non-hamiltonian",  no_argument, NULL,  'n'},
            {"traceable",               no_argument, NULL,  't'},
            {"verbose",                 optional_argument, NULL, 'v'},
            {NULL,                      0,           NULL,    0}
        };

        opt = getopt_long(argc, argv, "12achntv::", long_options, &option_index);
        if (opt == -1) break;
        switch(opt) {
            case '1':
                K1flag = true;
                break;
            case '2':
                K2flag = true;
                break;
            case 'a':
                allCyclesFlag = true;
                break;
            case 'c':
                complementFlag = true;
                break;
            case 'h':
                fprintf(stderr, "%s\n", USAGE);
                fprintf(stderr, "%s", HELPTEXT);
                return 0;
            case 'n':
                assumeNonHamFlag = true;
                break;
            case 't':
                traceableFlag = true;
                break;
            case 'v':

                // TODO: Make this more safe and perhaps allow for multiple
                // vertices or vertexPairs using strtok.
                verboseFlag = true;
                sscanf(optarg ? optarg : "(none)", "%d", &vertexToCheck);
                if(sscanf(optarg ? optarg : "(none)", "%d,%d", 
                 &vertexPairToCheck[0], &vertexPairToCheck[1]) == 2) {
                    vertexToCheck = -1;
                }; 
                break;
            case '?':
                fprintf(stderr,"Error: Unknown option: %c\n", optopt);
                fprintf(stderr, "%s\n", USAGE);
                fprintf(stderr,
                 "Use ./hamiltonicityChecker --help for more detailed instructions.\n");
                return 1;
        }
    }

    int optionsNumber = (K1flag ? 1 : 0) |
                        (K2flag ? 2 : 0) |
                        (assumeNonHamFlag ? 4 : 0) |
                        (traceableFlag ? 8 : 0);

    //  Loop over non-option arguments.
    while (optind < argc) {
        if(sscanf(argv[optind], "%d/%d", &res, &mod) == 2) {
            if(haveModResPair) {
                fprintf(stderr,
                 "Error: You can only add one mod/res pair as an argument.\n");
                fprintf(stderr, "%s\n", USAGE);
                fprintf(stderr,
                 "Use ./hamiltonicityChecker --help for more detailed instructions.\n");
                return 1;
            }
            haveModResPair = true;
        }
        else {
            fprintf(stderr,"Error: Unknown argument: %s\n", argv[optind]);
            fprintf(stderr, "%s\n", USAGE);
            fprintf(stderr,
             "Use ./hamiltonicityChecker --help for more detailed instructions.\n");
            return 1;
        }
        optind++;
    }

    if (optionsNumber % 4 == 3) {
        fprintf(stderr,"Error: Do not use these flags simultaneously.\n");
        fprintf(stderr, "%s\n", USAGE);
        fprintf(stderr,
         "Use ./hamiltonicityChecker --help for more detailed instructions.\n");
        return 1;
    }

    if (optionsNumber == 10 || optionsNumber == 14) {
        fprintf(stderr,"Error: Not yet implemented.\n");
        fprintf(stderr, "%s\n", USAGE);
        fprintf(stderr,
         "Use ./hamiltonicityChecker --help for more detailed instructions.\n");
        return 1;
    }

    //  Only -n or -t and -n are present
    if(optionsNumber % 8 == 4) {
        fprintf(stderr,"Error: Use this flag only in combination with -1 or -2.\n");
        fprintf(stderr, "%s\n", USAGE);
        fprintf(stderr,
         "Use ./hamiltonicityChecker --help for more detailed instructions.\n");
        return 1;
    }

    if(!haveModResPair) {
        mod = 1;
        res = 0;
    }
    

    if(mod <= res) {
        fprintf(stderr, 
         "Error: The remainder cannot be higher or equal than the modulus. Remainders start at zero.\n");
        fprintf(stderr, "%s\n", USAGE);
        fprintf(stderr,
         "Use ./hamiltonicityChecker --help for more detailed instructions.\n");
        return 1;
    }

    if(MAXVERTICES != 64) {
        fprintf(stderr,
         "Warning: For graphs up to 64 vertices, the 64 bit version of this program is faster.\n");
    }

    unsigned long long int counter = 0;
    unsigned long long int total = 0;
    unsigned long long int nonHamiltonianCounter = 0;
    unsigned long long int amountPassed = 0;
    unsigned long long int skippedGraphs = 0;

    clock_t start = clock();

    //  Start looping over lines of stdin.
    char * graphString = NULL;
    size_t size;
    while(getline(&graphString, &size, stdin) != -1) {

        //  If for graph n: n % mod != res, skip the graph.
        if (total++ % mod != res) {
            continue;
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
        if(verboseFlag || allCyclesFlag) {
            fprintf(stderr, "Looking at: %s", graphString);
        }

        if(traceableFlag) {
            if(isNonTraceableOrAssumedNonTraceable(adjacencyList, nVertices,
             verboseFlag, assumeNonHamFlag, allCyclesFlag)) {
                nonHamiltonianCounter++;
                if(K1flag) {
                    if(isK1Traceable(adjacencyList, nVertices, allCyclesFlag,
                     verboseFlag, vertexToCheck)) {
                        if(!complementFlag) {
                            printf("%s", graphString);
                            amountPassed++;
                        }
                    }
                    else if(complementFlag) {
                        printf("%s", graphString);
                        amountPassed++;
                    }
                }
            }
            else if(complementFlag) {
                printf("%s", graphString);
                amountPassed++;
            }
        }

        else if(isNonHamOrAssumedNonHam(adjacencyList,nVertices,verboseFlag,
         assumeNonHamFlag,allCyclesFlag)){
            nonHamiltonianCounter++;
            if(shouldWriteGraph(adjacencyList,nVertices,verboseFlag,
             allCyclesFlag, K1flag, K2flag, vertexToCheck, vertexPairToCheck)) {
                if(!complementFlag) {
                    printf("%s", graphString);
                    amountPassed++;
                }
            }
            else if(complementFlag) {
                printf("%s", graphString);
                amountPassed++;
            }
        }

        //  If we print complements, a hamiltonian graph should always be written.
        else if(complementFlag) {
            printf("%s",graphString);
            amountPassed++;
        }

    }
    clock_t end = clock();
    double time_spent = (double)(end - start) / CLOCKS_PER_SEC;

    fprintf(stderr,"\rChecked %lld graphs in %f seconds: ", counter, time_spent);

    char *compString = "";
    if(complementFlag) {
        compString = "not ";
    }

    switch(optionsNumber) {
        //  hamiltonicity check
        case 0:
            if(!complementFlag) {
                fprintf(stderr, "%lld are non-hamiltonian, ", nonHamiltonianCounter);
            }
            else {
                fprintf(stderr, "%lld are hamiltonian, ", amountPassed);
            }
            break;

        //  hypohamiltonicity check
        case 1:
            if(!complementFlag) {
                fprintf(stderr,"%lld are non-hamiltonian, ", nonHamiltonianCounter);
            }
            fprintf(stderr,"%lld are %shypohamiltonian, ", amountPassed, compString);
            break;

        //  K2-hypohamiltonicity check
        case 2:
            if(!complementFlag) {
                fprintf(stderr,"%lld are non-hamiltonian, ", nonHamiltonianCounter);
            }
            fprintf(stderr,"%lld are %sK2-hypohamiltonian, ", amountPassed, compString);
            break;
        case 3:
            //  Cannot occur
            break;
        case 4:
            //  Cannot occur
            break;

        //  K1-hamiltonicity check
        case 5:
            fprintf(stderr,"%lld are %sK1-hamiltonian, ", amountPassed, compString);
            break;
        //  K2-hamiltonicity check
        case 6:
            fprintf(stderr,"%lld are %sK2-hamiltonian, ", amountPassed, compString);
            break;
        case 7:
            //  -1 and -2 cannot occur.
            break;

        //  traceability check
        case 8:
            if(!complementFlag) {
                fprintf(stderr, "%lld are non-traceable, ", nonHamiltonianCounter);
            }
            else {
                fprintf(stderr, "%lld are traceable, ", amountPassed);
            }
            break;

        //  hypotraceability check
        case 9:
            if(!complementFlag) {
                fprintf(stderr,"%lld are non-traceable, ", nonHamiltonianCounter);
            }
            fprintf(stderr, "%lld are %shypotraceable, ", amountPassed, compString);
            break;
        case 10:
            //  K2-hypotraceability not implemented.
            break;
        case 11:
            // -1 and -2 cannot occur.
        case 12:
            //  Should not occur
            break;
        case 13:
            fprintf(stderr, "%lld are %sK1-traceable, ", amountPassed, compString);
            break;
        case 14:
            //  K2-traceability not implemented.
            break;
        case 15:
            //  -1 and -2 cannot occur.
            break;

    }
    fprintf(stderr,"\b\b.\n");
    if(skippedGraphs) {
        fprintf(stderr, "Warning: %lld graphs could not be read.\n", skippedGraphs);
    }

    return 0;
}