clang -c -emit-llvm example.c
opt -load build/instrument/libInstrumentPass.so -instrument < example.bc > example_inst.bc
llc example_inst.bc
clang -S tracerlib.c
clang tracerlib.s example_inst.s
