add_llvm_loadable_module(InstrumentPass InstrumentPass.cpp)

set_target_properties(InstrumentPass PROPERTIES COMPILE_FLAGS "-fno-rtti")

if(APPLE)
  set_target_properties(InstrumentPass PROPERTIES LINK_FLAGS "-undefined dynamic_lookup")
endif(APPLE)
