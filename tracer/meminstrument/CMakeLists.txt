add_llvm_loadable_module(MemInstrumentPass MemInstrumentPass.cpp)

# LLVM is built without RTTI
set_target_properties(MemInstrumentPass PROPERTIES COMPILE_FLAGS "-fno-rtti")

# Get Linux-like shared library behaviour on macOS where undefined symbols are looked-up
# dynamically
if(APPLE)
  set_target_properties(MemInstrumentPass PROPERTIES LINK_FLAGS "-undefined dynamic_lookup")
endif(APPLE)
