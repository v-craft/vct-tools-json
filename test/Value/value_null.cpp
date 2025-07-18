#include <vct/test_unit_macros.hpp>

import std;
import vct.test.unit;
import vct.tools.json;

using namespace vct::tools;

M_TEST(Value, Null) {
    // Test default construction (should be null)
    M_ASSERT_NO_THROW( json::Value null_default );
    M_ASSERT_EQ( json::Value{}.type(), json::Type::eNull );
    M_ASSERT_EQ( json::Value().type(), json::Type::eNull );
    
    // Test explicit null construction
    M_ASSERT_NO_THROW( json::Value null_explicit{json::Type::eNull} );
    M_ASSERT_EQ( json::Value{json::Type::eNull}.type(), json::Type::eNull );
    
    // Test nullptr construction
    M_ASSERT_NO_THROW( json::Value null_ptr{nullptr} );
    M_ASSERT_EQ( json::Value{nullptr}.type(), json::Type::eNull );
    
    // Test implicit construction (copy initialization)
    json::Value implicit_null = nullptr;
    M_ASSERT_EQ( implicit_null.type(), json::Type::eNull );
    
    // Test copy initialization with different forms
    json::Value copy_init_null = nullptr;
    json::Value copy_init_default = json::Value{};
    json::Value copy_init_explicit = json::Value{json::Type::eNull};
    
    M_ASSERT_EQ( copy_init_null.type(), json::Type::eNull );
    M_ASSERT_EQ( copy_init_default.type(), json::Type::eNull );
    M_ASSERT_EQ( copy_init_explicit.type(), json::Type::eNull );
    
    M_ASSERT_EQ( copy_init_null.get<std::nullptr_t>(), nullptr );
    M_ASSERT_EQ( copy_init_default.get<std::nullptr_t>(), nullptr );
    M_ASSERT_EQ( copy_init_explicit.get<std::nullptr_t>(), nullptr );
    
    // Test type checking
    json::Value null_val{nullptr};
    json::Value null_val2{};
    
    M_ASSERT_EQ( null_val.type(), json::Type::eNull );
    M_ASSERT_EQ( null_val2.type(), json::Type::eNull );
    
    M_ASSERT_TRUE( null_val.is(json::Type::eNull) );
    M_ASSERT_TRUE( null_val2.is(json::Type::eNull) );
    
    M_ASSERT_FALSE( null_val.is(json::Type::eString) );
    M_ASSERT_FALSE( null_val.is(json::Type::eNumber) );
    M_ASSERT_FALSE( null_val.is(json::Type::eBool) );
    M_ASSERT_FALSE( null_val.is(json::Type::eArray) );
    M_ASSERT_FALSE( null_val.is(json::Type::eObject) );
    
    M_EXPECT_STREQ( null_val.type_name(), "Null" );
    M_EXPECT_STREQ( null_val2.type_name(), "Null" );
    
    // Test get<Null>() method (Null is std::nullptr_t)
    M_ASSERT_NO_THROW( std::ignore = null_val.get<std::nullptr_t>() );
    M_ASSERT_EQ( null_val.get<std::nullptr_t>(), nullptr );
    M_ASSERT_EQ( null_val2.get<std::nullptr_t>(), nullptr );
    
    // Test get_ref<Null>() method
    M_ASSERT_NO_THROW( auto& ref = null_val.get_ref<std::nullptr_t>() );
    M_ASSERT_EQ( null_val.get_ref<std::nullptr_t>(), nullptr );
    
    // Test get_ref with different null values
    json::Value ref_test_null{nullptr};
    json::Value ref_test_default{};
    json::Value ref_test_explicit{json::Type::eNull};
    
    M_ASSERT_NO_THROW( auto& ref_null = ref_test_null.get_ref<std::nullptr_t>() );
    M_ASSERT_NO_THROW( auto& ref_default = ref_test_default.get_ref<std::nullptr_t>() );
    M_ASSERT_NO_THROW( auto& ref_explicit = ref_test_explicit.get_ref<std::nullptr_t>() );
    
    M_ASSERT_EQ( ref_test_null.get_ref<std::nullptr_t>(), nullptr );
    M_ASSERT_EQ( ref_test_default.get_ref<std::nullptr_t>(), nullptr );
    M_ASSERT_EQ( ref_test_explicit.get_ref<std::nullptr_t>(), nullptr );
    
    // Test const get_ref
    const json::Value const_null{nullptr};
    M_ASSERT_NO_THROW( auto& const_ref = const_null.get_ref<std::nullptr_t>() );
    M_ASSERT_EQ( const_null.get_ref<std::nullptr_t>(), nullptr );
    
    // Test get_ref type safety - wrong type should throw
    json::Value wrong_type_val{json::String("not null")};
    M_ASSERT_THROW( std::ignore = wrong_type_val.get_ref<std::nullptr_t>(), std::runtime_error );
    
    json::Value num_val_for_ref{42.0};
    M_ASSERT_THROW( std::ignore = num_val_for_ref.get_ref<std::nullptr_t>(), std::runtime_error );
    
    json::Value bool_val_for_ref{true};
    M_ASSERT_THROW( std::ignore = bool_val_for_ref.get_ref<std::nullptr_t>(), std::runtime_error );
    
    // Test assignment operations
    json::Value assign_val{json::Type::eBool}; // Start with different type
    M_ASSERT_NO_THROW( assign_val = nullptr );
    M_ASSERT_EQ( assign_val.type(), json::Type::eNull );
    M_ASSERT_EQ( assign_val.get<std::nullptr_t>(), nullptr );
    
    // Test implicit assignment (copy assignment)
    json::Value implicit_assign{true}; // Start with different type
    implicit_assign = nullptr;
    M_ASSERT_EQ( implicit_assign.type(), json::Type::eNull );
    M_ASSERT_EQ( implicit_assign.get<std::nullptr_t>(), nullptr );
    
    // Test Value-to-Value comparison
    json::Value null_cmp1{nullptr};
    json::Value null_cmp2{nullptr};
    json::Value null_cmp3{}; // Default construction
    json::Value bool_val{true};
    json::Value num_val{42};
    
    M_ASSERT_TRUE( null_cmp1 == null_cmp2 );
    M_ASSERT_TRUE( null_cmp1 == null_cmp3 );
    M_ASSERT_TRUE( null_cmp2 == null_cmp3 );
    M_ASSERT_FALSE( null_cmp1 == bool_val );
    M_ASSERT_FALSE( null_cmp1 == num_val );
    
    // Test Value-to-nullptr comparison
    json::Value null_compare{nullptr};
    M_ASSERT_TRUE( null_compare == nullptr );
    M_ASSERT_FALSE( bool_val == nullptr );
    M_ASSERT_FALSE( num_val == nullptr );
    
    // Test mixed type comparison
    json::Value str_val{json::String("hello")};
    json::Value array_val{json::Array{}};
    json::Value obj_val{json::Object{}};
    
    M_ASSERT_FALSE( null_cmp1 == str_val );
    M_ASSERT_FALSE( null_cmp1 == array_val );
    M_ASSERT_FALSE( null_cmp1 == obj_val );
    
    // Test serialization
    M_ASSERT_NO_THROW( null_val.serialize() );
    M_ASSERT_NO_THROW( null_val2.serialize() );
    
    // Test specific serialization results
    M_ASSERT_EQ( json::Value{nullptr}.serialize(), "null" );
    M_ASSERT_EQ( json::Value{}.serialize(), "null" );
    
    // Test pretty serialization
    M_ASSERT_NO_THROW( null_val.serialize_pretty() );
    M_ASSERT_EQ( json::Value{nullptr}.serialize_pretty(), "null" );
    M_ASSERT_EQ( json::Value{}.serialize_pretty(), "null" );
    
    // Test parsing from string
    M_ASSERT_NO_THROW( json::parse("null") );
    
    // Test parsing results
    auto parsed_null = json::parse("null");
    if (parsed_null.has_value()) {
        M_ASSERT_EQ( parsed_null->type(), json::Type::eNull );
        M_ASSERT_EQ( parsed_null->get<std::nullptr_t>(), nullptr );
    } else {
        M_ASSERT_FAIL("Failed to parse 'null'");
    }
    
    // Test round-trip serialization/parsing
    json::Value original_null{nullptr};
    auto serialized_null = original_null.serialize();
    auto parsed_back_null = json::parse(serialized_null);
    if (parsed_back_null.has_value()) {
        M_ASSERT_EQ( parsed_back_null->type(), json::Type::eNull );
        M_ASSERT_EQ( parsed_back_null->get<std::nullptr_t>(), nullptr );
    } else {
        M_ASSERT_FAIL("Failed to parse back serialized null");
    }
    
    // Test pointer type conversions (special behavior for null values)
    // According to your note: if Value is nullptr, any pointer type get<> should return nullptr
    json::Value null_for_pointers{nullptr};
    
    // Test various pointer types that should all return nullptr
    M_ASSERT_NO_THROW( std::ignore = null_for_pointers.get<int*>() );
    M_ASSERT_NO_THROW( std::ignore = null_for_pointers.get<char*>() );
    M_ASSERT_NO_THROW( std::ignore = null_for_pointers.get<void*>() );
    M_ASSERT_NO_THROW( std::ignore = null_for_pointers.get<double*>() );
    
    M_ASSERT_EQ( null_for_pointers.get<int*>(), nullptr );
    M_ASSERT_EQ( null_for_pointers.get<char*>(), nullptr );
    M_ASSERT_EQ( null_for_pointers.get<void*>(), nullptr );
    M_ASSERT_EQ( null_for_pointers.get<double*>(), nullptr );
    
    // Test that non-null values cannot be converted to pointer types
    json::Value non_null_val{42};
    M_ASSERT_THROW( std::ignore = non_null_val.get<int*>(), std::runtime_error );
    M_ASSERT_THROW( std::ignore = non_null_val.get<char*>(), std::runtime_error );
    M_ASSERT_THROW( std::ignore = non_null_val.get<void*>(), std::runtime_error );
    
    // Test type safety - wrong type access should throw (except pointer types)
    M_ASSERT_THROW( std::ignore = null_val.get<json::String>(), std::runtime_error );
    M_ASSERT_THROW( std::ignore = null_val.get<json::Number>(), std::runtime_error );
    M_ASSERT_THROW( std::ignore = null_val.get<json::Bool>(), std::runtime_error );
    M_ASSERT_THROW( std::ignore = null_val.get<json::Array>(), std::runtime_error );
    M_ASSERT_THROW( std::ignore = null_val.get<json::Object>(), std::runtime_error );
    
    // Test Null get_ref type safety - only exact type match should work
    M_ASSERT_THROW( std::ignore = null_val.get_ref<json::String>(), std::runtime_error );
    M_ASSERT_THROW( std::ignore = null_val.get_ref<json::Number>(), std::runtime_error );
    M_ASSERT_THROW( std::ignore = null_val.get_ref<json::Bool>(), std::runtime_error );
    M_ASSERT_THROW( std::ignore = null_val.get_ref<json::Array>(), std::runtime_error );
    M_ASSERT_THROW( std::ignore = null_val.get_ref<json::Object>(), std::runtime_error );
    
    // Test Null to non-pointer types should throw
    M_ASSERT_THROW( std::ignore = null_val.get<int>(), std::runtime_error );
    M_ASSERT_THROW( std::ignore = null_val.get<float>(), std::runtime_error );
    M_ASSERT_THROW( std::ignore = null_val.get<double>(), std::runtime_error );
    M_ASSERT_THROW( std::ignore = null_val.get<bool>(), std::runtime_error );
    M_ASSERT_THROW( std::ignore = null_val.get<std::string>(), std::runtime_error );
    
    // Test null consistency
    json::Value consistency_null1{};
    json::Value consistency_null2{nullptr};
    json::Value consistency_null3{json::Type::eNull};
    
    M_ASSERT_TRUE( consistency_null1 == consistency_null2 );
    M_ASSERT_TRUE( consistency_null2 == consistency_null3 );
    M_ASSERT_TRUE( consistency_null1 == consistency_null3 );
    
    M_ASSERT_TRUE( consistency_null1 == nullptr );
    M_ASSERT_TRUE( consistency_null2 == nullptr );
    M_ASSERT_TRUE( consistency_null3 == nullptr );
    
    // Test that all null values serialize to the same string
    M_ASSERT_EQ( consistency_null1.serialize(), "null" );
    M_ASSERT_EQ( consistency_null2.serialize(), "null" );
    M_ASSERT_EQ( consistency_null3.serialize(), "null" );
}

