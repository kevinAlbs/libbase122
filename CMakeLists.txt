cmake_minimum_required(VERSION 3.0.0)
project(libbase122 VERSION 0.1.0 LANGUAGES C)

add_library(base122 src/base122.c)

add_executable (test-libbase122 test/test.c test/assertions.c)
target_link_libraries(test-libbase122 base122)
# Use C99 in test-libbase122.
set_property(TARGET test-libbase122 PROPERTY C_STANDARD 99)
if (ENABLE_FUZZER)
    message ("Enabling fuzzer targets")
    add_executable (fuzz-libbase122 test/fuzz.c test/assertions.c)
    target_compile_options(fuzz-libbase122 PRIVATE -fsanitize=fuzzer)
    target_link_options(fuzz-libbase122 PRIVATE -fsanitize=fuzzer)
    target_link_libraries(fuzz-libbase122 base122)
    set_property(TARGET fuzz-libbase122 PROPERTY C_STANDARD 99)
endif()
target_include_directories(test-libbase122 PRIVATE ${CMAKE_SOURCE_DIR})

enable_testing()
add_test (NAME test-libbase122 COMMAND test-libbase122)

add_executable (b122 src/b122.c)
target_link_libraries(b122 base122)
install (TARGETS base122 b122)
install (FILES src/base122.h TYPE INCLUDE)
add_executable (example src/example.c)
target_link_libraries(example base122)