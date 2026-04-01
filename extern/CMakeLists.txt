add_library(glad STATIC
    glad/src/glad.c
)
target_include_directories(glad PUBLIC glad/include)

target_compile_options(glad PRIVATE
    $<$<CXX_COMPILER_ID:GNU,Clang>:-w>
)

include(FetchContent)
FetchContent_Declare(
    SDL3
    GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
    GIT_TAG        main   # en prod on pinnerait un tag précis ex: release-3.2.0
    GIT_SHALLOW    TRUE   # clone superficiel, plus rapide
)

set(SDL_SHARED OFF CACHE BOOL "" FORCE)
set(SDL_STATIC ON  CACHE BOOL "" FORCE)
set(SDL_TEST   OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(SDL3)