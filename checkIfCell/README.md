## checkIfCell
### Short manual

The latest version of this program can be obtained from: <https://github.com/JarneRenders/K2-Hamiltonian-Graphs/>.

This program can be used to filter suitable cells, K1-cells and K2-cells. For more information on these concepts, see "J. Goedgebeur, J. Renders, G. Wiener and C.T. Zamfirescu, K<sub>2</sub>-Hamiltonian Graphs: II, manuscript".

The program supports graphs up to and including 128 vertices.

### Installation

This requires a working shell and `make`. Navigate to `checkIfCell/` and compile using: 

* `make` to create a binary for the 64-bit version;
* `make 128bit` to create a binary for the 128-bit version;
* `make 128bitarray` to create a binary for an alternative 128-bit version;
* `make all` to create all the above binaries.

The 64-bit version supports graphs only up to 64 vertices, the 128-bit versions up to 128 vertices. For graphs containing up to 64 vertices the 64-bit version performs siginificantly faster than the 128-bit versions. Use `make clean` to remove all binaries created in this way.

### Usage of checkCell

All options can be found by executing `./checkCell -h`.

Usage: `./checkCell [-1|2] [-psSv] [-o#,#,#,#] [-h]`

Filter graphs that satisfy the conditions for being a suitable cell, a K1-cell or a K2-cell.

Graphs are read from stdin in graph6 format. Graphs are sent to stdout in graph6 format. If the input graph had a graph6 header, so will the output graph (if it passes through the filter).

The order in which the arguments appear does not matter, unless multiple instances of `-o` are given (the lastmost instance will be chosen).

```
	-1, --k1cell
		let the graphs which contain some outer vertices that form a K1-cell pass through the filter; if -s is present the graph with the specified outer vertices is assumed to be suitable; do not use with -2
	-2, --k2cell
		let the graphs which contain some outer vertices that form a K2-cell pass through the filter; if -s is present the graph with the specified outer vertices is assumed to be suitable; do not use with -1
	-h, --help
		print out help message
	-o, --outer-vertices
		will only do the checks for the specified outer vertices; use -o#,#,#,# where # represents an outer vertex; these need to be distinct and in the graph; suitability of all permutations of these vertices will be checked; if combining with -s the order does matter
	-p, --paths
		print the paths necessary to show that a suitable cell is K1 or K2; if it is not all paths will be printed until the point of failure; best combined with -v
	-s, --suitable
		skip the suitability check and assume all input graphs contain a suitable cell at the specified vertices; this option requires -o
	-S, --statistics
		print out which conditions of the suitability check failed how many percent of the time and how much percent of the computation they took
	-v, --verbose
		verbose mode; print out which vertices belong to the suitable cell and where the point of failure is in the K1- or K2-cell computation (if applicable)

```

### Examples
`./checkCell`
Sends all graphs from stdin that contain a suitable cell to stdout.

`./checkCell -1`
Sends all graphs from stdin that contain a K1-cell to stdout.

`./checkCell -2`
Sends all graphs from stdin that contain a K2-cell to stdout.

`./checkCell -2pv`
The same as the previous, but mentions when a graph is suitable or not and where it failed in the K2-cell check (if it did) as well as printing out all paths (up until failure) which are necessary to prove the suitable cell is a K2-cell.

`./checkCell -o1,2,3,4`
Sends all graphs from stdin for which any permutation of the vertices 1, 2, 3 and 4 is a suitable cell to stdout.

`./checkCell -o1,2,3,4 -1`
Sends all graphs from stdin for which any permutaiton of the vertices 1, 2, 3 and 4 is a K1-cell to stdout.

`./checkCell -o1,2,3,4 -1s`
Skips the suitability check. Sends all graphs from stdin for which (G,1,2,3,4) is a K1-cell (under the assumption that it is suitable) to stdout.
