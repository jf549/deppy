clang -c -emit-llvm example.c
opt -loop-simplify example.bc -o example_simplify.bc
opt -load ../build/tracer/loopinstrument/LoopInstrumentPass.dylib -instrument-loops example_simplify.bc -o example_instloop.bc
opt -load ../build/tracer/meminstrument/MemInstrumentPass.dylib -instrument-mem example_instloop.bc -o example_inst.bc
llc example_inst.bc
clang -L../build/tracer/ example_inst.s -ltracer
