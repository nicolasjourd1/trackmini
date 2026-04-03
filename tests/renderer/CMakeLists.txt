add_executable(test_renderer
    test_shader.cpp
    test_camera.cpp
)

target_link_libraries(test_renderer PRIVATE
    engine
    GTest::gtest_main
)

gtest_discover_tests(test_renderer)