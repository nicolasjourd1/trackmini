add_executable(test_platform
    test_window.cpp
)

target_link_libraries(test_platform PRIVATE
    engine
    GTest::gtest_main
)

gtest_discover_tests(test_platform)