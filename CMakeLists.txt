cmake_minimum_required(VERSION 3.12)

project(Knapsack VERSION 1.0
    DESCRIPTION "Simple yet optimized 0-1 and unbounded Knapsack branch and bound solver"
    LANGUAGES CXX)

# set(CMAKE_VERBOSE_MAKEFILE ON)

#################### Options ####################
SET(CMAKE_CXX_STANDARD 17)
SET(CMAKE_CXX_STANDARD_REQUIRED ON)

option(WARNINGS "" OFF)
option(COMPILE_FOR_NATIVE "" OFF)
option(COMPILE_WITH_LTO "" OFF)

#################### Modules ####################
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})
include(CMakeBuildTypeUtils)

#################### Packages ####################
include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup(TARGETS)

################### LIB target ####################
add_library(knapsack INTERFACE)
target_include_directories(knapsack INTERFACE include)
target_link_libraries(knapsack PUBLIC INTERFACE CONAN_PKG::boost)
target_link_libraries(knapsack PUBLIC INTERFACE CONAN_PKG::range-v3)

# ################## TEST target ####################
enable_testing()

add_executable(optimum_value_test test/optimum_value_test.cpp)
target_link_libraries(optimum_value_test knapsack)
target_link_libraries(optimum_value_test CONAN_PKG::gtest)

include(GoogleTest)
gtest_discover_tests(optimum_value_test)

################### EXEC target ####################
add_executable(knapsack_bnb src/knapsack_bnb.cpp)
target_include_directories(knapsack_bnb PUBLIC include)
target_link_libraries(knapsack_bnb knapsack)
target_compile_options_if_exists(knapsack_bnb PRIVATE ${WARNINGS} -Wall)
target_compile_options_if_exists(knapsack_bnb PRIVATE ${COMPILE_FOR_NATIVE} -march=native)
target_compile_options_if_exists(knapsack_bnb PRIVATE ${COMPILE_WITH_LTO} -flto)

add_executable(unbounded_knapsack_bnb src/unbounded_knapsack_bnb.cpp)
target_include_directories(unbounded_knapsack_bnb PUBLIC include)
target_link_libraries(unbounded_knapsack_bnb knapsack)
target_compile_options_if_exists(unbounded_knapsack_bnb PRIVATE ${WARNINGS} -Wall)
target_compile_options_if_exists(unbounded_knapsack_bnb PRIVATE ${COMPILE_FOR_NATIVE} -march=native)
target_compile_options_if_exists(unbounded_knapsack_bnb PRIVATE ${COMPILE_WITH_LTO} -flto)

add_executable(knapsack_dp src/knapsack_dp.cpp)
target_include_directories(knapsack_dp PUBLIC include)
target_link_libraries(knapsack_dp knapsack)
target_compile_options_if_exists(knapsack_dp PRIVATE ${WARNINGS} -Wall)
target_compile_options_if_exists(knapsack_dp PRIVATE ${COMPILE_FOR_NATIVE} -march=native)
target_compile_options_if_exists(knapsack_dp PRIVATE ${COMPILE_WITH_LTO} -flto)