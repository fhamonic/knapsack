cmake_minimum_required(VERSION 3.12)

project(Knapstack_Solver VERSION 1.0
    DESCRIPTION "Simple 0-1 Knapstack branch and bound solver"
    LANGUAGES CXX)

# set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR})

#################### Options ####################
SET(CMAKE_CXX_STANDARD 17)
SET(CMAKE_CXX_STANDARD_REQUIRED ON)

include(${CMAKE_CURRENT_SOURCE_DIR}/CMakeBuildTypeUtils.cmake)

option(WARNINGS "" OFF)
option(COMPILE_FOR_NATIVE "" OFF)
option(COMPILE_WITH_LTO "" OFF)

#################### Packages ####################
include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup(TARGETS)

################### EXEC target ####################
add_executable(knapstack_bnb src/knapstack_bnb.cpp)
target_include_directories(knapstack_bnb PUBLIC include)

target_link_libraries(knapstack_bnb CONAN_PKG::tbb)
target_link_libraries(knapstack_bnb CONAN_PKG::boost)
target_link_libraries(knapstack_bnb CONAN_PKG::range-v3)

target_compile_options_if_exists(knapstack_bnb PRIVATE ${WARNINGS} -Wall)
target_compile_options_if_exists(knapstack_bnb PRIVATE ${COMPILE_FOR_NATIVE} -march=native)
target_compile_options_if_exists(knapstack_bnb PRIVATE ${COMPILE_WITH_LTO} -flto)