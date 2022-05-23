# K2-hamiltonian Graphs
This repository contains four programs which were created for the article "J. Goedgebeur, J. Renders, G. Wiener and C.T. Zamfirescu, K<sub>2</sub>-Hamiltonian Graphs: II, manuscript", as well as some certificates that can be used to verify some of the computer-aided proofs in this article. The latter can be found in the folder `certificates`.

In the root folder one can find `hamiltonicityChecker`, which can be used to filter graphs satisfying certain hamiltonicity properties.

In the folder `checkIfCell` one can find the program `checkCell`. This filters graphs which are so-called suitable cells, K1-cells or K2-cells (see article for more details).

In the folder `findExtendableC5`, one can find the program `findExtendableC5`, which filters graphs containing an extendable 5-cycle (see article or "C. T. Zamfirescu, K<sub>2</sub>-Hamiltonian Graphs: I, <i>SIAM J. Discrete Math.</i> <b>35</b> (2021) 1706--1728").

In the folder `satisfiesDotProduct`, one can find the program `satisfiesDotProduct`, which can be used to filter graphs satisfying certain conditions involving the application of the dot product on K2-hamiltonian graphs (see its readme and article).

Each of these subfolders contain their own readme as well as a makefile for compiling the programs.

These programs all use Brendan McKay's graph6 format to read and write graphs. See <http://users.cecs.anu.edu.au/~bdm/data/formats.txt>.

## hamiltonicityChecker
### Short manual

The latest version of this program can be obtained from: <https://github.com/JarneRenders/K2-Hamiltonian-Graphs/>.

This program can be used to filter non-hamiltonian, hypohamiltonian (i.e. non-hamiltonian and K1-hamiltonian) or K2-hypohamiltonian (i.e. non-hamiltonian and K2-hamiltonian) graphs out of a list of graphs. One can also filter the graphs which are K1-hamiltonian, K2-hamiltonian or any complement of these options by passing arguments to the program.

The program supports graphs up to and including 128 vertices.

### Installation

This requires a working shell and `make`. Navigate to the folder containing hamiltonicityChecker.c and compile using: 

* `make` to create a binary for the 64-bit version;
* `make 128bit` to create a binary for the 128-bit version;
* `make 128bitarray` to create a binary for an alternative 128-bit version;
* `make all` to create all the above binaries.

The 64-bit version supports graphs only up to 64 vertices, the 128-bit versions up to 128 vertices. For graphs containing up to 64 vertices the 64-bit version performs siginificantly faster than the 128-bit versions. Typically, the 128-bit array version performs faster than the standard 128-bit version. Use `make clean` to remove all binaries created in this way.

### Usage of hamiltonicityChecker

All options can be found by executing `./hamiltonicityChecker -h`.

Usage: `./hamiltonicityChecker [-1|-2] [-n] [-c] [-v] [-v#] [-v#,#] [-a] [-h] [res/mod]`

Filter graphs satisfying certain hamiltonicity requirements.

Graphs are read from stdin in graph6 format. Graphs are sent to stdout in graph6 format. If the input graph had a graph6 header, so will the output graph (if it passes through the filter).

The order in which the arguments appear does not matter, unless multiple instances of `-v` with an optional argument are given (the lastmost instance will be chosen).
```
	-1, --K1-hamiltonian  
		let the K1-hamiltonian graphs pass through the filter; if -n and -c are not present this will send all hypohamiltonian graphs to stdout; cannot be used with -2
	-2, --K2-hamiltonian			
		let the K2-hamiltonian graphs pass through the filter; if -n and -c are not present this will send all K2-hypohamiltonian graphs to stdout; cannot be used with -1  
	-a, --all-cycles			
		counts all hamiltonian cycles of the graph; if -v is present these cycles get printed; if -v together with an optional argument is present, this is also done for the corresponding subgraph 
	-c, --complement			
		reverses which graphs are filtered 
	-h, --help
		print help message
	-n, --assume-non-hamiltonian		
		let all graphs pass the non-hamiltonicity check; does not check whether the graphs are actually non-hamiltonian  
	-v, --verbose				
		verbose mode; if -a is absent prints one hamiltonian cycle (if one exists); if -a is present prints all hamiltonian cycles; if entering -v# or -v#1,#2 where # represents vertices of the graph, a (or all) hamiltonian cycles of respectively G - # if -1 is present or G - #1 - #2 if -2 is present will be printed
```

### Examples

`./hamiltonicityChecker`
Sends all non-hamiltonian graphs from stdin to stdout.

`./hamiltonicityChecker -2`
Sends all non-hamiltonian K2-hamiltonian (K2-hypohamiltonian) graphs from stdin to stdout.

`./hamiltonicityChecker -n1`
Skips the hamiltonicity check but filters graphs which are not K1-hamiltonian. Precisely the K1-hamiltonian graphs are sent to stdout.

`./hamiltonicityChecker -cn2`
Sends the complement of all K2-hamiltonian graphs (argument -n ignores the hamiltonicity check) to stdout, i.e. all non-K2-hamiltonian graphs.

`./hamiltonicityChecker -a`
Sends the non-hamiltonian graphs to stdout and sends to stderr how many hamiltonian cycles were present in each input graph.

`./hamiltonicityChecker -v -1`
Sends all K1-hypohamiltonian graphs to stdout and for each input graph a hamiltonian cycle is sent to stderr (if it exists).

`./hamiltonicityChecker -v10 -1`
Same as the previous, but for each input graph G a hamiltonian cycle of G - 10 is sent to stderr (if it exists and if 10 is in the graph).

`./hamiltonicityChecker -v -a`
Sends the non-hamiltonian graphs to stdout and sends to stderr how many hamiltonian cycles were present in each input graph and prints each of these.

`./hamiltonicityChecker -v10,5 -a`
Sends the K2-hypohamiltonian graphs to stdout and sends to stderr how many hamiltonian cycles were present in each input and prints each of these and does the same for the subgraphs G - 10 - 5 (if vertices 5 and 10 are present in the graph.)


### Changelog

* 2022-05-23 First release.



