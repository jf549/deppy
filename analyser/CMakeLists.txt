find_package(Boost 1.63 COMPONENTS program_options REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})

add_library(Analyser
            DebugInfo.cpp
            DependenceResults.cpp
            Loop.cpp
            PointLoop.cpp
            Stride.cpp
            StrideDetector.cpp
            StrideLoop.cpp)

target_link_libraries(Analyser gmp ${Boost_LIBRARIES})

add_executable(deppy deppy.cpp)

target_link_libraries(deppy Analyser)
