#include <vct/test_unit_macros.hpp>

import std;
import vct.test.unit;
import vct.tools.json;


using namespace vct::tools;

// Test the Type enum values
M_TEST(Type, enum) {
    M_ASSERT_NO_THROW( M_EXPECT_EQ( static_cast<int>(json::Type::eNull),   0 ) );
    M_ASSERT_NO_THROW( M_EXPECT_EQ( static_cast<int>(json::Type::eBool), 1 ) );
    M_ASSERT_NO_THROW( M_EXPECT_EQ( static_cast<int>(json::Type::eNumber),  2 ) );
    M_ASSERT_NO_THROW( M_EXPECT_EQ( static_cast<int>(json::Type::eString), 3 ) );
    M_ASSERT_NO_THROW( M_EXPECT_EQ( static_cast<int>(json::Type::eArray), 4 ) );
    M_ASSERT_NO_THROW( M_EXPECT_EQ( static_cast<int>(json::Type::eObject),   5 ) );
    

    M_EXPECT_NO_THROW( M_EXPECT_EQ( static_cast<int>(json::ParseError::eNone),           0 ) );
    M_EXPECT_NO_THROW( M_EXPECT_EQ( static_cast<int>(json::ParseError::eEmptyData),      1 ) );
    M_EXPECT_NO_THROW( M_EXPECT_EQ( static_cast<int>(json::ParseError::eRedundantText),  2 ) );
    M_EXPECT_NO_THROW( M_EXPECT_EQ( static_cast<int>(json::ParseError::eDepthExceeded),  3 ) );
    M_EXPECT_NO_THROW( M_EXPECT_EQ( static_cast<int>(json::ParseError::eIllegalEscape),  4 ) );
    M_EXPECT_NO_THROW( M_EXPECT_EQ( static_cast<int>(json::ParseError::eInvalidNumber),  5 ) );
    M_EXPECT_NO_THROW( M_EXPECT_EQ( static_cast<int>(json::ParseError::eUnclosedString), 6 ) );
    M_EXPECT_NO_THROW( M_EXPECT_EQ( static_cast<int>(json::ParseError::eUnclosedObject), 7 ) );
    M_EXPECT_NO_THROW( M_EXPECT_EQ( static_cast<int>(json::ParseError::eUnclosedArray),  8 ) );
    M_EXPECT_NO_THROW( M_EXPECT_EQ( static_cast<int>(json::ParseError::eUnknownFormat),  9 ) );
}

// Test the Object type
M_TEST(Type, Object) {

    M_ASSERT_TRUE( (std::is_same_v<json::Object, std::map<json::String, json::Value>>) );
    M_ASSERT_EQ( json::Object{}, (std::map<json::String, json::Value> {} ));
    M_ASSERT_EQ( json::Object(), (std::map<json::String, json::Value> {} ));
}

// Test the Number type
M_TEST(Type, Number) {
    M_ASSERT_TRUE( (std::is_same_v<json::Number, double>) );
    M_ASSERT_EQ( 0.0 , json::Number{} );
    M_ASSERT_EQ( 0.0 , json::Number() );
}

// Test the String type
M_TEST(Type, Array) {
    M_ASSERT_TRUE( (std::is_same_v<json::Array, std::vector<json::Value>>) );
    M_ASSERT_EQ( std::vector<json::Value>{} , json::Array{} );
    M_ASSERT_EQ( std::vector<json::Value>{} , json::Array() );
}

// Test the String type
M_TEST(Type, String) {
    M_ASSERT_TRUE( (std::is_same_v<json::String, std::string>) );
    M_ASSERT_EQ( "" , json::String{} );
    M_ASSERT_EQ( "" , json::String() );
}

// Test the Bool type
M_TEST(Type, Bool) {
    M_ASSERT_TRUE( (std::is_same_v<json::Bool, bool>) );
    M_ASSERT_EQ( false , json::Bool{} );
    M_ASSERT_EQ( false , json::Bool() );
}

// Test the Null type
M_TEST(Type, Null) {
    M_ASSERT_TRUE( (std::is_same_v<json::Null, std::nullptr_t>) );
    M_ASSERT_EQ( nullptr , json::Null{} );
    M_ASSERT_EQ( nullptr , json::Null() );
}

// Test the json_types concept
M_TEST(Type, concept) {
    M_ASSERT_TRUE( requires { requires json::json_type<json::Object>; } );
    M_ASSERT_TRUE( requires { requires json::json_type<json::Array>; }  );
    M_ASSERT_TRUE( requires { requires json::json_type<json::Number>; } );
    M_ASSERT_TRUE( requires { requires json::json_type<json::String>; } );
    M_ASSERT_TRUE( requires { requires json::json_type<json::Bool>; }   );
    M_ASSERT_TRUE( requires { requires json::json_type<json::Null>; }   );
}

M_TEST(Type, value) {
    M_ASSERT_NO_THROW( std::ignore = json::Value{} );
}

