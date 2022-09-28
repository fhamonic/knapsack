# ################### Packages ###################
find_package(GTest)
include(GoogleTest)

# ################# TEST target ##################
add_executable(optimum_value_test optimum_value_test.cpp)
target_link_libraries(optimum_value_test GTest::gtest)
target_link_libraries(optimum_value_test knapsack)
gtest_discover_tests(optimum_value_test)
