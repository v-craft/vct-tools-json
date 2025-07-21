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
    M_ASSERT_NO_THROW( std::ignore = json::Value (json::Object()) );
    M_ASSERT_NO_THROW( std::ignore = json::Value (json::Array())  );
    M_ASSERT_NO_THROW( std::ignore = json::Value (json::String()) );
    M_ASSERT_NO_THROW( std::ignore = json::Value (json::Number()) );
    M_ASSERT_NO_THROW( std::ignore = json::Value (json::Bool())   );
    M_ASSERT_NO_THROW( std::ignore = json::Value (json::Null())   );

    M_ASSERT_NO_THROW( std::ignore = json::Value().is_nul() );
    M_ASSERT_NO_THROW( std::ignore = json::Value().is_obj() );
    M_ASSERT_NO_THROW( std::ignore = json::Value().is_arr() );
    M_ASSERT_NO_THROW( std::ignore = json::Value().is_str() );
    M_ASSERT_NO_THROW( std::ignore = json::Value().is_num() );
    M_ASSERT_NO_THROW( std::ignore = json::Value().is_bol() );

    M_ASSERT_EQ(json::Value().type(), json::Type::eNull);
    M_ASSERT_EQ(json::Value(json::Type::eObject).type(), json::Type::eNumber);
    M_ASSERT_EQ(json::Value(json::Type::eArray ).type(), json::Type::eNumber);
    M_ASSERT_EQ(json::Value(json::Type::eString).type(), json::Type::eNumber);
    M_ASSERT_EQ(json::Value(json::Type::eNumber).type(), json::Type::eNumber);
    M_ASSERT_EQ(json::Value(json::Type::eBool  ).type(), json::Type::eNumber);
    M_ASSERT_EQ(json::Value(json::Type::eNull  ).type(), json::Type::eNumber);

    M_EXPECT_STREQ(json::Value{ json::Object{} }.type_name(), "Object");
    M_EXPECT_STREQ(json::Value{ json::Array{}  }.type_name(), "Array");
    M_EXPECT_STREQ(json::Value{ json::String{} }.type_name(), "String");
    M_EXPECT_STREQ(json::Value{ json::Number{} }.type_name(), "Number");
    M_EXPECT_STREQ(json::Value{ json::Bool{}   }.type_name(), "Bool");
    M_EXPECT_STREQ(json::Value{ json::Null{}   }.type_name(), "Null");

    M_ASSERT_TRUE(json::Value().is_nul());
    M_ASSERT_FALSE(json::Value().is_obj());
    M_ASSERT_FALSE(json::Value().is_arr());
    M_ASSERT_FALSE(json::Value().is_str());
    M_ASSERT_FALSE(json::Value().is_num());
    M_ASSERT_FALSE(json::Value().is_bol());

    M_ASSERT_TRUE(json::Value( json::Object{}).is_obj());
    M_ASSERT_FALSE(json::Value(json::Object{}).is_nul());
    M_ASSERT_FALSE(json::Value(json::Object{}).is_arr());
    M_ASSERT_FALSE(json::Value(json::Object{}).is_str());
    M_ASSERT_FALSE(json::Value(json::Object{}).is_num());
    M_ASSERT_FALSE(json::Value(json::Object{}).is_bol());

    M_ASSERT_FALSE(json::Value(json::Array{}).is_nul());
    M_ASSERT_FALSE(json::Value(json::Array{}).is_obj());
    M_ASSERT_TRUE(json::Value( json::Array{}).is_arr());
    M_ASSERT_FALSE(json::Value(json::Array{}).is_str());
    M_ASSERT_FALSE(json::Value(json::Array{}).is_num());
    M_ASSERT_FALSE(json::Value(json::Array{}).is_bol());

    M_ASSERT_FALSE(json::Value(json::String{}).is_nul());
    M_ASSERT_FALSE(json::Value(json::String{}).is_obj());
    M_ASSERT_FALSE(json::Value(json::String{}).is_arr());
    M_ASSERT_TRUE(json::Value( json::String{}).is_str());
    M_ASSERT_FALSE(json::Value(json::String{}).is_num());
    M_ASSERT_FALSE(json::Value(json::String{}).is_bol());


    M_ASSERT_FALSE(json::Value(json::Number{}).is_nul());
    M_ASSERT_FALSE(json::Value(json::Number{}).is_obj());
    M_ASSERT_FALSE(json::Value(json::Number{}).is_arr());
    M_ASSERT_FALSE(json::Value(json::Number{}).is_str());
    M_ASSERT_TRUE(json::Value( json::Number{}).is_num());
    M_ASSERT_FALSE(json::Value(json::Number{}).is_bol());

    M_ASSERT_FALSE(json::Value(json::Bool{}).is_nul());
    M_ASSERT_FALSE(json::Value(json::Bool{}).is_obj());
    M_ASSERT_FALSE(json::Value(json::Bool{}).is_arr());
    M_ASSERT_FALSE(json::Value(json::Bool{}).is_str());
    M_ASSERT_FALSE(json::Value(json::Bool{}).is_num());
    M_ASSERT_TRUE(json::Value( json::Bool{}).is_bol());
}
