add_executable(test_engine
    test_clock.cpp
    test_eventbus.cpp
    test_gameloop.cpp
)

target_link_libraries(test_engine PRIVATE
    engine
    GTest::gtest_main
)

gtest_discover_tests(test_engine)