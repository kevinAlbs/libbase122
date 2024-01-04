cmake_minimum_required(VERSION 3.0.0)
project(libbase122 VERSION 0.1.0 LANGUAGES C)

add_library(base122 src/base122.c)
add_library(base122_shared SHARED src/base122.c)

target_compile_definitions(base122 PUBLIC BASE122_STATIC)
target_compile_definitions(base122 PRIVATE BASE122_COMPILING)
target_compile_definitions(base122_shared PRIVATE BASE122_COMPILING)

if (ENABLE_BASE122_TEST)
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
endif()

add_executable (b122 src/b122.c)
target_link_libraries(b122 base122)
install (TARGETS base122 b122 base122_shared)
install (FILES src/base122.h TYPE INCLUDE)
add_executable (example src/example.c)
target_link_libraries(example base122)
# Export CMake targets.
set_target_properties(base122 PROPERTIES EXPORT_NAME base122::static)
set_target_properties(base122_shared PROPERTIES EXPORT_NAME base122::shared)
target_include_directories(base122 INTERFACE $<INSTALL_INTERFACE:include>)
target_include_directories(base122_shared INTERFACE $<INSTALL_INTERFACE:include>)
install (TARGETS base122 base122_shared EXPORT Base122Targets)
install (EXPORT Base122Targets FILE Base122Targets.cmake DESTINATION lib/cmake/Base122)
include (CMakePackageConfigHelpers)
configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Base122Config.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/Base122Config.cmake"
    INSTALL_DESTINATION "lib/cmake/Base122"
    NO_SET_AND_CHECK_MACRO
)
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/Base122Version.cmake"
    COMPATIBILITY AnyNewerVersion
)
install (FILES
    "${CMAKE_CURRENT_BINARY_DIR}/Base122Config.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/Base122Version.cmake"
    DESTINATION "lib/cmake/Base122"
)