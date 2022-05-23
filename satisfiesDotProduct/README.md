## satisfiesDotProduct
### Short manual

The latest version of this program can be obtained from: <https://github.com/JarneRenders/K2-Hamiltonian-Graphs/>.

This program can be used to filter graphs that satisfy certain conditions involving the dot product and K2-hamiltonian graphs. For more details, see "J. Goedgebeur, J. Renders, G. Wiener and C.T. Zamfirescu, K<sub>2</sub>-Hamiltonian Graphs: II, manuscript". In particular, the conditions we mention are (i)-(vi) of Theorem 5, which we split up in the first three and the last three, and the conditions of Lemma 6, which we refer to as the extra conditions.

The program supports graphs up to and including 128 vertices.

### Installation

This requires a working shell and `make`. Navigate to `satisfiesDotProduct/` and compile using: 

* `make` to create a binary for the 64-bit version;
* `make 128bit` to create a binary for the 128-bit version;
* `make 128bitarray` to create a binary for an alternative 128-bit version;
* `make all` to create all the above binaries.

The 64-bit version supports graphs only up to 64 vertices, the 128-bit versions up to 128 vertices. For graphs containing up to 64 vertices the 64-bit version performs siginificantly faster than the 128-bit versions. Use `make clean` to remove all binaries created in this way.

### Usage of satisfiesDotProduct

All options can be found by executing `./satisfiesDotProduct -h`.

Usage: `./satisfiesDotProduct [-1|-2] [-apv] [-e] [-e#,#] [-P#,#] [-P#,#,#,#] [-h]`

All options can be found by executing `./satisfiesDotProduct -h`.

Filter graphs satisfying certain conditions involving the dot product and K2-hamiltonian graphs. Refer to `satifiesDotProduct/README.md` for more details on these conditions.

Graphs are read from stdin in graph6 format. Graphs are sent to stdout in graph6 format. If the input graph had a graph6 header, so will the output graph (if it passes through the filter).

The order in which the arguments appear does not matter, unless multiple instances of flags with an optional or required argument are given (the lastmost instance will be chosen). Concatenating an option after one with an argument will ignore the latter, e.g.: `-e5,6v` will ignore the `-v` flag.

```
	-1, --first
		let the graphs satisfying the first three conditions (i)-(iii) for some pair of independent edges pass through the filter; should not be used with -2; default
	-2, --last
		let the graphs satsifying the last three conditions (iv)-(vi) for some pair of independent edges pass through the filter; should not be used with -1
	-a, --all
		count how many pairs of independent edges (-1) or adjacent cubic vertices (-2) in this graph satisfy the conditions to be checked; if -v is present show all these pairs
	-e, --extra
		only use with -1; graphs will pass through the filter if the first three conditions are satisfied and some pair of adjacent cubic vertices satisfies the extra conditions; if used as -e#1,#2 require that (#1,#2) is a pair of adjacent cubic vertices satisfying the extra conditions
	-h, --help
		print help message
	-p, --print
		send a pair of independent edges (-1) or adjacent cubic vertices (-2) satisfying the required conditions to stdout; if -a is present send all such pairs
	-P, --paths
		requires -P#1,#2,#3,#4 (-1) or -P#1,#2 (-2); only checks whether the required conditions are satisfied by the pair of independent edges (#1,#2) and (#3,#4) or the pair of adjacent cubic vertices (#1,#2); shows all paths necessary in proving it is so
	-v, --verbose
		verbose mode; if a graph passes the filter, show a pair satisfying the required conditions; if used with -a show all pairs
```

### Examples
`./satisfiesDotProduct`
Sends all input graphs that satisfy the first three conditions, (i)-(iii), for some pair of independent edges to stdout.

`./satifiesDotProduct -2`
Sends all input graphs that satisfy the last three conditions, (iv)-(vi), for some pair of adjacent cubic vertices to stdout.

`./satisfiesDotProduct -e`
Sends all input graphs that satisfy the first three conditions, (i)-(iii), and the extra conditions for some pair of independent edges and some pair of adjacent cubic vertices to stdout.

`./satisfiesDotProduct -e0,1`
Sends all input graphs that satisfy the first three conditions for some pair of independent edges and for which the pair of adjacent vertices (0,1) satisfy the extra conditions to stdout.

`./satifiesDotProduct -a`
Same as the first example, but also counts how many such pairs of independent edges there are.

`./satifiesDotProduct -ae`
Same as the third example, but also counts how many such pairs of independent edges contain a pair of adjacent cubic vertices satisfying the extra conditions.

`./satifiesDotProduct -v`
Same as the first example, but shows a pair of independent edges satisfying the conditions.

`./satifiesDotProduct -P0,1,2,3`
Sends all input graphs for which the pair of independent edges (0,1) and (2,3) satisfies the first three conditions to stdout.

`./satifiesDotProduct -P0,1`
Sends all input graphs for which the pair of adjacent cubic vertices (0,1) satisfies the last three conditions to stdout.