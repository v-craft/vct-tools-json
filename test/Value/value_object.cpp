#include <vct/test_unit_macros.hpp>

import std;
import vct.test.unit;
import vct.tools.json;


using namespace vct::tools;

M_TEST(Value, Object) {
    // Test default object construction
    M_ASSERT_NO_THROW( json::Value obj_default{json::Type::eObject} );
    M_ASSERT_EQ( json::Value{json::Type::eObject}.type(), json::Type::eObject );
    M_ASSERT_EQ( json::Value{json::Type::eObject}.size(), 0 );
    M_ASSERT_TRUE( json::Value{json::Type::eObject}.to<json::Object>().empty() );
    
    // Test construction with explicit Object wrapper
    M_ASSERT_NO_THROW( json::Value obj_explicit{json::Object{}} );
    M_ASSERT_NO_THROW( json::Value obj_init{json::Object{{"key1", 1}, {"key2", 2}}} );
    
    // Test direct object construction
    json::Value obj_direct{json::Object{{"name", "John"}, {"age", 30}, {"active", true}}};
    M_ASSERT_EQ( obj_direct.type(), json::Type::eObject );
    M_ASSERT_EQ( obj_direct.size(), 3 );
    M_ASSERT_FALSE( obj_direct.to<json::Object>().empty() );
    
    // Test object with mixed types
    json::Value mixed_obj{json::Object{
        {"number", json::Number(42)},  // explicit conversion example
        {"string", "hello"},           // implicit string conversion
        {"boolean", true},             // implicit bool conversion
        {"null_value", nullptr}        // implicit null conversion
    }};
    M_ASSERT_EQ( mixed_obj.type(), json::Type::eObject );
    M_ASSERT_EQ( mixed_obj.size(), 4 );
    
    // Test type checking
    json::Value obj_val{json::Object{{"a", 1}, {"b", 2}, {"c", 3}}};
    json::Value empty_obj{json::Object{}};
    
    M_ASSERT_EQ( obj_val.type(), json::Type::eObject );
    M_ASSERT_EQ( empty_obj.type(), json::Type::eObject );
    
    M_ASSERT_TRUE( obj_val.is(json::Type::eObject) );
    M_ASSERT_TRUE( empty_obj.is(json::Type::eObject) );
    
    M_ASSERT_FALSE( obj_val.is(json::Type::eString) );
    M_ASSERT_FALSE( obj_val.is(json::Type::eNumber) );
    M_ASSERT_FALSE( obj_val.is(json::Type::eBool) );
    M_ASSERT_FALSE( obj_val.is(json::Type::eNull) );
    M_ASSERT_FALSE( obj_val.is(json::Type::eArray) );
    
    M_EXPECT_STREQ( obj_val.type_name(), "Object" );
    M_EXPECT_STREQ( empty_obj.type_name(), "Object" );
    
    // Test size() function
    json::Value size_test{json::Object{{"a", 1}, {"b", 2}, {"c", 3}, {"d", 4}, {"e", 5}}};
    M_ASSERT_EQ( size_test.size(), 5 );
    M_ASSERT_EQ( size_test.to<json::Object>().size(), 5 );
    
    json::Value empty_size_test{json::Object{}};
    M_ASSERT_EQ( empty_size_test.size(), 0 );
    M_ASSERT_EQ( empty_size_test.to<json::Object>().size(), 0 );
    M_ASSERT_TRUE( empty_size_test.to<json::Object>().empty() );
    
    // Test [] operator (key access)
    json::Value subscript_test{json::Object{{"first", 10}, {"second", 20}, {"third", 30}}};
    M_ASSERT_EQ( subscript_test["first"].to<json::Number>(), 10 );
    M_ASSERT_EQ( subscript_test["second"].to<json::Number>(), 20 );
    M_ASSERT_EQ( subscript_test["third"].to<json::Number>(), 30 );
    
    // Test [] operator with mixed types
    json::Value mixed_subscript{json::Object{
        {"number", 42},     // implicit number conversion
        {"text", "test"},   // implicit string conversion
        {"flag", true},     // implicit bool conversion
        {"empty", nullptr}  // implicit null conversion
    }};
    M_ASSERT_EQ( mixed_subscript["number"].to<json::Number>(), 42 );
    M_ASSERT_EQ( mixed_subscript["text"].to<json::String>(), "test" );
    M_ASSERT_EQ( mixed_subscript["flag"].to<json::Bool>(), true );
    M_ASSERT_EQ( mixed_subscript["empty"].type(), json::Type::eNull );
    
    // Test [] operator modification
    json::Value modify_test{json::Object{{"a", 1}, {"b", 2}, {"c", 3}}};
    modify_test["a"] = 100;         // implicit number conversion
    modify_test["b"] = "modified";  // implicit string conversion
    modify_test["c"] = false;       // implicit bool conversion
    
    M_ASSERT_EQ( modify_test["a"].to<json::Number>(), 100 );
    M_ASSERT_EQ( modify_test["b"].to<json::String>(), "modified" );
    M_ASSERT_EQ( modify_test["c"].to<json::Bool>(), false );
    
    // Test [] operator with new keys
    json::Value new_key_test{json::Object{{"existing", 1}}};
    new_key_test["new_key"] = "new_value";  // implicit conversion
    M_ASSERT_EQ( new_key_test.size(), 2 );
    M_ASSERT_EQ( new_key_test["existing"].to<json::Number>(), 1 );
    M_ASSERT_EQ( new_key_test["new_key"].to<json::String>(), "new_value" );
    
    // Test const [] operator
    const json::Value const_obj{json::Object{{"x", 1}, {"y", 2}, {"z", 3}}};
    M_ASSERT_EQ( const_obj["x"].to<json::Number>(), 1 );
    M_ASSERT_EQ( const_obj["y"].to<json::Number>(), 2 );
    M_ASSERT_EQ( const_obj["z"].to<json::Number>(), 3 );
    
    // Test at() function
    json::Value at_test{json::Object{{"alpha", 100}, {"beta", 200}, {"gamma", 300}}};
    M_ASSERT_NO_THROW( std::ignore = at_test.at("alpha") );
    M_ASSERT_NO_THROW( std::ignore = at_test.at("beta") );
    M_ASSERT_NO_THROW( std::ignore = at_test.at("gamma") );
    
    M_ASSERT_EQ( at_test.at("alpha").to<json::Number>(), 100 );
    M_ASSERT_EQ( at_test.at("beta").to<json::Number>(), 200 );
    M_ASSERT_EQ( at_test.at("gamma").to<json::Number>(), 300 );
    
    // Test at() with non-existent keys (should throw)
    M_ASSERT_THROW( std::ignore = at_test.at("nonexistent"), std::out_of_range );
    M_ASSERT_THROW( std::ignore = at_test.at("missing"), std::out_of_range );
    M_ASSERT_THROW( std::ignore = at_test.at(""), std::out_of_range );
    
    // Test at() with empty object (should throw)
    json::Value empty_at_test{json::Object{}};
    M_ASSERT_THROW( std::ignore = empty_at_test.at("any"), std::out_of_range );
    M_ASSERT_THROW( std::ignore = empty_at_test.at("key"), std::out_of_range );
    
    // Test const at() function
    const json::Value const_at_test{json::Object{{"one", 1}, {"two", 2}, {"three", 3}}};
    M_ASSERT_NO_THROW( std::ignore = const_at_test.at("one") );
    M_ASSERT_EQ( const_at_test.at("one").to<json::Number>(), 1 );
    M_ASSERT_EQ( const_at_test.at("two").to<json::Number>(), 2 );
    M_ASSERT_EQ( const_at_test.at("three").to<json::Number>(), 3 );
    M_ASSERT_THROW( std::ignore = const_at_test.at("four"), std::out_of_range );
    
    // Test get<T>() method
    json::Value get_test{json::Object{{"key1", 1}, {"key2", 2}, {"key3", 3}}};
    M_ASSERT_NO_THROW( std::ignore = get_test.to<json::Object>() );
    auto obj = get_test.to<json::Object>();
    M_ASSERT_EQ( obj.size(), 3 );
    M_ASSERT_EQ( obj["key1"].to<json::Number>(), 1 );
    M_ASSERT_EQ( obj["key2"].to<json::Number>(), 2 );
    M_ASSERT_EQ( obj["key3"].to<json::Number>(), 3 );
    
    // Test get_ref<T>() method
    json::Value ref_test{json::Object{{"data1", 10}, {"data2", 20}, {"data3", 30}}};
    M_ASSERT_NO_THROW( auto& ref = ref_test.get<json::Object>() );
    auto& obj_ref = ref_test.get<json::Object>();
    M_ASSERT_EQ( obj_ref.size(), 3 );
    
    // Test reference modification
    obj_ref["data1"] = 100;  // implicit number conversion
    obj_ref["new_data"] = "added";  // implicit string conversion
    M_ASSERT_EQ( ref_test.size(), 4 );
    M_ASSERT_EQ( ref_test["data1"].to<json::Number>(), 100 );
    M_ASSERT_EQ( ref_test["new_data"].to<json::String>(), "added" );
    
    // Test const get_ref
    const json::Value const_ref_test{json::Object{{"item1", 1}, {"item2", 2}, {"item3", 3}}};
    M_ASSERT_NO_THROW( auto& const_ref = const_ref_test.get<json::Object>() );
    const auto& const_obj_ref = const_ref_test.get<json::Object>();
    M_ASSERT_EQ( const_obj_ref.size(), 3 );
    M_ASSERT_EQ( const_obj_ref.at("item1").to<json::Number>(), 1 );
    
    // Test get_ref type safety - wrong type should throw
    json::Value wrong_type_val{42};
    M_ASSERT_THROW( std::ignore = wrong_type_val.get<json::Object>(), std::runtime_error );
    
    json::Value string_val{"not object"};
    M_ASSERT_THROW( std::ignore = string_val.get<json::Object>(), std::runtime_error );
    
    // Test assignment operations
    json::Value assign_val{json::Type::eObject};
    M_ASSERT_NO_THROW( assign_val = json::Object{{"key1", 1}, {"key2", 2}, {"key3", 3}} );
    M_ASSERT_EQ( assign_val.size(), 3 );
    M_ASSERT_EQ( assign_val["key1"].to<json::Number>(), 1 );
    
    M_ASSERT_NO_THROW( assign_val = json::Object{{"newkey1", 10}, {"newkey2", 20}} );
    M_ASSERT_EQ( assign_val.size(), 2 );
    M_ASSERT_EQ( assign_val["newkey1"].to<json::Number>(), 10 );
    M_ASSERT_EQ( assign_val["newkey2"].to<json::Number>(), 20 );
    
    // Test nested objects
    json::Value nested_obj{json::Object{
        {"person", json::Object{
            {"name", "John"},
            {"age", 30},
            {"active", true}
        }},
        {"company", json::Object{
            {"name", "TechCorp"},
            {"employees", 100},
            {"public", false}
        }},
        {"metadata", json::Object{
            {"version", "1.0"},
            {"timestamp", 1234567890},
            {"valid", true}
        }}
    }};
    M_ASSERT_EQ( nested_obj.type(), json::Type::eObject );
    M_ASSERT_EQ( nested_obj.size(), 3 );
    
    // Test accessing nested object elements
    M_ASSERT_EQ( nested_obj["person"].type(), json::Type::eObject );
    M_ASSERT_EQ( nested_obj["person"].size(), 3 );
    M_ASSERT_EQ( nested_obj["person"]["name"].to<json::String>(), "John" );
    M_ASSERT_EQ( nested_obj["person"]["age"].to<json::Number>(), 30 );
    M_ASSERT_EQ( nested_obj["person"]["active"].to<json::Bool>(), true );
    
    M_ASSERT_EQ( nested_obj["company"]["name"].to<json::String>(), "TechCorp" );
    M_ASSERT_EQ( nested_obj["company"]["employees"].to<json::Number>(), 100 );
    M_ASSERT_EQ( nested_obj["company"]["public"].to<json::Bool>(), false );
    
    M_ASSERT_EQ( nested_obj["metadata"]["version"].to<json::String>(), "1.0" );
    M_ASSERT_EQ( nested_obj["metadata"]["timestamp"].to<json::Number>(), 1234567890 );
    M_ASSERT_EQ( nested_obj["metadata"]["valid"].to<json::Bool>(), true );
    
    // Test nested objects with at()
    M_ASSERT_NO_THROW( std::ignore = nested_obj.at("person").at("name") );
    M_ASSERT_EQ( nested_obj.at("person").at("name").to<json::String>(), "John" );
    M_ASSERT_EQ( nested_obj.at("company").at("employees").to<json::Number>(), 100 );
    M_ASSERT_EQ( nested_obj.at("metadata").at("valid").to<json::Bool>(), true );
    
    // Test nested objects bounds checking
    M_ASSERT_THROW( std::ignore = nested_obj.at("nonexistent"), std::out_of_range );
    M_ASSERT_THROW( std::ignore = nested_obj.at("person").at("nonexistent"), std::out_of_range );
    M_ASSERT_THROW( std::ignore = nested_obj.at("company").at("missing"), std::out_of_range );
    
    // Test deeply nested objects (3 levels)
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
    M_ASSERT_EQ( deep_nested.size(), 1 );
    M_ASSERT_EQ( deep_nested["level1"].size(), 1 );
    M_ASSERT_EQ( deep_nested["level1"]["level2"].size(), 1 );
    M_ASSERT_EQ( deep_nested["level1"]["level2"]["level3"].size(), 2 );
    
    M_ASSERT_EQ( deep_nested["level1"]["level2"]["level3"]["data1"].to<json::String>(), "deep_value1" );
    M_ASSERT_EQ( deep_nested["level1"]["level2"]["level3"]["data2"].to<json::String>(), "deep_value2" );
    
    // Test deeply nested objects with at()
    M_ASSERT_EQ( deep_nested.at("level1").at("level2").at("level3").at("data1").to<json::String>(), "deep_value1" );
    M_ASSERT_EQ( deep_nested.at("level1").at("level2").at("level3").at("data2").to<json::String>(), "deep_value2" );
    M_ASSERT_THROW( std::ignore = deep_nested.at("level1").at("level2").at("level3").at("nonexistent"), std::out_of_range );
    
    // Test complex nested objects structure
    json::Value complex_nested{json::Object{
        {"config", json::Object{
            {"database", json::Object{
                {"host", "localhost"},
                {"port", 5432},
                {"name", "mydb"}
            }},
            {"cache", json::Object{
                {"enabled", true},
                {"ttl", 3600},
                {"size", 1000}
            }},
            {"logging", json::Object{
                {"level", "INFO"},
                {"file", "/var/log/app.log"},
                {"rotation", true}
            }}
        }},
        {"features", json::Object{
            {"auth", json::Object{
                {"enabled", true},
                {"provider", "oauth2"},
                {"timeout", 300}
            }},
            {"api", json::Object{
                {"version", "v1"},
                {"rate_limit", 1000},
                {"cors", false}
            }}
        }}
    }};
    
    // Test the complex structure dimensions
    M_ASSERT_EQ( complex_nested.size(), 2 );
    M_ASSERT_EQ( complex_nested["config"].size(), 3 );
    M_ASSERT_EQ( complex_nested["features"].size(), 2 );
    
    // Test database config
    M_ASSERT_EQ( complex_nested["config"]["database"].size(), 3 );
    M_ASSERT_EQ( complex_nested["config"]["database"]["host"].to<json::String>(), "localhost" );
    M_ASSERT_EQ( complex_nested["config"]["database"]["port"].to<json::Number>(), 5432 );
    M_ASSERT_EQ( complex_nested["config"]["database"]["name"].to<json::String>(), "mydb" );
    
    // Test cache config
    M_ASSERT_EQ( complex_nested["config"]["cache"]["enabled"].to<json::Bool>(), true );
    M_ASSERT_EQ( complex_nested["config"]["cache"]["ttl"].to<json::Number>(), 3600 );
    M_ASSERT_EQ( complex_nested["config"]["cache"]["size"].to<json::Number>(), 1000 );
    
    // Test logging config
    M_ASSERT_EQ( complex_nested["config"]["logging"]["level"].to<json::String>(), "INFO" );
    M_ASSERT_EQ( complex_nested["config"]["logging"]["file"].to<json::String>(), "/var/log/app.log" );
    M_ASSERT_EQ( complex_nested["config"]["logging"]["rotation"].to<json::Bool>(), true );
    
    // Test auth features
    M_ASSERT_EQ( complex_nested["features"]["auth"]["enabled"].to<json::Bool>(), true );
    M_ASSERT_EQ( complex_nested["features"]["auth"]["provider"].to<json::String>(), "oauth2" );
    M_ASSERT_EQ( complex_nested["features"]["auth"]["timeout"].to<json::Number>(), 300 );
    
    // Test api features
    M_ASSERT_EQ( complex_nested["features"]["api"]["version"].to<json::String>(), "v1" );
    M_ASSERT_EQ( complex_nested["features"]["api"]["rate_limit"].to<json::Number>(), 1000 );
    M_ASSERT_EQ( complex_nested["features"]["api"]["cors"].to<json::Bool>(), false );
    
    // Test Value-to-Value comparison
    json::Value obj_cmp1{json::Object{{"a", 1}, {"b", 2}, {"c", 3}}};
    json::Value obj_cmp2{json::Object{{"a", 1}, {"b", 2}, {"c", 3}}};
    json::Value obj_cmp3{json::Object{{"a", 1}, {"b", 2}, {"c", 4}}};
    json::Value obj_cmp4{json::Object{{"a", 1}, {"b", 2}}};
    
    M_ASSERT_TRUE( obj_cmp1 == obj_cmp2 );
    M_ASSERT_FALSE( obj_cmp1 == obj_cmp3 );
    M_ASSERT_FALSE( obj_cmp1 == obj_cmp4 );
    
    // Test object comparison with different types
    json::Value str_val{"not object"};
    json::Value num_val{42};
    M_ASSERT_FALSE( obj_cmp1 == str_val );
    M_ASSERT_FALSE( obj_cmp1 == num_val );
    
    // Test empty object comparison
    json::Value empty1{json::Object{}};
    json::Value empty2{json::Object{}};
    M_ASSERT_TRUE( empty1 == empty2 );
    M_ASSERT_FALSE( empty1 == obj_cmp1 );
    
    // Test nested object comparison
    json::Value nested_cmp1{json::Object{
        {"outer", json::Object{{"inner", 1}}}
    }};
    json::Value nested_cmp2{json::Object{
        {"outer", json::Object{{"inner", 1}}}
    }};
    json::Value nested_cmp3{json::Object{
        {"outer", json::Object{{"inner", 2}}}
    }};
    
    M_ASSERT_TRUE( nested_cmp1 == nested_cmp2 );
    M_ASSERT_FALSE( nested_cmp1 == nested_cmp3 );
    
    // Test basic serialization
    json::Value serial_test{json::Object{{"key1", 1}, {"key2", 2}, {"key3", 3}}};
    M_ASSERT_NO_THROW( serial_test.serialize() );
    
    json::Value empty_serial{json::Object{}};
    M_ASSERT_EQ( empty_serial.serialize(), "{}" );
    
    // Test mixed type serialization
    json::Value mixed_serial{json::Object{
        {"number", 42},       // implicit conversion
        {"string", "test"},   // implicit conversion
        {"boolean", true},    // implicit conversion
        {"null_val", nullptr} // implicit null conversion
    }};
    M_ASSERT_NO_THROW( mixed_serial.serialize() );
    
    // Test nested object serialization
    json::Value nested_serial{json::Object{
        {"outer1", json::Object{{"inner1", 1}, {"inner2", 2}}},
        {"outer2", json::Object{{"inner3", 3}, {"inner4", 4}}}
    }};
    M_ASSERT_NO_THROW( nested_serial.serialize() );
    
    // Test pretty serialization
    M_ASSERT_NO_THROW( serial_test.serialize_pretty() );
    M_ASSERT_NO_THROW( nested_serial.serialize_pretty() );
    
    // Test parsing and comparison (to handle unordered_map randomness)
    auto parsed_simple = json::parse("{\"key1\":1,\"key2\":2,\"key3\":3}");
    if (parsed_simple.has_value()) {
        M_ASSERT_EQ( parsed_simple->type(), json::Type::eObject );
        M_ASSERT_EQ( parsed_simple->size(), 3 );
        M_ASSERT_EQ( (*parsed_simple)["key1"].to<json::Number>(), 1 );
        M_ASSERT_EQ( (*parsed_simple)["key2"].to<json::Number>(), 2 );
        M_ASSERT_EQ( (*parsed_simple)["key3"].to<json::Number>(), 3 );
        
        // Compare with code-created object
        json::Value code_created{json::Object{{"key1", 1}, {"key2", 2}, {"key3", 3}}};
        M_ASSERT_TRUE( *parsed_simple == code_created );
    } else {
        M_ASSERT_FAIL("Failed to parse simple object");
    }
    
    auto parsed_empty = json::parse("{}");
    if (parsed_empty.has_value()) {
        M_ASSERT_EQ( parsed_empty->type(), json::Type::eObject );
        M_ASSERT_EQ( parsed_empty->size(), 0 );
        
        json::Value empty_code{json::Object{}};
        M_ASSERT_TRUE( *parsed_empty == empty_code );
    } else {
        M_ASSERT_FAIL("Failed to parse empty object");
    }
    
    // Test nested object parsing
    auto parsed_nested = json::parse("{\"outer\":{\"inner1\":1,\"inner2\":2}}");
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
    auto parsed_mixed = json::parse("{\"number\":42,\"string\":\"test\",\"boolean\":true,\"null_val\":null}");
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
        M_ASSERT_EQ( parsed_back_obj->type(), json::Type::eObject );
        M_ASSERT_EQ( parsed_back_obj->size(), original_obj.size() );
        M_ASSERT_TRUE( *parsed_back_obj == original_obj );
    } else {
        M_ASSERT_FAIL("Failed to parse back serialized object");
    }
    
    // Test type safety - wrong type access should throw
    M_ASSERT_THROW( std::ignore = obj_val.to<json::Number>(), std::runtime_error );
    M_ASSERT_THROW( std::ignore = obj_val.to<json::String>(), std::runtime_error );
    M_ASSERT_THROW( std::ignore = obj_val.to<json::Bool>(), std::runtime_error );
    M_ASSERT_THROW( std::ignore = obj_val.to<json::Array>(), std::runtime_error );
    M_ASSERT_THROW( std::ignore = obj_val.to<std::nullptr_t>(), std::runtime_error );
    
    // Test Object get_ref type safety
    M_ASSERT_THROW( std::ignore = obj_val.get<json::Number>(), std::runtime_error );
    M_ASSERT_THROW( std::ignore = obj_val.get<json::String>(), std::runtime_error );
    M_ASSERT_THROW( std::ignore = obj_val.get<json::Bool>(), std::runtime_error );
    M_ASSERT_THROW( std::ignore = obj_val.get<json::Array>(), std::runtime_error );
    M_ASSERT_THROW( std::ignore = obj_val.get<std::nullptr_t>(), std::runtime_error );
    
    // Test object consistency across different construction methods
    json::Value consistency_obj1{json::Object{{"x", 1}, {"y", 2}, {"z", 3}}};
    json::Value consistency_obj2{json::Object{{"x", 1}, {"y", 2}, {"z", 3}}};
    
    M_ASSERT_TRUE( consistency_obj1 == consistency_obj2 );
    M_ASSERT_EQ( consistency_obj1.size(), consistency_obj2.size() );
    
    // Test contains() method
    json::Value contains_test{json::Object{{"key1", 1}, {"key2", "hello"}, {"key3", true}}};
    
    // Test with existing keys
    M_ASSERT_TRUE( contains_test.contains("key1") );
    M_ASSERT_TRUE( contains_test.contains("key2") );
    M_ASSERT_TRUE( contains_test.contains("key3") );
    
    // Test with non-existent keys
    M_ASSERT_FALSE( contains_test.contains("nonexistent") );
    M_ASSERT_FALSE( contains_test.contains("missing") );
    M_ASSERT_FALSE( contains_test.contains("") );
    
    // Test with empty object
    json::Value empty_contains_test{json::Object{}};
    M_ASSERT_FALSE( empty_contains_test.contains("anything") );
    
    // Test modification through [] operator reference
    json::Value ref_modify_test{json::Object{{"a", 1}, {"b", 2}, {"c", 3}}};
    
    // Get references and modify through them
    auto& ref_a = ref_modify_test["a"];
    auto& ref_b = ref_modify_test["b"];
    auto& ref_c = ref_modify_test["c"];
    
    ref_a = 100;        // implicit conversion
    ref_b = "modified"; // implicit conversion
    ref_c = true;       // implicit conversion
    
    M_ASSERT_EQ( ref_modify_test["a"].to<json::Number>(), 100 );
    M_ASSERT_EQ( ref_modify_test["b"].to<json::String>(), "modified" );
    M_ASSERT_EQ( ref_modify_test["c"].to<json::Bool>(), true );
    
    // Test modification through at() reference
    json::Value at_modify_test{json::Object{{"first", 10}, {"second", 20}, {"third", 30}}};
    
    // Get references through at() and modify
    auto& at_ref_first = at_modify_test.at("first");
    auto& at_ref_second = at_modify_test.at("second");
    auto& at_ref_third = at_modify_test.at("third");
    
    at_ref_first = "one";      // implicit conversion
    at_ref_second = false;     // implicit conversion
    at_ref_third = nullptr;    // implicit null conversion
    
    M_ASSERT_EQ( at_modify_test.at("first").to<json::String>(), "one" );
    M_ASSERT_EQ( at_modify_test.at("second").to<json::Bool>(), false );
    M_ASSERT_EQ( at_modify_test.at("third").type(), json::Type::eNull );
    
    // Test nested object modification through [] references
    json::Value nested_ref_modify{json::Object{
        {"section1", json::Object{{"item1", 1}, {"item2", 2}}},
        {"section2", json::Object{{"item3", 3}, {"item4", 4}}}
    }};
    
    // Get nested references and modify
    auto& nested_ref_item1 = nested_ref_modify["section1"]["item1"];
    auto& nested_ref_item3 = nested_ref_modify["section2"]["item3"];
    
    nested_ref_item1 = "modified1";  // implicit conversion
    nested_ref_item3 = true;         // implicit conversion
    
    M_ASSERT_EQ( nested_ref_modify["section1"]["item1"].to<json::String>(), "modified1" );
    M_ASSERT_EQ( nested_ref_modify["section1"]["item2"].to<json::Number>(), 2 ); // unchanged
    M_ASSERT_EQ( nested_ref_modify["section2"]["item3"].to<json::Bool>(), true );
    M_ASSERT_EQ( nested_ref_modify["section2"]["item4"].to<json::Number>(), 4 ); // unchanged
    
    // Test nested object modification through at() references
    json::Value nested_at_modify{json::Object{
        {"group1", json::Object{{"val1", 100}, {"val2", 200}}},
        {"group2", json::Object{{"val3", 300}, {"val4", 400}}}
    }};
    
    // Get nested references through at() and modify
    auto& nested_at_ref_val1 = nested_at_modify.at("group1").at("val1");
    auto& nested_at_ref_val4 = nested_at_modify.at("group2").at("val4");
    
    nested_at_ref_val1 = "at_modified";  // implicit conversion
    nested_at_ref_val4 = nullptr;        // implicit null conversion
    
    M_ASSERT_EQ( nested_at_modify.at("group1").at("val1").to<json::String>(), "at_modified" );
    M_ASSERT_EQ( nested_at_modify.at("group1").at("val2").to<json::Number>(), 200 ); // unchanged
    M_ASSERT_EQ( nested_at_modify.at("group2").at("val3").to<json::Number>(), 300 ); // unchanged
    M_ASSERT_EQ( nested_at_modify.at("group2").at("val4").type(), json::Type::eNull );
    
    // Test serialization after reference modifications
    json::Value serial_after_ref{json::Object{{"key1", 1}, {"key2", 2}, {"key3", 3}}};
    serial_after_ref["key2"] = "modified";  // implicit conversion
    
    auto serialized_after = serial_after_ref.serialize();
    // Note: Due to potential unordered_map randomness, we test by parsing and comparing
    auto parsed_after = json::parse(serialized_after);
    if (parsed_after.has_value()) {
        M_ASSERT_TRUE( *parsed_after == serial_after_ref );
        M_ASSERT_EQ( (*parsed_after)["key2"].to<json::String>(), "modified" );
    } else {
        M_ASSERT_FAIL("Failed to parse after reference modification");
    }
}
