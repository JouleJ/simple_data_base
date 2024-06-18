cmake_minimum_required(VERSION 3.15)

project(simple_data_base)

file(GLOB CORE_CPP_FILES server/core/*.cpp)
add_library(core ${CORE_CPP_FILES})
target_include_directories(core PRIVATE .)

file(GLOB PERSISTENT_CPP_FILES server/persistent/*.cpp)
add_library(persistent ${PERSISTENT_CPP_FILES})
target_link_libraries(persistent core)
target_include_directories(persistent PRIVATE .)

file(GLOB QUERY_CPP_FILES server/query/*.cpp)
add_library(query ${QUERY_CPP_FILES})
target_link_libraries(query core persistent)
target_include_directories(query PRIVATE .)

file(GLOB_RECURSE TEST_CPP_FILES server/test/*.cpp)
add_executable(unittests ${TEST_CPP_FILES})
target_link_libraries(unittests core persistent query)
target_include_directories(unittests PRIVATE .)

file(GLOB_RECURSE DRIVER_CPP_FILES server/driver/*.cpp)
add_executable(server ${DRIVER_CPP_FILES})
target_link_libraries(server core persistent query)
target_include_directories(server PRIVATE .)

add_test(NAME UnitTests COMMAND unittests)

enable_testing()
