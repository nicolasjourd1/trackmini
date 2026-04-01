cmake_minimum_required(VERSION 3.28)

project(trackmini
    VERSION 0.1.0
    LANGUAGES C CXX
)

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
    message(FATAL_ERROR "In source builds are not allowed. Use: cmake -B build")
endif()

include(cmake/CompilerOptions.cmake)

# Dependencies
add_subdirectory(extern)

# Sources
add_subdirectory(src)

# Tests
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    enable_testing()
    add_subdirectory(tests)
endif()