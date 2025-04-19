cmake_minimum_required(VERSION 4.0)

set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD "a9e1cf81-9932-4810-974b-6eccaf14e457")

project(json)

set(CMAKE_CXX_STANDARD 23)

set(CMAKE_CXX_MODULE_STD ON)

add_library(json STATIC)

target_sources(json PUBLIC
        FILE_SET json_modules TYPE CXX_MODULES FILES src/Json.ixx
)


#if(VCT_ENABLE_TEST_JSON STREQUAL "ON")
#    enable_testing()
#    message("Msg: enable google test")
#    # ctest -R GoogleTest -C DEBUG --verbose
#
#    add_executable(test test.cpp)
#
#    target_sources(test PUBLIC
#            FILE_SET test_json_modules TYPE CXX_MODULES FILES src/Json.ixx
#    )
#
#
#    add_test(NAME Test COMMAND test)
#
#endif ()
