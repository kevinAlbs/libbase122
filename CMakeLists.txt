cmake_minimum_required(VERSION 3.0.0)
project(libbase122 VERSION 0.1.0)

add_library(libbase122 src/libbase122.c)

add_executable (test-libbase122 test/test.c test/assertions.c)
target_link_libraries(test-libbase122 libbase122)
# Use C99 in test-libbase122.
set_property(TARGET test-libbase122 PROPERTY C_STANDARD 99)
if (ENABLE_FUZZER)
    message ("Enabling fuzzer targets")
    add_executable (fuzz-libbase122 test/fuzz.c test/assertions.c)
    target_compile_options(fuzz-libbase122 PRIVATE -fsanitize=fuzzer)
    target_link_options(fuzz-libbase122 PRIVATE -fsanitize=fuzzer)
    target_link_libraries(fuzz-libbase122 libbase122)
    set_property(TARGET fuzz-libbase122 PROPERTY C_STANDARD 99)
endif()

enable_testing()
add_test (NAME test-libbase122 COMMAND test-libbase122)