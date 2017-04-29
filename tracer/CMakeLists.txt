add_library(tracer tracerlib.c)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/instrument.sh DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/ccinst DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

find_package(LLVM REQUIRED CONFIG)

list(APPEND CMAKE_MODULE_PATH "${LLVM_CMAKE_DIR}")
include(AddLLVM)

add_definitions(${LLVM_DEFINITIONS})
include_directories(SYSTEM ${LLVM_INCLUDE_DIRS})

add_subdirectory(meminstrument)
add_subdirectory(loopinstrument)
