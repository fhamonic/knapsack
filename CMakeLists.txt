cmake_minimum_required(VERSION 3.12)

project(
    Knapsack
    VERSION 1.0
    DESCRIPTION
        "Simple yet optimized 0-1 and unbounded Knapsack branch and bound solver"
    LANGUAGES CXX)

# set(CMAKE_VERBOSE_MAKEFILE ON)

# ################### Options ####################
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(OPTIMIZE_FOR_NATIVE "Build with -march=native" OFF)
option(ENABLE_TESTING "Enable Test Builds" OFF)
option(ENABLE_EXEC "Enable Exec Builds" OFF)

# ################### Modules ####################
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_BINARY_DIR}" ${CMAKE_MODULE_PATH})
include(GNUInstallDirs)
include(CompilerWarnings)
include(CompilerOptimizations)

# ################### Packages ####################
find_package(range-v3)

# ################## LIB target ###################
add_library(knapsack INTERFACE)
target_include_directories(
    knapsack INTERFACE $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include> $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/exec>
                       $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
target_link_libraries(knapsack INTERFACE range-v3::range-v3)

# ############### Project Options ################
set_project_optimizations(knapsack)

# #################### TESTS #####################
if(ENABLE_TEST)
    enable_testing()
    message("Building Tests.")
    set_project_warnings(knapsack)
    add_subdirectory(test)
endif()

# ################# EXEC target ##################
if(ENABLE_EXEC)
    message("Building Executables.")
    add_subdirectory(exec)
endif()
