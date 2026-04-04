add_executable(test_editor
    test_command_history.cpp
    test_raycast.cpp
)

target_link_libraries(test_editor PRIVATE 
    engine 
    GTest::gtest_main
)

gtest_discover_tests(test_editor)