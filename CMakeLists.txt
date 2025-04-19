cmake_minimum_required(VERSION 4.0)

set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD "a9e1cf81-9932-4810-974b-6eccaf14e457")

project(vct-tools-json)

set(CMAKE_CXX_STANDARD 23)

set(CMAKE_CXX_MODULE_STD ON)

add_library(vct-tools-json STATIC)

target_sources(vct-tools-json PUBLIC
        FILE_SET json_modules TYPE CXX_MODULES FILES src/Json.ixx
)


if(VCT_ENABLE_TEST STREQUAL "ON" OR VCT_ENABLE_TEST_TOOL_JSON STREQUAL "ON")
    message(" ===== CTest was enabled =====")

    # ctest -R VctTest -C DEBUG --verbose
    add_executable(TTest test/main.cpp)

    target_sources(TTest PUBLIC
            FILE_SET ttest_json_modules TYPE CXX_MODULES FILES src/Json.ixx test/src/Test.ixx
    )
    target_include_directories(TTest PRIVATE test/src)

    add_test(NAME VctTest COMMAND TTest)
endif ()
