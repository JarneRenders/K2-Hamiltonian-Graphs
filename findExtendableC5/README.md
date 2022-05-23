## findExtendableC5
### Short manual

The latest version of this program can be obtained from: <https://github.com/JarneRenders/K2-Hamiltonian-Graphs/>.

This program can be used to find an extendable 5-cycle in a graph and to filter graphs which contain one. For a definition and more information on this topic, see "C. T. Zamfirescu, K<sub>2</sub>-Hamiltonian Graphs: I,
<i>SIAM J. Discrete Math.</i> <b>35</b> (2021) 1706--1728" or "J. Goedgebeur, J. Renders, G. Wiener and C.T. Zamfirescu, K<sub>2</sub>-Hamiltonian Graphs: II, manuscript".

The program supports graphs up to and including 128 vertices.

### Installation

This requires a working shell and `make`. Navigate to `findExtendableC5/` and compile using: 

* `make` to create a binary for the 64-bit version;
* `make 128bit` to create a binary for the 128-bit version;
* `make 128bitarray` to create a binary for an alternative 128-bit version;
* `make all` to create all the above binaries.

The 64-bit version supports graphs only up to 64 vertices, the 128-bit versions up to 128 vertices. For graphs containing up to 64 vertices the 64-bit version performs siginificantly faster than the 128-bit versions. Use `make clean` to remove all binaries created in this way.

### Usage of findExtendableC5

All options can be found by executing `./findExtendableC5 -h`.

Usage: `./findExtendableC5 [-a|-c#,#,#,#,#] [-pv] [-h]`

Filter graphs that contain an extendable 5-cycle. 

Graphs are read from stdin in graph6 format. Graphs are sent to stdout in graph6 format. If the input graph had a graph6 header, so will the output graph (if it passes through the filter).

The order in which the arguments appear does not matter, unless multiple instances of `-c` are given (the lastmost instance will be chosen).

```
	-a, --all
		count all extendable 5-cycles in the graph; cannot be used with -c
	-c, --cycle
		specify a cycle for which to check whether it is extendable; if it is and -p is present this will be sent to stdout
	-h, --help
		print out help message
	-p, --print
		if a cycle is found it will be sent to stdout; with -a this will be the last found, with -c this will be the specified cycle (if it is extendable) and without these flags it will be the first found
	-v, --verbose
		verbose mode; for each checked extendable 5-cycle print out the paths showing it is one
```

### Examples
`./findExtendableC5`
Sends all graphs from stdin containing an extendable 5-cycle to stdout.

`./findExtendableC5 -p`
The same as the previous but also sends an extendable 5-cycle to stdout after every graph.

`./findExtendableC5 -a`
Same as the first, but also counts how many extendable 5-cycles there are.

`./findExtendableC5 -v`
Same as the first, but if a graph has an extendable 5-cycle, ten cycles are printed which prove it is one.

`./findExtendableC5 -va`
Same as first, but shows all extendable 5-cycles in the graph and for each prints ten cycles proving it is one.

`./findExtendableC5 -c0,1,2,3,4`
Sends all graphs from stdin for which 0,1,2,3,4 is an extendable 5-cycle to stdout.

`./findExtendableC5 -c0,1,2,3,4 -v`
Same as the previous one, but also prints ten cycles proving it is an extendable 5-cycle (if it is one).