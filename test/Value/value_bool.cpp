#include <vct/test_unit_macros.hpp>

import std;
import vct.test.unit;
import vct.tools.json;


using namespace vct::tools;

M_TEST(Value, Bool) {
    // Test default bool construction
    M_ASSERT_NO_THROW( json::Value bool_default{json::Type::eBool} );
    M_ASSERT_EQ( json::Value{json::Type::eBool}.get<json::Bool>(), false );
    M_ASSERT_EQ( json::Value{json::Type::eBool}.type(), json::Type::eBool );
    
    // Test construction with explicit Bool wrapper
    M_ASSERT_NO_THROW( json::Value bool_true{json::Bool(true)} );
    M_ASSERT_NO_THROW( json::Value bool_false{json::Bool(false)} );
    
    // Test direct bool construction (without json::Bool wrapper)
    M_ASSERT_NO_THROW( json::Value direct_true{true} );
    M_ASSERT_NO_THROW( json::Value direct_false{false} );
    
    // Test implicit construction (copy initialization)
    json::Value implicit_true = true;
    json::Value implicit_false = false;
    M_ASSERT_EQ( implicit_true.type(), json::Type::eBool );
    M_ASSERT_EQ( implicit_false.type(), json::Type::eBool );
    M_ASSERT_EQ( implicit_true.get<json::Bool>(), true );
    M_ASSERT_EQ( implicit_false.get<json::Bool>(), false );
    
    // Test copy initialization with different forms
    json::Value copy_init_true = true;
    json::Value copy_init_false = false;
    json::Value copy_init_wrapper_true = json::Bool(true);
    json::Value copy_init_wrapper_false = json::Bool(false);
    
    M_ASSERT_EQ( copy_init_true.type(), json::Type::eBool );
    M_ASSERT_EQ( copy_init_false.type(), json::Type::eBool );
    M_ASSERT_EQ( copy_init_wrapper_true.type(), json::Type::eBool );
    M_ASSERT_EQ( copy_init_wrapper_false.type(), json::Type::eBool );
    
    M_ASSERT_EQ( copy_init_true.get<json::Bool>(), true );
    M_ASSERT_EQ( copy_init_false.get<json::Bool>(), false );
    M_ASSERT_EQ( copy_init_wrapper_true.get<json::Bool>(), true );
    M_ASSERT_EQ( copy_init_wrapper_false.get<json::Bool>(), false );
    
    // Test type checking
    json::Value true_val{true};
    json::Value false_val{false};
    
    M_ASSERT_EQ( true_val.type(), json::Type::eBool );
    M_ASSERT_EQ( false_val.type(), json::Type::eBool );
    
    M_ASSERT_TRUE( true_val.is(json::Type::eBool) );
    M_ASSERT_TRUE( false_val.is(json::Type::eBool) );
    
    M_ASSERT_FALSE( true_val.is(json::Type::eString) );
    M_ASSERT_FALSE( false_val.is(json::Type::eNumber) );
    M_ASSERT_FALSE( true_val.is(json::Type::eNull) );
    M_ASSERT_FALSE( false_val.is(json::Type::eArray) );
    M_ASSERT_FALSE( true_val.is(json::Type::eObject) );
    
    M_EXPECT_STREQ( true_val.type_name(), "Bool" );
    M_EXPECT_STREQ( false_val.type_name(), "Bool" );
    
    // Test get<T>() method
    M_ASSERT_EQ( true_val.get<json::Bool>(), true );
    M_ASSERT_EQ( false_val.get<json::Bool>(), false );
    
    // Test get_ref<T>() method
    M_ASSERT_NO_THROW( auto& ref = true_val.get_ref<json::Bool>() );
    M_ASSERT_EQ( true_val.get_ref<json::Bool>(), true );
    M_ASSERT_EQ( false_val.get_ref<json::Bool>(), false );
    
    // Test get_ref with different bool values
    json::Value ref_test_true{true};
    json::Value ref_test_false{false};
    
    M_ASSERT_NO_THROW( auto& ref_true = ref_test_true.get_ref<json::Bool>() );
    M_ASSERT_NO_THROW( auto& ref_false = ref_test_false.get_ref<json::Bool>() );
    
    M_ASSERT_EQ( ref_test_true.get_ref<json::Bool>(), true );
    M_ASSERT_EQ( ref_test_false.get_ref<json::Bool>(), false );
    
    // Test const get_ref
    const json::Value const_bool{true};
    M_ASSERT_NO_THROW( auto& const_ref = const_bool.get_ref<json::Bool>() );
    M_ASSERT_EQ( const_bool.get_ref<json::Bool>(), true );
    
    // Test get_ref type safety - wrong type should throw
    json::Value wrong_type_val{json::String("not a bool")};
    M_ASSERT_THROW( std::ignore = wrong_type_val.get_ref<json::Bool>(), std::runtime_error );
    
    json::Value num_val_for_ref{42.0};
    M_ASSERT_THROW( std::ignore = num_val_for_ref.get_ref<json::Bool>(), std::runtime_error );
    
    json::Value null_val_for_ref{nullptr};
    M_ASSERT_THROW( std::ignore = null_val_for_ref.get_ref<json::Bool>(), std::runtime_error );
    
    // Test assignment operations
    json::Value assign_val{json::Type::eBool};
    M_ASSERT_NO_THROW( assign_val = json::Bool(true) );
    M_ASSERT_EQ( assign_val.get<json::Bool>(), true );
    M_ASSERT_EQ( assign_val.type(), json::Type::eBool );
    
    M_ASSERT_NO_THROW( assign_val = json::Bool(false) );
    M_ASSERT_EQ( assign_val.get<json::Bool>(), false );
    
    // Test direct assignment (without json::Bool wrapper)
    json::Value direct_assign{};
    M_ASSERT_NO_THROW( direct_assign = true );
    M_ASSERT_EQ( direct_assign.type(), json::Type::eBool );
    M_ASSERT_EQ( direct_assign.get<json::Bool>(), true );
    
    M_ASSERT_NO_THROW( direct_assign = false );
    M_ASSERT_EQ( direct_assign.get<json::Bool>(), false );
    
    // Test implicit assignment (copy assignment)
    json::Value implicit_assign{};
    implicit_assign = true;
    M_ASSERT_EQ( implicit_assign.type(), json::Type::eBool );
    M_ASSERT_EQ( implicit_assign.get<json::Bool>(), true );
    
    implicit_assign = false;
    M_ASSERT_EQ( implicit_assign.get<json::Bool>(), false );
    
    // Test reference modification
    json::Value mod_val{true};
    auto& bool_ref = mod_val.get_ref<json::Bool>();
    bool_ref = false;
    M_ASSERT_EQ( mod_val.get<json::Bool>(), false );
    
    bool_ref = true;
    M_ASSERT_EQ( mod_val.get<json::Bool>(), true );
    
    // Test Value-to-Value comparison
    json::Value val_true1{true};
    json::Value val_true2{true};
    json::Value val_false1{false};
    json::Value val_false2{false};
    json::Value val_string{json::String("true")};
    
    M_ASSERT_TRUE( val_true1 == val_true2 );
    M_ASSERT_TRUE( val_false1 == val_false2 );
    M_ASSERT_FALSE( val_true1 == val_false1 );
    M_ASSERT_FALSE( val_false1 == val_true1 );
    M_ASSERT_FALSE( val_true1 == val_string ); // Different types
    
    // Test Value-to-bool comparison
    json::Value bool_val_true{true};
    json::Value bool_val_false{false};
    M_ASSERT_TRUE( bool_val_true == true );
    M_ASSERT_TRUE( bool_val_false == false );
    M_ASSERT_FALSE( bool_val_true == false );
    M_ASSERT_FALSE( bool_val_false == true );
    
    // Test mixed type comparison
    json::Value str_val{json::String("hello")};
    json::Value num_val{42.0};
    json::Value null_val{nullptr};
    
    M_ASSERT_FALSE( bool_val_true == str_val );
    M_ASSERT_FALSE( bool_val_true == num_val );
    M_ASSERT_FALSE( bool_val_true == null_val );
    M_ASSERT_TRUE( null_val == nullptr );
    
    // Test serialization
    M_ASSERT_NO_THROW( true_val.serialize() );
    M_ASSERT_NO_THROW( false_val.serialize() );
    
    // Test specific serialization results
    M_ASSERT_EQ( json::Value{true}.serialize(), "true" );
    M_ASSERT_EQ( json::Value{false}.serialize(), "false" );
    
    // Test pretty serialization
    M_ASSERT_NO_THROW( true_val.serialize_pretty() );
    M_ASSERT_NO_THROW( false_val.serialize_pretty() );
    M_ASSERT_EQ( json::Value{true}.serialize_pretty(), "true" );
    M_ASSERT_EQ( json::Value{false}.serialize_pretty(), "false" );
    
    // Test parsing from string
    M_ASSERT_NO_THROW( json::parse("true") );
    M_ASSERT_NO_THROW( json::parse("false") );
    
    // Test parsing results
    auto parsed_true = json::parse("true");
    if (parsed_true.has_value()) {
        M_ASSERT_EQ( parsed_true->type(), json::Type::eBool );
        M_ASSERT_EQ( parsed_true->get<json::Bool>(), true );
    } else {
        M_ASSERT_FAIL("Failed to parse 'true'");
    }
    
    auto parsed_false = json::parse("false");
    if (parsed_false.has_value()) {
        M_ASSERT_EQ( parsed_false->type(), json::Type::eBool );
        M_ASSERT_EQ( parsed_false->get<json::Bool>(), false );
    } else {
        M_ASSERT_FAIL("Failed to parse 'false'");
    }
    
    // Test type safety - wrong type access should throw (except arithmetic conversions)
    M_ASSERT_THROW( std::ignore = true_val.get<json::String>(), std::runtime_error );
    M_ASSERT_THROW( std::ignore = true_val.get<json::Array>(), std::runtime_error );
    M_ASSERT_THROW( std::ignore = false_val.get<json::Object>(), std::runtime_error );
    M_ASSERT_THROW( std::ignore = false_val.get<std::nullptr_t>(), std::runtime_error );
    
    // Test Bool to Number conversion is allowed (arithmetic conversion)
    M_ASSERT_NO_THROW( auto num_from_true = true_val.get<json::Number>() );
    M_ASSERT_NO_THROW( auto num_from_false = false_val.get<json::Number>() );
    M_ASSERT_EQ( true_val.get<json::Number>(), 1.0 );  // true -> 1.0
    M_ASSERT_EQ( false_val.get<json::Number>(), 0.0 ); // false -> 0.0
    
    // Test Bool to other numeric types (should work due to arithmetic conversion)
    M_ASSERT_NO_THROW( auto int_from_true = true_val.get<int>() );
    M_ASSERT_NO_THROW( auto int_from_false = false_val.get<int>() );
    M_ASSERT_EQ( true_val.get<int>(), 1 );   // true -> 1
    M_ASSERT_EQ( false_val.get<int>(), 0 );  // false -> 0
    
    M_ASSERT_NO_THROW( auto float_from_true = true_val.get<float>() );
    M_ASSERT_NO_THROW( auto double_from_false = false_val.get<double>() );
    M_ASSERT_EQ( true_val.get<float>(), 1.0f );  // true -> 1.0f
    M_ASSERT_EQ( false_val.get<double>(), 0.0 ); // false -> 0.0
    
    // Test Bool get_ref type safety - only exact type match should work
    M_ASSERT_THROW( std::ignore = true_val.get_ref<json::Number>(), std::runtime_error );
    M_ASSERT_THROW( std::ignore = false_val.get_ref<json::String>(), std::runtime_error );
    M_ASSERT_THROW( std::ignore = true_val.get_ref<json::Array>(), std::runtime_error );
    M_ASSERT_THROW( std::ignore = false_val.get_ref<json::Object>(), std::runtime_error );
    M_ASSERT_THROW( std::ignore = true_val.get_ref<std::nullptr_t>(), std::runtime_error );
    
    // Test round-trip serialization/parsing
    json::Value original_true{true};
    json::Value original_false{false};
    
    auto serialized_true = original_true.serialize();
    auto parsed_back_true = json::parse(serialized_true);
    if (parsed_back_true.has_value()) {
        M_ASSERT_EQ( parsed_back_true->type(), json::Type::eBool );
        M_ASSERT_EQ( parsed_back_true->get<json::Bool>(), true );
    } else {
        M_ASSERT_FAIL("Failed to parse back serialized true");
    }
    
    auto serialized_false = original_false.serialize();
    auto parsed_back_false = json::parse(serialized_false);
    if (parsed_back_false.has_value()) {
        M_ASSERT_EQ( parsed_back_false->type(), json::Type::eBool );
        M_ASSERT_EQ( parsed_back_false->get<json::Bool>(), false );
    } else {
        M_ASSERT_FAIL("Failed to parse back serialized false");
    }
    
    // Test Bool to Number conversion (allowed as arithmetic conversion)
    // According to C++ standard: false -> 0, true -> 1
    json::Value bool_to_num_true{true};
    json::Value bool_to_num_false{false};
    
    M_ASSERT_NO_THROW( auto num_val = bool_to_num_true.get<json::Number>() );
    M_ASSERT_EQ( bool_to_num_true.get<json::Number>(), 1.0 );  // true -> 1.0
    M_ASSERT_EQ( bool_to_num_false.get<json::Number>(), 0.0 ); // false -> 0.0
    
    // Test boolean logic consistency
    json::Value logical_true{true};
    json::Value logical_false{false};
    
    M_ASSERT_TRUE( logical_true.get<json::Bool>() );
    M_ASSERT_FALSE( logical_false.get<json::Bool>() );
    
    // Test that boolean values are exactly true/false
    M_ASSERT_TRUE( logical_true.get<json::Bool>() == true );
    M_ASSERT_TRUE( logical_false.get<json::Bool>() == false );
    M_ASSERT_FALSE( logical_true.get<json::Bool>() == false );
    M_ASSERT_FALSE( logical_false.get<json::Bool>() == true );
}
