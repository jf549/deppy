add_library (Analyser Loop.cpp)
add_executable(deppy deppy.cpp)
target_link_libraries (deppy Analyser)
