#include <vct/test_unit_macros.hpp>

import std;
import vct.test.unit;
import vct.tools.json;


using namespace vct::tools;

M_TEST(Value, Number) {
    // Test default number construction
    M_ASSERT_NO_THROW( json::Value num_default{json::Type::eNumber} );
    M_ASSERT_EQ( json::Value{json::Type::eNumber}.get<json::Number>(), 0.0 );
    M_ASSERT_EQ( json::Value{json::Type::eNumber}.type(), json::Type::eNumber );
    
    // Test construction with various integer types
    M_ASSERT_NO_THROW( json::Value int_val{json::Number(42)} );
    M_ASSERT_NO_THROW( json::Value long_val{json::Number(1234567890L)} );
    M_ASSERT_NO_THROW( json::Value short_val{json::Number(static_cast<short>(123))} );
    M_ASSERT_NO_THROW( json::Value uint_val{json::Number(42u)} );
    M_ASSERT_NO_THROW( json::Value char_val{json::Number(static_cast<char>('A'))} );
    M_ASSERT_NO_THROW( json::Value schar_val{json::Number(static_cast<signed char>(65))} );
    M_ASSERT_NO_THROW( json::Value uchar_val{json::Number(static_cast<unsigned char>(255))} );
    
    // Test construction with floating point types
    M_ASSERT_NO_THROW( json::Value float_val{json::Number(3.14f)} );
    M_ASSERT_NO_THROW( json::Value double_val{json::Number(3.141592653589793)} );
    M_ASSERT_NO_THROW( json::Value ldouble_val{json::Number(static_cast<long double>(2.718281828))} );
    
    // Test construction with special values
    M_ASSERT_NO_THROW( json::Value zero_val{json::Number(0.0)} );
    M_ASSERT_NO_THROW( json::Value neg_val{json::Number(-42.5)} );
    M_ASSERT_NO_THROW( json::Value large_val{json::Number(1e308)} );
    M_ASSERT_NO_THROW( json::Value small_val{json::Number(1e-307)} );
    
    // Test type checking
    json::Value int_val{json::Number(42)};
    json::Value float_val{json::Number(3.14)};
    json::Value neg_val{json::Number(-123.456)};
    json::Value zero_val{json::Number(0.0)};
    
    M_ASSERT_EQ( int_val.type(), json::Type::eNumber );
    M_ASSERT_EQ( float_val.type(), json::Type::eNumber );
    M_ASSERT_EQ( neg_val.type(), json::Type::eNumber );
    M_ASSERT_EQ( zero_val.type(), json::Type::eNumber );
    
    M_ASSERT_TRUE( int_val.is(json::Type::eNumber) );
    M_ASSERT_TRUE( float_val.is(json::Type::eNumber) );
    M_ASSERT_TRUE( neg_val.is(json::Type::eNumber) );
    M_ASSERT_TRUE( zero_val.is(json::Type::eNumber) );
    
    M_ASSERT_FALSE( int_val.is(json::Type::eString) );
    M_ASSERT_FALSE( float_val.is(json::Type::eBool) );
    M_ASSERT_FALSE( neg_val.is(json::Type::eNull) );
    M_ASSERT_FALSE( zero_val.is(json::Type::eArray) );
    
    M_EXPECT_STREQ( int_val.type_name(), "Number" );
    M_EXPECT_STREQ( float_val.type_name(), "Number" );
    M_EXPECT_STREQ( neg_val.type_name(), "Number" );
    M_EXPECT_STREQ( zero_val.type_name(), "Number" );
    
    // Test get<T>() method
    M_ASSERT_EQ( int_val.get<json::Number>(), 42.0 );
    M_ASSERT_EQ( float_val.get<json::Number>(), 3.14 );
    M_ASSERT_EQ( neg_val.get<json::Number>(), -123.456 );
    M_ASSERT_EQ( zero_val.get<json::Number>(), 0.0 );
    
    // Test get_ref<T>() method
    M_ASSERT_NO_THROW( auto& ref = int_val.get_ref<json::Number>() );
    M_ASSERT_EQ( int_val.get_ref<json::Number>(), 42.0 );
    M_ASSERT_EQ( float_val.get_ref<json::Number>(), 3.14 );
    
    // Test get_ref with different numeric types
    json::Value ref_test_int{123};
    json::Value ref_test_float{456.789f};
    json::Value ref_test_double{987.654321};
    
    M_ASSERT_NO_THROW( auto& ref_int = ref_test_int.get_ref<json::Number>() );
    M_ASSERT_NO_THROW( auto& ref_float = ref_test_float.get_ref<json::Number>() );
    M_ASSERT_NO_THROW( auto& ref_double = ref_test_double.get_ref<json::Number>() );
    
    M_ASSERT_EQ( ref_test_int.get_ref<json::Number>(), 123.0 );
    M_ASSERT_EQ( ref_test_float.get_ref<json::Number>(), 456.789f );
    M_ASSERT_EQ( ref_test_double.get_ref<json::Number>(), 987.654321 );
    
    // Test const get_ref
    const json::Value const_num{42.5};
    M_ASSERT_NO_THROW( auto& const_ref = const_num.get_ref<json::Number>() );
    M_ASSERT_EQ( const_num.get_ref<json::Number>(), 42.5 );
    
    // Test get_ref type safety - wrong type should throw
    json::Value wrong_type_val{json::String("not a number")};
    M_ASSERT_THROW( std::ignore = wrong_type_val.get_ref<json::Number>(), std::runtime_error );
    
    json::Value bool_val_for_ref{true};
    M_ASSERT_THROW( std::ignore = bool_val_for_ref.get_ref<json::Number>(), std::runtime_error );
    
    json::Value null_val_for_ref{nullptr};
    M_ASSERT_THROW( std::ignore = null_val_for_ref.get_ref<json::Number>(), std::runtime_error );
    
    // Test assignment operations
    json::Value assign_val{json::Type::eNumber};
    M_ASSERT_NO_THROW( assign_val = json::Number(99.5) );
    M_ASSERT_EQ( assign_val.get<json::Number>(), 99.5 );
    M_ASSERT_EQ( assign_val.type(), json::Type::eNumber );
    
    // Test assignment with different numeric types
    M_ASSERT_NO_THROW( assign_val = json::Number(100) );
    M_ASSERT_EQ( assign_val.get<json::Number>(), 100.0 );
    
    M_ASSERT_NO_THROW( assign_val = json::Number(-50.25f) );
    M_ASSERT_EQ( assign_val.get<json::Number>(), -50.25 );
    
    // Test reference modification
    json::Value mod_val{json::Number(10.0)};
    auto& num_ref = mod_val.get_ref<json::Number>();
    num_ref = 20.0;
    M_ASSERT_EQ( mod_val.get<json::Number>(), 20.0 );
    
    // Test edge cases and precision
    json::Value precise_val{json::Number(0.1 + 0.2)};
    M_ASSERT_TRUE( std::abs(precise_val.get<json::Number>() - 0.3) < 1e-15 );
    
    // Test very large numbers
    json::Value large_pos{json::Number(1.7976931348623157e+308)};
    json::Value large_neg{json::Number(-1.7976931348623157e+308)};
    M_ASSERT_NO_THROW( std::ignore = large_pos.get<json::Number>() );
    M_ASSERT_NO_THROW( std::ignore = large_neg.get<json::Number>() );
    
    // Test very small numbers
    json::Value small_pos{json::Number(2.2250738585072014e-308)};
    json::Value small_neg{json::Number(-2.2250738585072014e-308)};
    M_ASSERT_NO_THROW( std::ignore = small_pos.get<json::Number>() );
    M_ASSERT_NO_THROW( std::ignore = small_neg.get<json::Number>() );
    
    // Test serialization
    M_ASSERT_NO_THROW( int_val.serialize() );
    M_ASSERT_NO_THROW( float_val.serialize() );
    M_ASSERT_NO_THROW( neg_val.serialize() );
    M_ASSERT_NO_THROW( zero_val.serialize() );
    
    // Test specific serialization results
    M_ASSERT_EQ( json::Value{json::Number(42.0)}.serialize(), "42" );
    M_ASSERT_EQ( json::Value{json::Number(0.0)}.serialize(), "0" );
    M_ASSERT_EQ( json::Value{json::Number(-1.0)}.serialize(), "-1" );
    
    // Test pretty serialization
    M_ASSERT_NO_THROW( int_val.serialize_pretty() );
    M_ASSERT_NO_THROW( float_val.serialize_pretty() );
    M_ASSERT_EQ( json::Value{json::Number(123.0)}.serialize_pretty(), "123" );
    
    // Test parsing from string
    M_ASSERT_NO_THROW( json::parse("42") );
    M_ASSERT_NO_THROW( json::parse("3.14") );
    M_ASSERT_NO_THROW( json::parse("-123.456") );
    M_ASSERT_NO_THROW( json::parse("0") );
    M_ASSERT_NO_THROW( json::parse("1e10") );
    M_ASSERT_NO_THROW( json::parse("1.5e-5") );
    
    // Test parsing results
    auto parsed_int = json::parse("42");
    if (parsed_int.has_value()) {
        M_ASSERT_EQ( parsed_int->type(), json::Type::eNumber );
        M_ASSERT_EQ( parsed_int->get<json::Number>(), 42.0 );
    } else {
        M_EXPECT_FAIL( "Failed to parse valid integer '42'" );
    }
    
    auto parsed_float = json::parse("3.14159");
    if (parsed_float.has_value()) {
        M_ASSERT_EQ( parsed_float->type(), json::Type::eNumber );
        M_ASSERT_EQ( parsed_float->get<json::Number>(), 3.14159 );
    } else {
        M_EXPECT_FAIL( "Failed to parse valid float '3.14159'" ); 
    }
    
    auto parsed_neg = json::parse("-999.001");
    if (parsed_neg.has_value()) {
        M_ASSERT_EQ( parsed_neg->type(), json::Type::eNumber );
        M_ASSERT_EQ( parsed_neg->get<json::Number>(), -999.001 );
    } else {
        M_EXPECT_FAIL( "Failed to parse valid negative number '-999.001'" );
    }
    
    auto parsed_exp = json::parse("1e5");
    if (parsed_exp.has_value()) {
        M_ASSERT_EQ( parsed_exp->type(), json::Type::eNumber );
        M_ASSERT_EQ( parsed_exp->get<json::Number>(), 100000.0 );
    } else {
        M_EXPECT_FAIL( "Failed to parse valid exponential number '1e5'" );
    }
    
    auto parsed_exp_neg = json::parse("2.5e-3");
    if (parsed_exp_neg.has_value()) {
        M_ASSERT_EQ( parsed_exp_neg->type(), json::Type::eNumber );
        M_ASSERT_EQ( parsed_exp_neg->get<json::Number>(), 0.0025 );
    } else {
        M_EXPECT_FAIL( "Failed to parse valid negative exponential number '2.5e-3'" );
    }
    
    // Test type safety - Number to Bool conversion is allowed (arithmetic conversion)
    // According to C++ standard: 0.0 -> false, any non-zero -> true
    json::Value zero_num{0.0};
    json::Value nonzero_num{3.14};
    json::Value negative_num{-1.5};
    json::Value neg_zero_num{-0.0}; // -0.0 == 0.0 in IEEE 754
    
    M_ASSERT_NO_THROW( auto bool_val = zero_num.get<json::Bool>() );
    M_ASSERT_FALSE( zero_num.get<json::Bool>() );  // 0.0 -> false
    M_ASSERT_TRUE( nonzero_num.get<json::Bool>() ); // 3.14 -> true
    M_ASSERT_TRUE( negative_num.get<json::Bool>() ); // -1.5 -> true
    M_ASSERT_FALSE( neg_zero_num.get<json::Bool>() ); // -0.0 -> false
    
    // Test other invalid type conversions that should throw
    M_ASSERT_THROW( std::ignore = int_val.get<json::String>(), std::runtime_error );
    M_ASSERT_THROW( std::ignore = neg_val.get<json::Array>(), std::runtime_error );
    M_ASSERT_THROW( std::ignore = zero_val.get<json::Object>(), std::runtime_error );
    M_ASSERT_THROW( std::ignore = float_val.get<std::nullptr_t>(), std::runtime_error );
    
    // Test Number get_ref type safety - only exact JSON type match should work
    M_ASSERT_THROW( std::ignore = int_val.get_ref<json::Bool>(), std::runtime_error );
    M_ASSERT_THROW( std::ignore = float_val.get_ref<json::String>(), std::runtime_error );
    M_ASSERT_THROW( std::ignore = neg_val.get_ref<json::Array>(), std::runtime_error );
    M_ASSERT_THROW( std::ignore = zero_val.get_ref<json::Object>(), std::runtime_error );
    M_ASSERT_THROW( std::ignore = int_val.get_ref<std::nullptr_t>(), std::runtime_error );
    
    // Test Number to various numeric types conversion (should work due to arithmetic conversion)
    json::Value num_convert_test{42.7};
    M_ASSERT_NO_THROW( auto int_conv = num_convert_test.get<int>() );
    M_ASSERT_NO_THROW( auto float_conv = num_convert_test.get<float>() );
    M_ASSERT_NO_THROW( auto double_conv = num_convert_test.get<double>() );
    M_ASSERT_NO_THROW( auto long_conv = num_convert_test.get<long>() );
    
    M_ASSERT_EQ( num_convert_test.get<int>(), 43 );      // nearest integer
    M_ASSERT_EQ( num_convert_test.get<float>(), 42.7f ); // conversion
    M_ASSERT_EQ( num_convert_test.get<double>(), 42.7 ); // exact match
    M_ASSERT_EQ( num_convert_test.get<long>(), 43L );    // nearest integer
    
    // Test round-trip serialization/parsing
    json::Value original{json::Number(123.456789)};
    auto serialized = original.serialize();
    auto parsed_back = json::parse(serialized);
    if (parsed_back.has_value()) {
        M_ASSERT_EQ( parsed_back->type(), json::Type::eNumber );
        M_ASSERT_TRUE( std::abs(parsed_back->get<json::Number>() - 123.456789) < 1e-10 );
    } else {
        M_ASSERT_FAIL("Failed to parse back serialized number");
    }

    // Test direct numeric construction (without json::Number wrapper)
    M_ASSERT_NO_THROW( json::Value int_direct{42} );
    M_ASSERT_NO_THROW( json::Value long_direct{1234567890L} );
    M_ASSERT_NO_THROW( json::Value longlong_direct{123456789012345LL} ); // 15 digits max
    M_ASSERT_NO_THROW( json::Value uint_direct{4294967295u} );
    M_ASSERT_NO_THROW( json::Value ulong_direct{123456789012345UL} ); // 15 digits max
    M_ASSERT_NO_THROW( json::Value float_direct{3.14f} );
    M_ASSERT_NO_THROW( json::Value double_direct{2.718281828459045} );
    
    // Test implicit construction (copy initialization)
    json::Value implicit_int = 42;
    json::Value implicit_long = 1234567890L;
    json::Value implicit_longlong = 123456789012345LL;
    json::Value implicit_uint = 4294967295u;
    json::Value implicit_ulong = 123456789012345UL;
    json::Value implicit_float = 3.14f;
    json::Value implicit_double = 2.718281828459045;
    
    M_ASSERT_EQ( implicit_int.type(), json::Type::eNumber );
    M_ASSERT_EQ( implicit_long.type(), json::Type::eNumber );
    M_ASSERT_EQ( implicit_longlong.type(), json::Type::eNumber );
    M_ASSERT_EQ( implicit_uint.type(), json::Type::eNumber );
    M_ASSERT_EQ( implicit_ulong.type(), json::Type::eNumber );
    M_ASSERT_EQ( implicit_float.type(), json::Type::eNumber );
    M_ASSERT_EQ( implicit_double.type(), json::Type::eNumber );
    
    M_ASSERT_EQ( implicit_int.get<json::Number>(), 42.0 );
    M_ASSERT_EQ( implicit_long.get<json::Number>(), 1234567890.0 );
    M_ASSERT_EQ( implicit_longlong.get<json::Number>(), 123456789012345.0 );
    M_ASSERT_EQ( implicit_uint.get<json::Number>(), 4294967295.0 );
    M_ASSERT_EQ( implicit_ulong.get<json::Number>(), 123456789012345.0 );
    M_ASSERT_EQ( implicit_float.get<json::Number>(), 3.14f );
    M_ASSERT_EQ( implicit_double.get<json::Number>(), 2.718281828459045 );
    
    // Test copy initialization with different forms
    json::Value copy_init_int = 99;
    json::Value copy_init_float = 1.23f;
    json::Value copy_init_double = 4.56789;
    json::Value copy_init_wrapper = json::Number(777.888);
    
    M_ASSERT_EQ( copy_init_int.type(), json::Type::eNumber );
    M_ASSERT_EQ( copy_init_float.type(), json::Type::eNumber );
    M_ASSERT_EQ( copy_init_double.type(), json::Type::eNumber );
    M_ASSERT_EQ( copy_init_wrapper.type(), json::Type::eNumber );
    
    M_ASSERT_EQ( copy_init_int.get<json::Number>(), 99.0 );
    M_ASSERT_EQ( copy_init_float.get<json::Number>(), 1.23f );
    M_ASSERT_EQ( copy_init_double.get<json::Number>(), 4.56789 );
    M_ASSERT_EQ( copy_init_wrapper.get<json::Number>(), 777.888 );
    
    // Test direct numeric assignment (without json::Number wrapper)
    json::Value numeric_assign{};
    M_ASSERT_NO_THROW( numeric_assign = 42 );
    M_ASSERT_EQ( numeric_assign.type(), json::Type::eNumber );
    M_ASSERT_EQ( numeric_assign.get<json::Number>(), 42.0 );
    
    M_ASSERT_NO_THROW( numeric_assign = 1234567890L );
    M_ASSERT_EQ( numeric_assign.get<json::Number>(), 1234567890.0 );
    
    M_ASSERT_NO_THROW( numeric_assign = 3.14159f );
    M_ASSERT_EQ( numeric_assign.get<json::Number>(), 3.14159f );
    
    M_ASSERT_NO_THROW( numeric_assign = 2.718281828459045 );
    M_ASSERT_EQ( numeric_assign.get<json::Number>(), 2.718281828459045 );
    
    // Test implicit numeric assignment (copy assignment)
    json::Value implicit_numeric_assign{};
    implicit_numeric_assign = 99;
    M_ASSERT_EQ( implicit_numeric_assign.type(), json::Type::eNumber );
    M_ASSERT_EQ( implicit_numeric_assign.get<json::Number>(), 99.0 );
    
    implicit_numeric_assign = 123.456f;
    M_ASSERT_EQ( implicit_numeric_assign.get<json::Number>(), 123.456f );
    
    implicit_numeric_assign = 987.654321;
    M_ASSERT_EQ( implicit_numeric_assign.get<json::Number>(), 987.654321 );
    
    // Test large integer types (use EXPECT for >15 digits due to double precision limits)
    json::Value large_int{123456789012345LL}; // 15 digits - safe with ASSERT
    M_ASSERT_EQ( large_int.type(), json::Type::eNumber );
    M_ASSERT_EQ( large_int.get<json::Number>(), 123456789012345.0 );
    
    json::Value very_large_int{1234567890123456789LL}; // 19 digits - use EXPECT
    M_ASSERT_EQ( very_large_int.type(), json::Type::eNumber );
    M_EXPECT_DOUBLE_EQ_DEFAULT( very_large_int.get<json::Number>(), 1234567890123456789.0 );
    
    json::Value large_uint{123456789012345UL}; // 15 digits - safe with ASSERT
    M_ASSERT_EQ( large_uint.type(), json::Type::eNumber );
    M_ASSERT_EQ( large_uint.get<json::Number>(), 123456789012345.0 );
    
    json::Value very_large_uint{1234567890123456789UL}; // 19 digits - use EXPECT
    M_ASSERT_EQ( very_large_uint.type(), json::Type::eNumber );
    M_EXPECT_DOUBLE_EQ_DEFAULT( very_large_uint.get<json::Number>(), 1234567890123456789.0 );
    
    // Test Value-to-Value comparison
    json::Value val1{42.0};
    json::Value val2{42.0};
    json::Value val3{43.0};
    json::Value val4{json::String("42")};
    
    M_ASSERT_TRUE( val1 == val2 );
    M_ASSERT_FALSE( val1 == val3 );
    M_ASSERT_FALSE( val1 == val4 ); // Different types
    
    // Test Value-to-numeric comparison
    json::Value num_val{42.0};
    M_ASSERT_TRUE( num_val == 42.0 );
    M_ASSERT_TRUE( num_val == 42 );
    M_ASSERT_TRUE( num_val == 42.0f );
    M_ASSERT_FALSE( num_val == 43.0 );
    M_ASSERT_FALSE( num_val == 41 );
    
    // Test mixed type comparison
    json::Value str_val{json::String("hello")};
    json::Value bool_val{true};
    json::Value null_val{nullptr};
    
    M_ASSERT_FALSE( num_val == str_val );
    M_ASSERT_FALSE( num_val == bool_val );
    M_ASSERT_FALSE( num_val == null_val );
    M_ASSERT_TRUE( null_val == nullptr );
    M_ASSERT_TRUE( bool_val == true );
    M_ASSERT_FALSE( bool_val == false );
    
    // Test precision comparisons using EXPECT for high precision
    json::Value precise1{0.1 + 0.2};
    json::Value precise2{0.3};
    M_EXPECT_TRUE( std::abs(precise1.get<json::Number>() - 0.3) < 1e-15 );
    M_EXPECT_FALSE( precise1 == precise2 ); // Exact equality may fail due to floating-point precision
    
    // Test using DEFAULT comparison macros for floating point (percentage-based)
    json::Value float1{3.14159265358979323846};
    json::Value float2{3.14159265358979323847}; // Tiny difference
    M_EXPECT_DOUBLE_EQ_DEFAULT( float1.get<json::Number>(), 3.14159265358979323846 );
    // Use the actual float value that results from double->float conversion
    M_EXPECT_FLOAT_EQ_DEFAULT( static_cast<float>(float1.get<json::Number>()), static_cast<float>(3.14159265358979323846) );
    
    // Test special numeric values
    json::Value zero_pos{0.0};
    json::Value zero_neg{-0.0};
    M_ASSERT_TRUE( zero_pos == zero_neg ); // -0.0 == 0.0 in IEEE 754
    
    // Test numeric edge cases
    json::Value max_int{std::numeric_limits<int>::max()};
    json::Value min_int{std::numeric_limits<int>::min()};
    json::Value max_double{std::numeric_limits<double>::max()};
    json::Value min_double{std::numeric_limits<double>::lowest()};
    
    M_ASSERT_EQ( max_int.type(), json::Type::eNumber );
    M_ASSERT_EQ( min_int.type(), json::Type::eNumber );
    M_ASSERT_EQ( max_double.type(), json::Type::eNumber );
    M_ASSERT_EQ( min_double.type(), json::Type::eNumber );
    
    // Test serialization of large numbers
    M_ASSERT_NO_THROW( max_int.serialize() );
    M_ASSERT_NO_THROW( min_int.serialize() );
    M_ASSERT_NO_THROW( max_double.serialize() );
    M_ASSERT_NO_THROW( min_double.serialize() );
    
    // Test round-trip with large numbers
    auto large_serialized = max_int.serialize();
    auto large_parsed = json::parse(large_serialized);
    if (large_parsed.has_value()) {
        M_ASSERT_EQ( large_parsed->type(), json::Type::eNumber );
        M_EXPECT_DOUBLE_EQ_DEFAULT( large_parsed->get<json::Number>(), 
                                   static_cast<double>(std::numeric_limits<int>::max()) );
    } else {
        M_ASSERT_FAIL("Failed to parse back serialized large number");
    }
    
    // Test integer precision preservation
    json::Value big_int{123456789012345LL}; // 15 digits - safe for exact comparison
    auto big_serialized = big_int.serialize();
    auto big_parsed = json::parse(big_serialized);
    if (big_parsed.has_value()) {
        M_ASSERT_EQ( big_parsed->type(), json::Type::eNumber );
        M_ASSERT_EQ( big_parsed->get<json::Number>(), 123456789012345.0 );
    } else {
        M_ASSERT_FAIL("Failed to parse back serialized big integer");
    }
    
    // Test very large integer precision (>15 digits - use EXPECT)
    json::Value very_big_int{1234567890123456789LL}; // 19 digits - may lose precision
    auto very_big_serialized = very_big_int.serialize();
    auto very_big_parsed = json::parse(very_big_serialized);
    if (very_big_parsed.has_value()) {
        M_ASSERT_EQ( very_big_parsed->type(), json::Type::eNumber );
        // Use EXPECT for high precision comparison due to double precision limits
        M_EXPECT_DOUBLE_EQ_DEFAULT( very_big_parsed->get<json::Number>(), 1234567890123456789.0 );
    } else {
        M_ASSERT_FAIL("Failed to parse back serialized very big integer");
    }

    // Test scientific notation parsing
    auto sci_pos = json::parse("1.23e10");
    if (sci_pos.has_value()) {
        M_ASSERT_EQ( sci_pos->type(), json::Type::eNumber );
        M_ASSERT_EQ( sci_pos->get<json::Number>(), 1.23e10 );
    } else {
        M_ASSERT_FAIL("Failed to parse scientific notation '1.23e10'");
    }
    
    auto sci_neg = json::parse("5.67e-8");
    if (sci_neg.has_value()) {
        M_ASSERT_EQ( sci_neg->type(), json::Type::eNumber );
        M_ASSERT_EQ( sci_neg->get<json::Number>(), 5.67e-8 );
    } else {
        M_ASSERT_FAIL("Failed to parse scientific notation '5.67e-8'");
    }
    
    // Test boundary parsing
    auto zero_parse = json::parse("0.0");
    if (zero_parse.has_value()) {
        M_ASSERT_EQ( zero_parse->type(), json::Type::eNumber );
        M_ASSERT_EQ( zero_parse->get<json::Number>(), 0.0 );
    } else {
        M_ASSERT_FAIL("Failed to parse '0.0'");
    }
    
    // Test negative zero
    auto neg_zero_parse = json::parse("-0.0");
    if (neg_zero_parse.has_value()) {
        M_ASSERT_EQ( neg_zero_parse->type(), json::Type::eNumber );
        M_ASSERT_EQ( neg_zero_parse->get<json::Number>(), -0.0 );
    } else {
        M_ASSERT_FAIL("Failed to parse '-0.0'");
    }
    
    // Test various integer constructions
    json::Value char_val{'A'};        // char -> int conversion
    json::Value uchar_val{static_cast<unsigned char>(255)};
    json::Value short_val{static_cast<short>(-32768)};
    json::Value ushort_val{static_cast<unsigned short>(65535)};
    
    M_ASSERT_EQ( char_val.type(), json::Type::eNumber );
    M_ASSERT_EQ( uchar_val.type(), json::Type::eNumber );
    M_ASSERT_EQ( short_val.type(), json::Type::eNumber );
    M_ASSERT_EQ( ushort_val.type(), json::Type::eNumber );
    
    M_ASSERT_EQ( char_val.get<json::Number>(), 65.0 );  // 'A' = 65
    M_ASSERT_EQ( uchar_val.get<json::Number>(), 255.0 );
    M_ASSERT_EQ( short_val.get<json::Number>(), -32768.0 );
    M_ASSERT_EQ( ushort_val.get<json::Number>(), 65535.0 );
    
    // Test direct comparison with different numeric types
    json::Value int_42{42};
    M_ASSERT_TRUE( int_42 == 42 );
    M_ASSERT_TRUE( int_42 == 42.0 );
    M_ASSERT_TRUE( int_42 == 42.0f );
    M_ASSERT_TRUE( int_42 == 42L );
    M_ASSERT_TRUE( int_42 == 42LL );
    M_ASSERT_TRUE( int_42 == 42u );
    M_ASSERT_TRUE( int_42 == 42ul );
    M_ASSERT_TRUE( int_42 == static_cast<short>(42) );
    M_ASSERT_TRUE( int_42 == static_cast<char>(42) );
    
    // Test inequality comparisons
    M_ASSERT_FALSE( int_42 == 41 );
    M_ASSERT_FALSE( int_42 == 43 );
    M_ASSERT_FALSE( int_42 == 42.1 );
    M_ASSERT_FALSE( int_42 == 41.9 );
    
    // Test precision limits
    json::Value precision_test{0.123456789012345};
    M_EXPECT_DOUBLE_EQ_DEFAULT( precision_test.get<json::Number>(), 0.123456789012345 );
    
    // Test that exact equality may fail for computed values
    json::Value computed{0.1 + 0.2};
    json::Value literal{0.3};
    M_EXPECT_FALSE( computed == literal ); // Exact equality likely fails
    M_EXPECT_TRUE( std::abs(computed.get<json::Number>() - 0.3) < 1e-15 ); // But close enough
    
    // Test serialization consistency
    json::Value serialize_test{123.456789};
    auto serialized_str = serialize_test.serialize();
    auto parsed_back_test = json::parse(serialized_str);
    if (parsed_back_test.has_value()) {
        M_EXPECT_DOUBLE_EQ_DEFAULT( serialize_test.get<json::Number>(), 
                                   parsed_back_test->get<json::Number>() );
    } else {
        M_ASSERT_FAIL("Failed to parse back serialized number for consistency test");
    }

    // M_EXPECT/ASSERT_DOUBLE_EQ_DEFAULT(val1, val2)
    // M_EXPECT/ASSERT_FLOAT_EQ_DEFAULT(val1, val2)
    // M_EXPECT/ASSERT_FLOAT_EQ(val1, val2, dv)

}


