add_library(compiler_options INTERFACE)

# C++23
target_compile_features(compiler_options INTERFACE cxx_std_23)
set_target_properties(compiler_options PROPERTIES
    CXX_EXTENSIONS OFF
)

# Warnings
target_compile_options(compiler_options INTERFACE
    $<$<CXX_COMPILER_ID:GNU,Clang>:
        -Wall -Wextra -Wpedantic
        -Wconversion -Wshadow -Wnull-dereference
        -Wno-unused-parameter
    >
    $<$<CXX_COMPILER_ID:MSCV>:
        /W4 /permissive-
    >
)

# Debug
target_compile_options(compiler_options INTERFACE
    $<$<AND:$<CXX_COMPILER_ID:GNU,Clang>,$<CONFIG:Debug>>:
        -fsanitize=address,undefined
        -fno-omit-frame-pointer
    >
)
target_link_options(compiler_options INTERFACE
    $<$<AND:$<CXX_COMPILER_ID:GNU,Clang>,$<CONFIG:Debug>>:
        -fsanitize=address,undefined
    >
)

# Release
target_compile_options(compiler_options INTERFACE
    $<$<CONFIG:Release>:
        -O3 -DNDEBUG
    >
)