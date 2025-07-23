#include <vct/test_unit_macros.hpp>

import std;
import vct.test.unit;
import vct.tools.json;


using namespace vct::tools;

// --- Construction tests ---
M_TEST(Value, Number) {
    // Default construction
    M_ASSERT_NO_THROW(json::Value num_default{json::Type::eNumber});
    M_ASSERT_NO_THROW(json::Value num_default{ json::Number{} });
    M_ASSERT_EQ(json::Value{json::Number{}}.to<json::Number>(), 0.0);

    // Construction with various numeric types
    M_ASSERT_NO_THROW(json::Value int_val{42});
    M_ASSERT_NO_THROW(json::Value float_val{3.14f});
    M_ASSERT_NO_THROW(json::Value double_val{2.718281828459045});
    M_ASSERT_NO_THROW(json::Value long_val{1234567890L});
    M_ASSERT_NO_THROW(json::Value ulong_val{123456789012345UL});
    M_ASSERT_NO_THROW(json::Value neg_val{-42.5});
    M_ASSERT_NO_THROW(json::Value zero_val{0.0});
    M_ASSERT_NO_THROW(json::Value large_val{1e308});
    M_ASSERT_NO_THROW(json::Value small_val{1e-307});

    // --- Type checking ---
    json::Value v_int{42}, v_float{3.14f}, v_neg{-123.456}, v_zero{0.0};
    M_ASSERT_EQ(v_int.type(), json::Type::eNumber);
    M_ASSERT_TRUE(v_int.is_num());
    M_ASSERT_FALSE(v_int.is_str());
    M_EXPECT_STREQ(v_int.type_name(), "Number");

    // --- Reference access ---
    M_ASSERT_EQ(v_int.num(), 42.0);
    M_ASSERT_EQ(v_float.num(), 3.14f);
    M_ASSERT_EQ(v_neg.num(), -123.456);
    M_ASSERT_EQ(v_zero.num(), 0.0);

    // Const reference access
    const json::Value const_num{42.5};
    M_ASSERT_EQ(const_num.num(), 42.5);

    // Type safety: wrong type throws
    M_ASSERT_THROW(std::ignore = json::Value{"not a number"}.num(), std::bad_variant_access);
    M_ASSERT_THROW(std::ignore = json::Value{true}.num(), std::bad_variant_access);
    M_ASSERT_THROW(std::ignore = json::Value{nullptr}.num(), std::bad_variant_access);

    // --- Assignment tests ---
    json::Value assign_val{json::Number{}};
    assign_val = 99.5;
    M_ASSERT_EQ(assign_val.to<json::Number>(), 99.5);
    assign_val = 100;
    M_ASSERT_EQ(assign_val.to<json::Number>(), 100.0);
    assign_val = -50.25f;
    M_ASSERT_EQ(assign_val.to<json::Number>(), -50.25);

    // Reference modification
    json::Value mod_val{10.0};
    auto& num_ref = mod_val.num();
    num_ref = 20.0;
    M_ASSERT_EQ(mod_val.to<json::Number>(), 20.0);

    // --- Edge cases and precision ---
    json::Value precise_val{0.1 + 0.2};
    M_ASSERT_TRUE(std::abs(precise_val.to<json::Number>() - 0.3) < 1e-15);

    // Large and small numbers
    M_ASSERT_NO_THROW(std::ignore = json::Value{1.7976931348623157e+308}.to<json::Number>());
    M_ASSERT_NO_THROW(std::ignore = json::Value{-1.7976931348623157e+308}.to<json::Number>());
    M_ASSERT_NO_THROW(std::ignore = json::Value{2.2250738585072014e-308}.to<json::Number>());
    M_ASSERT_NO_THROW(std::ignore = json::Value{-2.2250738585072014e-308}.to<json::Number>());

    // --- Serialization tests ---
    M_ASSERT_EQ(json::Value{42.0}.dump(), "42");
    M_ASSERT_EQ(json::Value{0.0}.dump(), "0");
    M_ASSERT_EQ(json::Value{-1.0}.dump(), "-1");
    M_ASSERT_EQ(json::Value{123.0}.dumpf(), "123");

    // --- Parsing tests ---
    auto parsed_int = json::parse("42");
    M_ASSERT_TRUE(parsed_int.has_value() && parsed_int->num() == 42.0);

    auto parsed_float = json::parse("3.14159");
    M_ASSERT_TRUE(parsed_float.has_value() && parsed_float->num() == 3.14159);

    auto parsed_neg = json::parse("-999.001");
    M_ASSERT_TRUE(parsed_neg.has_value() && parsed_neg->num() == -999.001);

    auto parsed_exp = json::parse("1e5");
    M_ASSERT_TRUE(parsed_exp.has_value() && parsed_exp->num() == 100000.0);

    auto parsed_exp_neg = json::parse("2.5e-3");
    M_ASSERT_TRUE(parsed_exp_neg.has_value() && parsed_exp_neg->num() == 0.0025);

    // --- Type safety for conversions ---
    M_ASSERT_THROW(std::ignore = v_int.to<json::String>(), std::runtime_error);
    M_ASSERT_THROW(std::ignore = v_neg.to<json::Array>(), std::runtime_error);
    M_ASSERT_THROW(std::ignore = v_zero.to<json::Object>(), std::runtime_error);
    M_ASSERT_THROW(std::ignore = v_float.to<std::nullptr_t>(), std::runtime_error);

    // --- Numeric conversions ---
    json::Value num_convert_test{42.7};
    M_ASSERT_EQ(num_convert_test.to<int>(), 43);
    M_ASSERT_EQ(num_convert_test.to<float>(), 42.7f);
    M_ASSERT_EQ(num_convert_test.to<double>(), 42.7);
    M_ASSERT_EQ(num_convert_test.to<long>(), 43L);

    // --- Round-trip serialization/parsing ---
    json::Value original{123.456789};
    auto serialized = original.dump();
    auto parsed_back = json::parse(serialized);
    M_ASSERT_TRUE(parsed_back.has_value() && std::abs(parsed_back->num() - 123.456789) < 1e-10);

    // --- Direct numeric construction and assignment ---
    M_ASSERT_NO_THROW(json::Value int_direct{42});
    M_ASSERT_NO_THROW(json::Value float_direct{3.14f});
    json::Value implicit_int = 42;
    M_ASSERT_EQ(implicit_int.type(), json::Type::eNumber);
    M_ASSERT_EQ(implicit_int.to<json::Number>(), 42.0);

    // --- Value-to-Value and Value-to-numeric comparison ---
    json::Value val1{42.0}, val2{42.0}, val3{43.0}, val4{json::String("42")};
    M_ASSERT_TRUE(val1 == val2);
    M_ASSERT_FALSE(val1 == val3);
    M_ASSERT_FALSE(val1 == val4);

    json::Value num_val{42.0};
    M_ASSERT_TRUE(num_val == 42.0);
    M_ASSERT_TRUE(num_val == 42);
    M_ASSERT_FALSE(num_val == 43.0);

    // --- Mixed type comparison ---
    json::Value str_val{json::String("hello")};
    json::Value bool_val{true};
    json::Value null_val{nullptr};
    M_ASSERT_FALSE(num_val == str_val);
    M_ASSERT_FALSE(num_val == bool_val);
    M_ASSERT_FALSE(num_val == null_val);
    M_ASSERT_TRUE(null_val == nullptr);

    // --- Precision comparisons ---
    json::Value precise1{0.1 + 0.2}, precise2{0.3};
    M_EXPECT_TRUE(std::abs(precise1.to<json::Number>() - 0.3) < 1e-15);
    M_EXPECT_FALSE(precise1 == precise2);

    // --- Special numeric values ---
    json::Value zero_pos{0.0}, zero_neg{-0.0};
    M_ASSERT_TRUE(zero_pos == zero_neg);

    // --- Numeric edge cases ---
    json::Value max_int{std::numeric_limits<int>::max()};
    json::Value min_int{std::numeric_limits<int>::min()};
    M_ASSERT_EQ(max_int.type(), json::Type::eNumber);
    M_ASSERT_EQ(min_int.type(), json::Type::eNumber);

    // --- Serialization of large numbers ---
    M_ASSERT_NO_THROW(std::ignore = max_int.dump());
    M_ASSERT_NO_THROW(std::ignore = min_int.dump());

    // --- Round-trip with large numbers ---
    auto large_serialized = max_int.dump();
    auto large_parsed = json::parse(large_serialized);
    M_ASSERT_TRUE(large_parsed.has_value() && large_parsed->num() == static_cast<double>(std::numeric_limits<int>::max()));

    // --- Integer precision preservation ---
    json::Value big_int{123456789012345LL};
    auto big_serialized = big_int.dump();
    auto big_parsed = json::parse(big_serialized);
    M_ASSERT_TRUE(big_parsed.has_value() && big_parsed->num() == 123456789012345.0);

    // --- Scientific notation parsing ---
    auto sci_pos = json::parse("1.23e10");
    M_ASSERT_TRUE(sci_pos.has_value() && sci_pos->num() == 1.23e10);

    auto sci_neg = json::parse("5.67e-8");
    M_ASSERT_TRUE(sci_neg.has_value() && sci_neg->num() == 5.67e-8);

    // --- Boundary parsing ---
    auto zero_parse = json::parse("0.0");
    M_ASSERT_TRUE(zero_parse.has_value() && zero_parse->num() == 0.0);

    auto neg_zero_parse = json::parse("-0.0");
    M_ASSERT_TRUE(neg_zero_parse.has_value() && neg_zero_parse->num() == -0.0);

    // --- Various integer constructions ---
    json::Value char_val{'A'};
    json::Value uchar_val{static_cast<unsigned char>(255)};
    json::Value short_val{static_cast<short>(-32768)};
    json::Value ushort_val{static_cast<unsigned short>(65535)};
    M_ASSERT_EQ(char_val.to<json::Number>(), 65.0);
    M_ASSERT_EQ(uchar_val.to<json::Number>(), 255.0);
    M_ASSERT_EQ(short_val.to<json::Number>(), -32768.0);
    M_ASSERT_EQ(ushort_val.to<json::Number>(), 65535.0);

    // --- Direct comparison with different numeric types ---
    json::Value int_42{42};
    M_ASSERT_TRUE(int_42 == 42);
    M_ASSERT_TRUE(int_42 == 42.0);
    M_ASSERT_TRUE(int_42 == 42.0f);
    M_ASSERT_FALSE(int_42 == 41);

    // --- Precision limits ---
    json::Value precision_test{0.123456789012345};
    M_EXPECT_DOUBLE_EQ_DEFAULT(precision_test.to<json::Number>(), 0.123456789012345);

    // --- Serialization consistency ---
    json::Value serialize_test{123.456789};
    auto serialized_str = serialize_test.dump();
    auto parsed_back_test = json::parse(serialized_str);
    M_ASSERT_TRUE(parsed_back_test.has_value() && std::abs(serialize_test.to<json::Number>() - parsed_back_test->num()) < 1e-10);
}
