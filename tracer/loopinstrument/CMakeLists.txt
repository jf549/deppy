add_llvm_loadable_module(LoopInstrumentPass LoopInstrumentPass.cpp)

# LLVM is built without RTTI
set_target_properties(LoopInstrumentPass PROPERTIES COMPILE_FLAGS "-fno-rtti")

# Get Linux-like shared library behaviour on macOS where undefined symbols are looked-up
# dynamically
if(APPLE)
  set_target_properties(LoopInstrumentPass PROPERTIES LINK_FLAGS "-undefined dynamic_lookup")
endif(APPLE)
