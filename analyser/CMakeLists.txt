add_library (Analyser
             DebugInfo.cpp
             Loop.cpp
             Stride.cpp
             StrideDetector.cpp
             StrideLoop.cpp)

add_executable(deppy deppy.cpp)

target_link_libraries (deppy Analyser)
