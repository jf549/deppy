add_llvm_loadable_module(MemInstrumentPass MemInstrumentPass.cpp)

set_target_properties(MemInstrumentPass PROPERTIES COMPILE_FLAGS "-fno-rtti")

if(APPLE)
  set_target_properties(MemInstrumentPass PROPERTIES LINK_FLAGS "-undefined dynamic_lookup")
endif(APPLE)
