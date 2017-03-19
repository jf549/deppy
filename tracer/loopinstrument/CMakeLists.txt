add_llvm_loadable_module(LoopInstrumentPass LoopInstrumentPass.cpp)

set_target_properties(LoopInstrumentPass PROPERTIES COMPILE_FLAGS "-fno-rtti")

if(APPLE)
  set_target_properties(LoopInstrumentPass PROPERTIES LINK_FLAGS "-undefined dynamic_lookup")
endif(APPLE)
