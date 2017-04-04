add_executable(tests
               test_main.cpp
               test_analyser.cpp
               test_lib.cpp)

target_link_libraries (tests Analyser)

add_test(NAME tests COMMAND tests)
