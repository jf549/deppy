add_library(Analyser
            DebugInfo.cpp
            Logger.cpp
            Loop.cpp
            PointLoop.cpp
            Stride.cpp
            StrideDetector.cpp
            StrideLoop.cpp)

target_link_libraries(Analyser gmpxx gmp)

add_executable(deppy deppy.cpp)

target_link_libraries(deppy Analyser)
