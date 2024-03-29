# Deppy
An efficient dynamic data-dependence profiler

## Building
```shell
$ mkdir build
$ cd build
$ cmake ..
$ make
```

## Running
### Instrumenting a C program
```shell
$ cd build
$ ./tracer/instrument.sh path/to/source.c
```
The C program will be compiled with instrumentation into the executable `build/tracer/tracer`.

### Running the profiler
```shell
$ cd build
$ ./tracer/tracer | ./analyser/deppy
```

### Running the unit tests
```shell
$ cd build
$ ./test/tests
```
