cmake_minimum_required(VERSION 3.0.0)
project(libbase122 VERSION 0.1.0)

add_library(libbase122 src/libbase122.c)

add_executable (test-libbase122 test/test.c test/assertions.c)
target_link_libraries(test-libbase122 libbase122)
# Use C99 in test-libbase122.
set_property(TARGET test-libbase122 PROPERTY C_STANDARD 99)
