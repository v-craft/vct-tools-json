#include <vct/test_unit_macros.hpp>

import std;
import vct.test.unit;
import vct.tools.json;


using namespace vct::tools;

std::string read_file(const std::string& path) {
    std::ifstream file( CURRENT_PATH "/" + path, std::ios::binary);
    if (!file) {
        throw std::runtime_error("Failed to open file: " + path);
    }
    return { std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>() };
}

M_TEST(File, prettify_senior_1) {
    std::ifstream ifs( CURRENT_PATH "/files/senior_1_plain.json" );
    if (!ifs.is_open()) M_EXPECT_FAIL( "Failed to open file: " + CURRENT_PATH );
    M_ASSERT_TRUE( ifs.is_open() );
    json::Value value = json::parse(ifs).value_or( nullptr );
    M_ASSERT_FALSE( value.is_nul() );
    std::ofstream ofs( CURRENT_PATH "/files/out_senior_1.json" );
    if (!ofs.is_open()) M_EXPECT_FAIL( "Failed to open file: " + CURRENT_PATH "/files/out_senior_1.json" );
    M_ASSERT_TRUE( value.prettify_to(ofs) );
}

M_TEST(File, prettify_senior_2) {
    std::ifstream ifs( CURRENT_PATH "/files/senior_2_plain.json" );
    if (!ifs.is_open()) M_EXPECT_FAIL( "Failed to open file: " + CURRENT_PATH );
    M_ASSERT_TRUE( ifs.is_open() );
    json::Value value = json::parse(ifs).value_or( nullptr );
    M_ASSERT_FALSE( value.is_nul() );
    std::ofstream ofs( CURRENT_PATH "/files/out_senior_2.json" );
    if (!ofs.is_open()) M_EXPECT_FAIL( "Failed to open file: " + CURRENT_PATH "/files/out_senior_2.json" );
    M_ASSERT_TRUE( value.prettify_to(ofs) );
}

M_TEST(File, medium_1) {
    std::ifstream ifs( CURRENT_PATH "/files/medium_1_plain.json" );
    M_ASSERT_TRUE( ifs.is_open() );
    json::Value value = json::parse(ifs).value_or( nullptr );
    M_ASSERT_NE( value.type(), json::Type::eNull );
    std::string pretty_str = read_file("files/medium_1.json");
    json::Value pretty_value = json::parse(pretty_str).value_or( nullptr );
    M_ASSERT_NE( pretty_value.type(), json::Type::eNull );
    M_ASSERT_EQ( value, pretty_value );
}

M_TEST(File, simple_1) {
    std::ifstream ifs( CURRENT_PATH "/files/simple_1_plain.json" );
    M_ASSERT_TRUE( ifs.is_open() );
    json::Value value = json::parse(ifs).value_or( nullptr );
    M_ASSERT_NE( value.type(), json::Type::eNull );
    std::string pretty_str = read_file("files/simple_1.json");
    json::Value pretty_value = json::parse(pretty_str).value_or( nullptr );
    M_ASSERT_NE( pretty_value.type(), json::Type::eNull );
    M_ASSERT_EQ( value, pretty_value );
}

M_TEST(File, simple_2) {
    std::ifstream ifs( CURRENT_PATH "/files/simple_2_plain.json" );
    M_ASSERT_TRUE( ifs.is_open() );
    json::Value value = json::parse(ifs).value_or( nullptr );
    M_ASSERT_NE( value.type(), json::Type::eNull );
    std::string pretty_str = read_file("files/simple_2.json");
    json::Value pretty_value = json::parse(pretty_str).value_or( nullptr );
    M_ASSERT_NE( pretty_value.type(), json::Type::eNull );
    M_ASSERT_EQ( value, pretty_value );
}

M_TEST(File, simple_3) {
    std::ifstream ifs( CURRENT_PATH "/files/simple_3_plain.json" );
    M_ASSERT_TRUE( ifs.is_open() );
    json::Value value = json::parse(ifs).value_or( nullptr );
    M_ASSERT_NE( value.type(), json::Type::eNull );
    std::string pretty_str = read_file("files/simple_3.json");
    json::Value pretty_value = json::parse(pretty_str).value_or( nullptr );
    M_ASSERT_NE( pretty_value.type(), json::Type::eNull );
    M_ASSERT_EQ( value, pretty_value );
}

M_TEST(File, senior_1) {
    std::ifstream ifs( CURRENT_PATH "/files/senior_1_plain.json" );
    M_ASSERT_TRUE( ifs.is_open() );
    json::Value value = json::parse(ifs).value_or( nullptr );
    M_ASSERT_NE( value.type(), json::Type::eNull );
    std::string pretty_str = read_file("files/senior_1.json");

    const auto bein = std::chrono::system_clock::now();
    json::Value pretty_value = json::parse(pretty_str).value_or( nullptr );
    const auto end = std::chrono::system_clock::now();
    const auto time = std::chrono::duration_cast<std::chrono::microseconds>(end - bein).count();
    std::println("--------------------------------------------------------------parse_senior_1 time: {} us", time);

    M_ASSERT_NE( pretty_value.type(), json::Type::eNull );
    M_ASSERT_EQ( value, pretty_value );
}

M_TEST(File, senior_2) {
    std::ifstream ifs( CURRENT_PATH "/files/senior_2_plain.json" );
    M_ASSERT_TRUE( ifs.is_open() );
    json::Value value = json::parse(ifs).value_or( nullptr );
    M_ASSERT_NE( value.type(), json::Type::eNull );
    std::string pretty_str = read_file("files/senior_2.json");

    const auto bein = std::chrono::system_clock::now();
    json::Value pretty_value = json::parse(pretty_str).value_or( nullptr );
    const auto end = std::chrono::system_clock::now();
    const auto time = std::chrono::duration_cast<std::chrono::microseconds>(end - bein).count();
    std::println("-------------------------------------------------------------parse_senior_2 time: {} us", time);

    M_ASSERT_NE( pretty_value.type(), json::Type::eNull );
    M_ASSERT_EQ( value, pretty_value );
}

#ifndef M_VCT_TOOLS_JSON_UNORDERED_MAP


M_TEST(File, ordered_medium_1) {
    std::ifstream ifs( CURRENT_PATH "/files/medium_1.json" );
    M_ASSERT_TRUE( ifs.is_open() );
    json::Value value = json::parse(ifs).value_or( nullptr );
    M_ASSERT_NE( value.type(), json::Type::eNull );
    std::string plain = read_file("files/medium_1_plain.json");
    M_EXPECT_EQ(  value.serialize(), plain );
}

M_TEST(File, ordered_simple_1) {
    std::ifstream ifs( CURRENT_PATH "/files/simple_1.json" );
    M_ASSERT_TRUE( ifs.is_open() );
    json::Value value = json::parse(ifs).value_or( nullptr );
    M_ASSERT_NE( value.type(), json::Type::eNull );
    std::string plain = read_file("files/simple_1_plain.json");
    M_EXPECT_EQ(  value.serialize(), plain );
}

M_TEST(File, ordered_simple_2) {
    std::ifstream ifs( CURRENT_PATH "/files/simple_2.json" );
    M_ASSERT_TRUE( ifs.is_open() );
    json::Value value = json::parse(ifs).value_or( nullptr );
    M_ASSERT_NE( value.type(), json::Type::eNull );
    std::string plain = read_file("files/simple_2_plain.json");
    M_EXPECT_EQ(  value.serialize(), plain );
}

M_TEST(File, ordered_simple_3) {
    std::ifstream ifs( CURRENT_PATH "/files/simple_3.json" );
    M_ASSERT_TRUE( ifs.is_open() );
    json::Value value = json::parse(ifs).value_or( nullptr );
    M_ASSERT_NE( value.type(), json::Type::eNull );
    std::string plain = read_file("files/simple_3_plain.json");
    M_EXPECT_EQ(  value.serialize(), plain );
}

M_TEST(File, ordered_senior_1) {
    std::ifstream ifs( CURRENT_PATH "/files/senior_1.json" );
    M_ASSERT_TRUE( ifs.is_open() );
    json::Value value = json::parse(ifs).value_or( nullptr );
    M_ASSERT_NE( value.type(), json::Type::eNull );
    std::string plain = read_file("files/senior_1_plain.json");

    const auto bein = std::chrono::system_clock::now();
    std::string str = value.serialize();
    const auto end = std::chrono::system_clock::now();
    const auto time = std::chrono::duration_cast<std::chrono::microseconds>(end - bein).count();
    std::println("-------------------------------------------------------------serialize_senior_1 time: {} us", time);

    M_EXPECT_EQ(  str, plain );
}

M_TEST(File, ordered_senior_2) {
    std::ifstream ifs( CURRENT_PATH "/files/senior_2.json" );
    M_ASSERT_TRUE( ifs.is_open() );
    json::Value value = json::parse(ifs).value_or( nullptr );
    M_ASSERT_NE( value.type(), json::Type::eNull );
    std::string plain = read_file("files/senior_2_plain.json");

    const auto bein = std::chrono::system_clock::now();
    std::string str = value.serialize();
    const auto end = std::chrono::system_clock::now();
    const auto time = std::chrono::duration_cast<std::chrono::microseconds>(end - bein).count();
    std::println("-------------------------------------------------------------serialize_senior_2 time: {} us", time);

    M_EXPECT_EQ(  str, plain );
}

#endif // M_VCT_TOOLS_JSON_UNORDERED_MAP


