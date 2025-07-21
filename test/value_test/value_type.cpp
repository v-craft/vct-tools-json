#include <vct/test_unit_macros.hpp>

import std;
import vct.test.unit;
import vct.tools.json;


using namespace vct::tools;


M_TEST(Value, type_constructors) {
    M_ASSERT_NO_THROW( json::Value value );
    M_ASSERT_NO_THROW( std::ignore = json::Value () );
    M_ASSERT_NO_THROW( std::ignore = json::Value {} );
    M_ASSERT_NO_THROW( std::ignore = json::Value (json::Type::eObject) );
    M_ASSERT_NO_THROW( std::ignore = json::Value (json::Type::eArray)  );
    M_ASSERT_NO_THROW( std::ignore = json::Value (json::Type::eString) );
    M_ASSERT_NO_THROW( std::ignore = json::Value (json::Type::eNumber) );
    M_ASSERT_NO_THROW( std::ignore = json::Value (json::Type::eBool)   );
    M_ASSERT_NO_THROW( std::ignore = json::Value (json::Type::eNull)   );

    M_ASSERT_NO_THROW( std::ignore = json::Value().is<json::Null>()   );
    M_ASSERT_NO_THROW( std::ignore = json::Value().is<json::Object>() );
    M_ASSERT_NO_THROW( std::ignore = json::Value().is<json::Array>()  );
    M_ASSERT_NO_THROW( std::ignore = json::Value().is<json::String>() );
    M_ASSERT_NO_THROW( std::ignore = json::Value().is<json::Number>() );
    M_ASSERT_NO_THROW( std::ignore = json::Value().is<json::Bool>()   );

    M_ASSERT_EQ(json::Value().type(), json::Type::eNull);
    M_ASSERT_EQ(json::Value(json::Type::eObject).type(), json::Type::eObject);
    M_ASSERT_EQ(json::Value(json::Type::eArray).type(), json::Type::eArray);
    M_ASSERT_EQ(json::Value(json::Type::eString).type(), json::Type::eString);
    M_ASSERT_EQ(json::Value(json::Type::eNumber).type(), json::Type::eNumber);
    M_ASSERT_EQ(json::Value(json::Type::eBool).type(), json::Type::eBool);
    M_ASSERT_EQ(json::Value(json::Type::eNull).type(), json::Type::eNull);

    M_EXPECT_STREQ(json::Value{json::Type::eObject}.type_name(), "Object");
    M_EXPECT_STREQ(json::Value{json::Type::eArray}.type_name(), "Array");
    M_EXPECT_STREQ(json::Value{json::Type::eString}.type_name(), "String");
    M_EXPECT_STREQ(json::Value{json::Type::eNumber}.type_name(), "Number");
    M_EXPECT_STREQ(json::Value{json::Type::eBool}.type_name(), "Bool");
    M_EXPECT_STREQ(json::Value{json::Type::eNull}.type_name(), "Null");

    M_ASSERT_TRUE(json::Value().is<json::Null>());
    M_ASSERT_FALSE(json::Value().is<json::Object>());
    M_ASSERT_FALSE(json::Value().is<json::Array>());
    M_ASSERT_FALSE(json::Value().is<json::String>());
    M_ASSERT_FALSE(json::Value().is<json::Number>());
    M_ASSERT_FALSE(json::Value().is<json::Bool>());

    M_ASSERT_TRUE(json::Value(json::Type::eObject).is<json::Object>());
    M_ASSERT_FALSE(json::Value(json::Type::eObject).is<json::Null>());
    M_ASSERT_FALSE(json::Value(json::Type::eObject).is<json::Array>());
    M_ASSERT_FALSE(json::Value(json::Type::eObject).is<json::String>());
    M_ASSERT_FALSE(json::Value(json::Type::eObject).is<json::Number>());
    M_ASSERT_FALSE(json::Value(json::Type::eObject).is<json::Bool>());

    M_ASSERT_FALSE(json::Value(json::Type::eArray).is<json::Null>());
    M_ASSERT_FALSE(json::Value(json::Type::eArray).is<json::Object>());
    M_ASSERT_TRUE(json::Value(json::Type::eArray).is<json::Array>());
    M_ASSERT_FALSE(json::Value(json::Type::eArray).is<json::String>());
    M_ASSERT_FALSE(json::Value(json::Type::eArray).is<json::Number>());
    M_ASSERT_FALSE(json::Value(json::Type::eArray).is<json::Bool>());

    M_ASSERT_FALSE(json::Value(json::Type::eString).is<json::Null>());
    M_ASSERT_FALSE(json::Value(json::Type::eString).is<json::Object>());
    M_ASSERT_FALSE(json::Value(json::Type::eString).is<json::Array>());
    M_ASSERT_TRUE(json::Value(json::Type::eString).is<json::String>());
    M_ASSERT_FALSE(json::Value(json::Type::eString).is<json::Number>());
    M_ASSERT_FALSE(json::Value(json::Type::eString).is<json::Bool>());


    M_ASSERT_FALSE(json::Value(json::Type::eNumber).is<json::Null>());
    M_ASSERT_FALSE(json::Value(json::Type::eNumber).is<json::Object>());
    M_ASSERT_FALSE(json::Value(json::Type::eNumber).is<json::Array>());
    M_ASSERT_FALSE(json::Value(json::Type::eNumber).is<json::String>());
    M_ASSERT_TRUE(json::Value(json::Type::eNumber).is<json::Number>());
    M_ASSERT_FALSE(json::Value(json::Type::eNumber).is<json::Bool>());

    M_ASSERT_FALSE(json::Value(json::Type::eBool).is<json::Null>());
    M_ASSERT_FALSE(json::Value(json::Type::eBool).is<json::Object>());
    M_ASSERT_FALSE(json::Value(json::Type::eBool).is<json::Array>());
    M_ASSERT_FALSE(json::Value(json::Type::eBool).is<json::String>());
    M_ASSERT_FALSE(json::Value(json::Type::eBool).is<json::Number>());
    M_ASSERT_TRUE(json::Value(json::Type::eBool).is<json::Bool>());
}
