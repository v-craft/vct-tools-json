#include <vct/test_unit_macros.hpp>

import std;
import vct.test.unit;
import vct.tools.json;


using namespace vct::tools;

M_TEST(Value, Bool) {
    // --- Construction tests ---
    // Default bool construction
    M_ASSERT_NO_THROW(json::Value bool_default{json::Type::eBool});
    M_ASSERT_EQ(json::Value{json::Type::eBool}.to<json::Bool>(), false);
    M_ASSERT_EQ(json::Value{json::Type::eBool}.type(), json::Type::eBool);

    // Explicit construction with json::Bool
    M_ASSERT_NO_THROW(json::Value bool_true{json::Bool(true)});
    M_ASSERT_NO_THROW(json::Value bool_false{json::Bool(false)});

    // Direct bool construction
    M_ASSERT_NO_THROW(json::Value direct_true{true});
    M_ASSERT_NO_THROW(json::Value direct_false{false});

    // Implicit construction
    json::Value implicit_true = true;
    json::Value implicit_false = false;
    M_ASSERT_EQ(implicit_true.type(), json::Type::eBool);
    M_ASSERT_EQ(implicit_false.type(), json::Type::eBool);
    M_ASSERT_EQ(implicit_true.to<json::Bool>(), true);
    M_ASSERT_EQ(implicit_false.to<json::Bool>(), false);

    // --- Type checking ---
    json::Value true_val{true};
    json::Value false_val{false};
    M_ASSERT_TRUE(true_val.is<json::Bool>());
    M_ASSERT_TRUE(false_val.is<json::Bool>());
    M_ASSERT_FALSE(true_val.is<json::String>());
    M_ASSERT_FALSE(false_val.is<json::Number>());
    M_EXPECT_STREQ(true_val.type_name(), "Bool");
    M_EXPECT_STREQ(false_val.type_name(), "Bool");

    // --- Reference access ---
    M_ASSERT_EQ(true_val.get<json::Bool>(), true);
    M_ASSERT_EQ(false_val.get<json::Bool>(), false);
    const json::Value const_bool{true};
    M_ASSERT_EQ(const_bool.get<json::Bool>(), true);
    M_ASSERT_THROW(std::ignore = json::Value{json::String("not a bool")}.get<json::Bool>(), std::runtime_error);

    // --- Assignment tests ---
    json::Value assign_val{json::Type::eBool};
    assign_val = json::Bool(true);
    M_ASSERT_EQ(assign_val.to<json::Bool>(), true);
    assign_val = false;
    M_ASSERT_EQ(assign_val.to<json::Bool>(), false);

    // Direct assignment
    json::Value direct_assign{};
    direct_assign = true;
    M_ASSERT_EQ(direct_assign.type(), json::Type::eBool);
    M_ASSERT_EQ(direct_assign.to<json::Bool>(), true);
    direct_assign = false;
    M_ASSERT_EQ(direct_assign.to<json::Bool>(), false);

    // Reference modification
    json::Value mod_val{true};
    auto& bool_ref = mod_val.get<json::Bool>();
    bool_ref = false;
    M_ASSERT_EQ(mod_val.to<json::Bool>(), false);
    bool_ref = true;
    M_ASSERT_EQ(mod_val.to<json::Bool>(), true);

    // --- Comparison tests ---
    json::Value val_true1{true}, val_true2{true};
    json::Value val_false1{false}, val_false2{false};
    json::Value val_string{json::String("true")};
    M_ASSERT_TRUE(val_true1 == val_true2);
    M_ASSERT_TRUE(val_false1 == val_false2);
    M_ASSERT_FALSE(val_true1 == val_false1);
    M_ASSERT_FALSE(val_true1 == val_string);

    // Value-to-bool comparison
    M_ASSERT_TRUE(json::Value{true} == true);
    M_ASSERT_TRUE(json::Value{false} == false);
    M_ASSERT_FALSE(json::Value{true} == false);

    // Mixed type comparison
    M_ASSERT_FALSE(true_val == json::Value{json::String("hello")});
    M_ASSERT_FALSE(true_val == json::Value{42.0});
    M_ASSERT_FALSE(true_val == json::Value{nullptr});
    M_ASSERT_TRUE(json::Value{nullptr} == nullptr);

    // --- Serialization tests ---
    M_ASSERT_EQ(json::Value{true}.serialize(), "true");
    M_ASSERT_EQ(json::Value{false}.serialize(), "false");
    M_ASSERT_EQ(json::Value{true}.serialize_pretty(), "true");
    M_ASSERT_EQ(json::Value{false}.serialize_pretty(), "false");

    // --- Parsing tests ---
    auto parsed_true = json::parse("true");
    M_ASSERT_TRUE(parsed_true.has_value());
    M_ASSERT_EQ(parsed_true->type(), json::Type::eBool);
    M_ASSERT_EQ(parsed_true->get<json::Bool>(), true);

    auto parsed_false = json::parse("false");
    M_ASSERT_TRUE(parsed_false.has_value());
    M_ASSERT_EQ(parsed_false->type(), json::Type::eBool);
    M_ASSERT_EQ(parsed_false->get<json::Bool>(), false);

    // --- Type safety for conversions ---
    M_ASSERT_THROW(std::ignore = true_val.to<json::String>(), std::runtime_error);
    M_ASSERT_THROW(std::ignore = true_val.to<json::Array>(), std::runtime_error);
    M_ASSERT_THROW(std::ignore = false_val.to<json::Object>(), std::runtime_error);
    M_ASSERT_THROW(std::ignore = false_val.to<std::nullptr_t>(), std::runtime_error);

    // --- Arithmetic conversions ---
    M_ASSERT_EQ(true_val.to<json::Number>(), 1.0);
    M_ASSERT_EQ(false_val.to<json::Number>(), 0.0);
    M_ASSERT_EQ(true_val.to<int>(), 1);
    M_ASSERT_EQ(false_val.to<int>(), 0);
    M_ASSERT_EQ(true_val.to<float>(), 1.0f);
    M_ASSERT_EQ(false_val.to<double>(), 0.0);

    // --- get<T> type safety ---
    M_ASSERT_THROW(std::ignore = true_val.get<json::Number>(), std::runtime_error);
    M_ASSERT_THROW(std::ignore = false_val.get<json::String>(), std::runtime_error);
    M_ASSERT_THROW(std::ignore = true_val.get<json::Array>(), std::runtime_error);
    M_ASSERT_THROW(std::ignore = false_val.get<json::Object>(), std::runtime_error);
    M_ASSERT_THROW(std::ignore = true_val.get<std::nullptr_t>(), std::runtime_error);

    // --- Round-trip serialization/parsing ---
    auto serialized_true = json::Value{true}.serialize();
    auto parsed_back_true = json::parse(serialized_true);
    M_ASSERT_TRUE(parsed_back_true.has_value());
    M_ASSERT_EQ(parsed_back_true->type(), json::Type::eBool);
    M_ASSERT_EQ(parsed_back_true->get<json::Bool>(), true);

    auto serialized_false = json::Value{false}.serialize();
    auto parsed_back_false = json::parse(serialized_false);
    M_ASSERT_TRUE(parsed_back_false.has_value());
    M_ASSERT_EQ(parsed_back_false->type(), json::Type::eBool);
    M_ASSERT_EQ(parsed_back_false->get<json::Bool>(), false);

    // --- Boolean logic consistency ---
    M_ASSERT_TRUE(json::Value{true}.to<json::Bool>());
    M_ASSERT_FALSE(json::Value{false}.to<json::Bool>());
    M_ASSERT_TRUE(json::Value{true}.to<json::Bool>() == true);
    M_ASSERT_TRUE(json::Value{false}.to<json::Bool>() == false);
    M_ASSERT_FALSE(json::Value{true}.to<json::Bool>() == false);
    M_ASSERT_FALSE(json::Value{false}.to<json::Bool>() == true);
}
