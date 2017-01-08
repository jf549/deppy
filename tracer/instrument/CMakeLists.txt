add_library(InstrumentPass MODULE InstrumentPass.cpp)

target_compile_features(InstrumentPass PRIVATE cxx_range_for cxx_auto_type)

set_target_properties(InstrumentPass PROPERTIES COMPILE_FLAGS "-fno-rtti")

if(APPLE)
  set_target_properties(InstrumentPass PROPERTIES LINK_FLAGS "-undefined dynamic_lookup")
endif(APPLE)
