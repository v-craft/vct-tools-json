#include <vct/test_unit_macros.hpp>

import std;
import vct.test.unit;
import vct.tools.json;

using namespace vct::tools;

// --- Construction tests ---
M_TEST(Value, Null) {
    // Default construction
    M_ASSERT_NO_THROW(json::Value null_default);
    M_ASSERT_EQ(json::Value{}.type(), json::Type::eNull);

    // Explicit null construction
    M_ASSERT_NO_THROW(json::Value null_explicit{json::Type::eNull});
    M_ASSERT_EQ(json::Value{json::Type::eNull}.type(), json::Type::eNull);

    // nullptr construction
    M_ASSERT_NO_THROW(json::Value null_ptr{nullptr});
    M_ASSERT_EQ(json::Value{nullptr}.type(), json::Type::eNull);

    // Implicit construction
    json::Value implicit_null = nullptr;
    M_ASSERT_EQ(implicit_null.type(), json::Type::eNull);

    // --- Type checking ---
    json::Value null_val{nullptr};
    M_ASSERT_TRUE(null_val.is<json::Null>());
    M_ASSERT_FALSE(null_val.is<json::String>());
    M_EXPECT_STREQ(null_val.type_name(), "Null");

    // --- Reference access ---
    M_ASSERT_NO_THROW(std::ignore = null_val.get<std::nullptr_t>());
    M_ASSERT_EQ(null_val.get<std::nullptr_t>(), nullptr);

    // Const reference access
    const json::Value const_null{nullptr};
    M_ASSERT_EQ(const_null.get<std::nullptr_t>(), nullptr);

    // Type safety: wrong type throws
    M_ASSERT_THROW(std::ignore = json::Value{"not null"}.get<std::nullptr_t>(), std::runtime_error);
    M_ASSERT_THROW(std::ignore = json::Value{42.0}.get<std::nullptr_t>(), std::runtime_error);

    // --- Assignment tests ---
    json::Value assign_val{json::Type::eBool};
    assign_val = nullptr;
    M_ASSERT_EQ(assign_val.type(), json::Type::eNull);

    // --- Comparison tests ---
    json::Value null_cmp1{nullptr}, null_cmp2{nullptr}, null_cmp3{};
    json::Value bool_val{true}, num_val{42};
    M_ASSERT_TRUE(null_cmp1 == null_cmp2);
    M_ASSERT_TRUE(null_cmp1 == null_cmp3);
    M_ASSERT_FALSE(null_cmp1 == bool_val);
    M_ASSERT_FALSE(null_cmp1 == num_val);

    // Value-to-nullptr comparison
    M_ASSERT_TRUE(null_cmp1 == nullptr);
    M_ASSERT_FALSE(bool_val == nullptr);

    // Mixed type comparison
    M_ASSERT_FALSE(null_cmp1 == json::Value{"hello"});
    M_ASSERT_FALSE(null_cmp1 == json::Value{json::Array{}});
    M_ASSERT_FALSE(null_cmp1 == json::Value{json::Object{}});

    // --- Serialization tests ---
    M_ASSERT_EQ(json::Value{nullptr}.serialize(), "null");
    M_ASSERT_EQ(json::Value{}.serialize(), "null");
    M_ASSERT_EQ(json::Value{nullptr}.serialize_pretty(), "null");

    // --- Parsing tests ---
    auto parsed_null = json::parse("null");
    if (parsed_null.has_value()) {
        M_ASSERT_EQ(parsed_null->type(), json::Type::eNull);
        M_ASSERT_EQ(parsed_null->get<std::nullptr_t>(), nullptr);
    } else {
        M_ASSERT_FAIL("Failed to parse 'null'");
    }

    // Round-trip serialization/parsing
    auto serialized_null = json::Value{nullptr}.serialize();
    auto parsed_back_null = json::parse(serialized_null);
    if (parsed_back_null.has_value()) {
        M_ASSERT_EQ(parsed_back_null->type(), json::Type::eNull);
        M_ASSERT_EQ(parsed_back_null->get<std::nullptr_t>(), nullptr);
    } else {
        M_ASSERT_FAIL("Failed to parse back serialized null");
    }

    // --- Pointer conversion tests ---
    json::Value null_for_pointers{nullptr};
    M_ASSERT_EQ(null_for_pointers.to<int*>(), nullptr);
    M_ASSERT_EQ(null_for_pointers.to<char*>(), nullptr);
    M_ASSERT_EQ(null_for_pointers.to<void*>(), nullptr);
    M_ASSERT_EQ(null_for_pointers.to<double*>(), nullptr);

    // Non-null values cannot be converted to pointer types
    M_ASSERT_THROW(std::ignore = json::Value{42}.to<int*>(), std::runtime_error);

    // --- Type safety for conversions ---
    M_ASSERT_THROW(std::ignore = null_val.to<json::String>(), std::runtime_error);
    M_ASSERT_THROW(std::ignore = null_val.to<json::Number>(), std::runtime_error);
    M_ASSERT_THROW(std::ignore = null_val.to<json::Bool>(), std::runtime_error);
    M_ASSERT_THROW(std::ignore = null_val.to<json::Array>(), std::runtime_error);
    M_ASSERT_THROW(std::ignore = null_val.to<json::Object>(), std::runtime_error);

    // Null to non-pointer types should throw
    M_ASSERT_THROW(std::ignore = null_val.to<int>(), std::runtime_error);
    M_ASSERT_THROW(std::ignore = null_val.to<float>(), std::runtime_error);
    M_ASSERT_THROW(std::ignore = null_val.to<double>(), std::runtime_error);
    M_ASSERT_THROW(std::ignore = null_val.to<bool>(), std::runtime_error);
    M_ASSERT_THROW(std::ignore = null_val.to<std::string>(), std::runtime_error);

    // --- Null consistency ---
    json::Value consistency_null1{}, consistency_null2{nullptr}, consistency_null3{json::Type::eNull};
    M_ASSERT_TRUE(consistency_null1 == consistency_null2);
    M_ASSERT_TRUE(consistency_null2 == consistency_null3);
    M_ASSERT_TRUE(consistency_null1 == consistency_null3);
    M_ASSERT_TRUE(consistency_null1 == nullptr);

    // All null values serialize to the same string
    M_ASSERT_EQ(consistency_null1.serialize(), "null");
    M_ASSERT_EQ(consistency_null2.serialize(), "null");
    M_ASSERT_EQ(consistency_null3.serialize(), "null");
}
