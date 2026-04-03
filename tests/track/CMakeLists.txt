add_executable(test_track
    test_grid.cpp
    test_catalog.cpp
    test_serializer.cpp
)

target_link_libraries(test_track PRIVATE
    engine
    GTest::gtest_main
)

gtest_discover_tests(test_track)