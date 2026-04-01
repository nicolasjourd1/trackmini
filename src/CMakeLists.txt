add_library(engine STATIC
    platform/Window.cpp
)

target_include_directories(engine PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(engine PUBLIC
    compiler_options
    glad
    SDL3::SDL3-static
)

add_executable(trackmini main.cpp)
target_link_libraries(trackmini PRIVATE engine)