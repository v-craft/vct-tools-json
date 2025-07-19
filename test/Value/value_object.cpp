#include <vct/test_unit_macros.hpp>

import std;
import vct.test.unit;
import vct.tools.json;


using namespace vct::tools;

// --- Object Construction and Type Checks ---
M_TEST(Value, Object) {
    // Default and explicit construction
    M_ASSERT_NO_THROW(json::Value obj_default{json::Type::eObject});
    M_ASSERT_EQ(json::Value{json::Type::eObject}.type(), json::Type::eObject);
    M_ASSERT_EQ(json::Value{json::Type::eObject}.size(), 0);
    M_ASSERT_TRUE(json::Value{json::Type::eObject}.to<json::Object>().empty());

    M_ASSERT_NO_THROW(json::Value obj_explicit{json::Object{}});
    M_ASSERT_NO_THROW(json::Value obj_init{json::Object{{"key1", 1}, {"key2", 2}}});

    // Direct construction and mixed types
    json::Value obj_direct{json::Object{{"name", "John"}, {"age", 30}, {"active", true}}};
    M_ASSERT_EQ(obj_direct.size(), 3);

    json::Value mixed_obj{json::Object{
        {"number", json::Number(42)},
        {"string", "hello"},
        {"boolean", true},
        {"null_value", nullptr}
    }};
    M_ASSERT_EQ(mixed_obj.size(), 4);

    // Type checking
    json::Value obj_val{json::Object{{"a", 1}, {"b", 2}, {"c", 3}}};
    json::Value empty_obj{json::Object{}};
    M_ASSERT_TRUE(obj_val.is(json::Type::eObject));
    M_ASSERT_FALSE(obj_val.is(json::Type::eString));
    M_EXPECT_STREQ(obj_val.type_name(), "Object");

    // --- Size and Empty Checks ---
    json::Value size_test{json::Object{{"a", 1}, {"b", 2}, {"c", 3}, {"d", 4}, {"e", 5}}};
    M_ASSERT_EQ(size_test.size(), 5);
    json::Value empty_size_test{json::Object{}};
    M_ASSERT_EQ(empty_size_test.size(), 0);
    M_ASSERT_TRUE(empty_size_test.to<json::Object>().empty());

    // --- Key Access and Modification ---
    json::Value subscript_test{json::Object{{"first", 10}, {"second", 20}, {"third", 30}}};
    M_ASSERT_EQ(subscript_test["first"].to<json::Number>(), 10);

    json::Value mixed_subscript{json::Object{
        {"number", 42},
        {"text", "test"},
        {"flag", true},
        {"empty", nullptr}
    }};
    M_ASSERT_EQ(mixed_subscript["text"].to<json::String>(), "test");

    // Modification via subscript
    json::Value modify_test{json::Object{{"a", 1}, {"b", 2}, {"c", 3}}};
    modify_test["a"] = 100;
    modify_test["b"] = "modified";
    modify_test["c"] = false;
    M_ASSERT_EQ(modify_test["b"].to<json::String>(), "modified");

    // Add new key
    json::Value new_key_test{json::Object{{"existing", 1}}};
    new_key_test["new_key"] = "new_value";
    M_ASSERT_EQ(new_key_test.size(), 2);

    // --- Const Key Access ---
    const json::Value const_obj{json::Object{{"x", 1}, {"y", 2}, {"z", 3}}};
    M_ASSERT_EQ(const_obj["y"].to<json::Number>(), 2);

    // --- Safe Access with at() ---
    json::Value at_test{json::Object{{"alpha", 100}, {"beta", 200}, {"gamma", 300}}};
    M_ASSERT_NO_THROW(std::ignore = at_test.at("alpha"));
    M_ASSERT_EQ(at_test.at("gamma").to<json::Number>(), 300);
    M_ASSERT_THROW(std::ignore = at_test.at("nonexistent"), std::out_of_range);

    // --- Reference Access and Modification ---
    json::Value ref_test{json::Object{{"data1", 10}, {"data2", 20}}};
    auto& obj_ref = ref_test.get<json::Object>();
    obj_ref["data1"] = 100;
    obj_ref["new_data"] = "added";
    M_ASSERT_EQ(ref_test.size(), 3);

    // --- Assignment Operations ---
    json::Value assign_val{json::Type::eObject};
    assign_val = json::Object{{"key1", 1}, {"key2", 2}};
    M_ASSERT_EQ(assign_val.size(), 2);

    // --- Nested Object Access ---
    json::Value nested_obj{json::Object{
        {"person", json::Object{{"name", "John"}, {"age", 30}, {"active", true}}},
        {"company", json::Object{{"name", "TechCorp"}, {"employees", 100}, {"public", false}}}
    }};
    M_ASSERT_EQ(nested_obj["person"]["name"].to<json::String>(), "John");
    M_ASSERT_EQ(nested_obj["company"]["employees"].to<json::Number>(), 100);

    // --- Deeply Nested Object Access ---
    json::Value deep_nested{json::Object{
        {"level1", json::Object{
            {"level2", json::Object{
                {"level3", json::Object{
                    {"data1", "deep_value1"},
                    {"data2", "deep_value2"}
                }}
            }}
        }}
    }};
    M_ASSERT_EQ(deep_nested["level1"]["level2"]["level3"]["data2"].to<json::String>(), "deep_value2");

    // --- Value-to-Value Comparison ---
    json::Value obj_cmp1{json::Object{{"a", 1}, {"b", 2}}};
    json::Value obj_cmp2{json::Object{{"a", 1}, {"b", 2}}};
    json::Value obj_cmp3{json::Object{{"a", 1}, {"b", 3}}};
    M_ASSERT_TRUE(obj_cmp1 == obj_cmp2);
    M_ASSERT_FALSE(obj_cmp1 == obj_cmp3);

    // --- Serialization and Parsing ---
    json::Value serial_test{json::Object{{"key1", 1}, {"key2", 2}}};
    M_ASSERT_NO_THROW(std::ignore = serial_test.serialize());
    M_ASSERT_NO_THROW(std::ignore = serial_test.serialize_pretty());

    auto parsed_simple = json::parse(R"({"key1":1,"key2":2})");
    if (parsed_simple.has_value()) {
        M_ASSERT_TRUE(*parsed_simple == serial_test);
    } else {
        M_ASSERT_FAIL("Failed to parse simple object");
    }

    auto parsed_empty = json::parse("{}");
    if (parsed_empty.has_value()) {
        json::Value empty_code{json::Object{}};
        M_ASSERT_TRUE(*parsed_empty == empty_code);
    } else {
        M_ASSERT_FAIL("Failed to parse empty object");
    }

    // Test nested object parsing
    auto parsed_nested = json::parse(R"({"outer":{"inner1":1,"inner2":2}})");
    if (parsed_nested.has_value()) {
        M_ASSERT_EQ( parsed_nested->type(), json::Type::eObject );
        M_ASSERT_EQ( parsed_nested->size(), 1 );
        M_ASSERT_EQ( (*parsed_nested)["outer"].size(), 2 );
        M_ASSERT_EQ( (*parsed_nested)["outer"]["inner1"].to<json::Number>(), 1 );
        M_ASSERT_EQ( (*parsed_nested)["outer"]["inner2"].to<json::Number>(), 2 );
        
        // Compare with code-created nested object
        json::Value nested_code{json::Object{
            {"outer", json::Object{{"inner1", 1}, {"inner2", 2}}}
        }};
        M_ASSERT_TRUE( *parsed_nested == nested_code );
    } else {
        M_ASSERT_FAIL("Failed to parse nested object");
    }
    
    // Test mixed type parsing
    auto parsed_mixed = json::parse(R"({"number":42,"string":"test","boolean":true,"null_val":null})");
    if (parsed_mixed.has_value()) {
        M_ASSERT_EQ( parsed_mixed->type(), json::Type::eObject );
        M_ASSERT_EQ( parsed_mixed->size(), 4 );
        M_ASSERT_EQ( (*parsed_mixed)["number"].to<json::Number>(), 42 );
        M_ASSERT_EQ( (*parsed_mixed)["string"].to<json::String>(), "test" );
        M_ASSERT_EQ( (*parsed_mixed)["boolean"].to<json::Bool>(), true );
        M_ASSERT_EQ( (*parsed_mixed)["null_val"].type(), json::Type::eNull );
        
        // Compare with code-created mixed object
        json::Value mixed_code{json::Object{
            {"number", 42},
            {"string", "test"},
            {"boolean", true},
            {"null_val", nullptr}
        }};
        M_ASSERT_TRUE( *parsed_mixed == mixed_code );
    } else {
        M_ASSERT_FAIL("Failed to parse mixed type object");
    }
    
    // Test round-trip serialization/parsing
    json::Value original_obj{json::Object{
        {"level1", json::Object{
            {"name", "nested"},
            {"value", 123}
        }},
        {"level2", json::Object{
            {"items", json::Object{{"item1", 1}, {"item2", 2}}}
        }}
    }};
    auto serialized_obj = original_obj.serialize();
    auto parsed_back_obj = json::parse(serialized_obj);
    if (parsed_back_obj.has_value()) {
        M_ASSERT_TRUE( *parsed_back_obj == original_obj );
    } else {
        M_ASSERT_FAIL("Failed to parse back serialized object");
    }
    
    // --- Contains Method ---
    json::Value contains_test{json::Object{{"key1", 1}, {"key2", "hello"}}};
    M_ASSERT_TRUE(contains_test.contains("key1"));
    M_ASSERT_FALSE(contains_test.contains("nonexistent"));

    // --- Type Safety ---
    M_ASSERT_THROW(std::ignore = obj_val.to<json::Number>(), std::runtime_error);
    M_ASSERT_THROW(std::ignore = obj_val.get<json::Array>(), std::runtime_error);

    // --- Modification via Reference ---
    json::Value ref_modify_test{json::Object{{"a", 1}, {"b", 2}}};
    auto& ref_a = ref_modify_test["a"];
    ref_a = 100;
    M_ASSERT_EQ(ref_modify_test["a"].to<json::Number>(), 100);
}
