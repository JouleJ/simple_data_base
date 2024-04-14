cmake_minimum_required(VERSION 3.15)

project(simple_data_base)

file(GLOB CORE_CPP_FILES server/core/*.cpp)
add_library(core ${CORE_CPP_FILES})
target_include_directories(core PRIVATE .)

file(GLOB_RECURSE TEST_CPP_FILES server/test/*.cpp)
add_executable(unittests ${TEST_CPP_FILES})
target_link_libraries(unittests core)
target_include_directories(unittests PRIVATE .)

add_test(NAME UnitTests COMMAND unittests)

enable_testing()
