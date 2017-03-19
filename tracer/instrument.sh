clang -c -emit-llvm example.c
opt -loop-simplify example.bc -o example_simplify.bc
opt -load build/instrument/InstrumentPass.dylib -instrument example_simplify.bc -o example_inst.bc
llc example_inst.bc
clang -S tracerlib.c
clang tracerlib.s example_inst.s
