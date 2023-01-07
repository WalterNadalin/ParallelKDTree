## Compile
To compile the serial version:

```bash
make
```

to compile the parallel version (requires `mpic++`):
```bash
make parallel=1
```

to remove the executable and objective files generated:
```
make clean
```

to format the source and the headers (requires `clang-format`):
```
make format
```

modify the `MakeFile` at will for more functionalities.

## Run
If the serial version hase been compiled, to run the serial version with `h` 2 dimensional points use the command:

```bash
.\kdtree.x <h> <command>
```

`command` allows to decide which information to print:

- `print`: prints the *k*-d tree data structure on terminal;
- `time`: prints the construction and execution time ;
- `info`: prints the times and other information about the threads;
- `all`: prints all the above.
	
If the serial parallel hase been compiled, to run the parallel version with `h` 2 dimensional points, `m` processes and `n` threads use the command:

```bash
mpirun -np <m> .\kdtree.x <h> <command> <n>
```

and that should do the trick.

## Output
The program will produce a different output based on the preferences of the user (see above).
