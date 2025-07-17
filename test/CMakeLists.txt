cmake_minimum_required(VERSION 4.0)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_MODULE_STD ON)

message(" ===== CTest was enabled =====")

include(FetchContent)

FetchContent_Declare(
    vct-test-unit
    URL      https://github.com/v-craft/vct-test-unit/archive/refs/tags/v1.0.0.tar.gz
)
FetchContent_MakeAvailable(vct-test-unit)

file(GLOB test_cpp_files ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

add_executable(test_json ${test_cpp_files})
target_link_libraries(test_json PRIVATE ${prev_name}::${lib_name})
target_link_libraries(test_json PRIVATE vct::test-unit)

add_test(NAME vct_tests COMMAND test_json)


