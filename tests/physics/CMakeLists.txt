add_executable(test_physics
    test_rigidbody.cpp
    test_vehicle.cpp
)

target_link_libraries(test_physics PRIVATE
    engine 
    GTest::gtest_main
)

gtest_discover_tests(test_physics)