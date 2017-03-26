add_executable(tests test_main.cpp test_lib.cpp)

add_test(NAME tests COMMAND tests)
