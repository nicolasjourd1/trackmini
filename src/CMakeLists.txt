add_library(engine STATIC
    engine/GameLoop.cpp
    platform/Window.cpp
    renderer/Camera.cpp
    renderer/Mesh.cpp
    renderer/Shader.cpp
    renderer/VertexArray.cpp
    track/BlockCatalog.cpp
    track/Grid.cpp
    track/Track.cpp
    editor/CommandHistory.cpp
    editor/EditorState.cpp
    editor/TrackRenderer.cpp
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