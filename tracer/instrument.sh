#!/bin/bash

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

clang -g -c -emit-llvm -o - "$@" |
opt -loop-simplify |
opt -load $DIR/loopinstrument/LoopInstrumentPass.dylib -instrument-loops |
opt -load $DIR/meminstrument/MemInstrumentPass.dylib -instrument-mem |
clang -x ir -L$DIR -ltracer -O2 -o $DIR/tracer -
