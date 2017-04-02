add_library (Analyser Loop.cpp StrideLoop.cpp StrideDetector.cpp DynamicGcd.cpp)
add_executable(deppy deppy.cpp)
target_link_libraries (deppy Analyser)
