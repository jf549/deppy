#!/bin/bash

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

i=0

for var in "$@"
do
  clang -g -c -emit-llvm -o $DIR/tmp$i.bc $var
  params="$params $DIR/tmp$i.bc"
  ((++i))
done

llvm-link $params |
opt -loop-simplify |
opt -load $DIR/loopinstrument/LoopInstrumentPass.dylib -instrument-loops |
opt -load $DIR/meminstrument/MemInstrumentPass.dylib -instrument-mem |
clang -x ir -L$DIR -ltracer -o $DIR/tracer -
