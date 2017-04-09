#!/bin/bash

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

clang -g -emit-llvm $1 -c -o $DIR/tmp0.bc

opt -loop-simplify $DIR/tmp0.bc -o $DIR/tmp1.bc
opt -load $DIR/loopinstrument/LoopInstrumentPass.dylib -instrument-loops $DIR/tmp1.bc -o $DIR/tmp2.bc
opt -load $DIR/meminstrument/MemInstrumentPass.dylib -instrument-mem $DIR/tmp2.bc -o $DIR/tmp3.bc

llc $DIR/tmp3.bc

clang -L$DIR $DIR/tmp3.s -ltracer -o $DIR/tracer
