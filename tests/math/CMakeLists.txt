add_executable(test_math
    test_vec3.cpp
    test_mat4.cpp
    test_quat.cpp
)

target_link_libraries(test_math PRIVATE
    engine
    GTest::gtest_main
)

gtest_discover_tests(test_math)