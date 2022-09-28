# ################# EXEC target ##################
add_executable(knapsack_bnb knapsack_bnb.cpp)
target_link_libraries(knapsack_bnb knapsack)

add_executable(unbounded_knapsack_bnb unbounded_knapsack_bnb.cpp)
target_link_libraries(unbounded_knapsack_bnb knapsack)

add_executable(knapsack_dp knapsack_dp.cpp)
target_link_libraries(knapsack_dp knapsack)
