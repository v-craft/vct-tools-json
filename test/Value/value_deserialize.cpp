#include <vct/test_unit_macros.hpp>

import std;
import vct.test.unit;
import vct.tools.json;


using namespace vct::tools;

M_TEST(Value, Deserialize) {
    // Test basic JSON types deserialization
    
    // Test number parsing
    {
        auto result_int = json::parse("42");
        M_ASSERT_TRUE(result_int.has_value());
        M_ASSERT_EQ(result_int->type(), json::Type::eNumber);
        M_ASSERT_EQ(result_int->get<json::Number>(), 42);
    }
    
    {
        auto result_float = json::parse("3.14159");
        M_ASSERT_TRUE(result_float.has_value());
        M_ASSERT_EQ(result_float->type(), json::Type::eNumber);
        M_ASSERT_EQ(result_float->get<json::Number>(), 3.14159);
    }
    
    {
        auto result_negative = json::parse("-123.45");
        M_ASSERT_TRUE(result_negative.has_value());
        M_ASSERT_EQ(result_negative->type(), json::Type::eNumber);
        M_ASSERT_EQ(result_negative->get<json::Number>(), -123.45);
    }
    
    // Test string parsing
    {
        auto result_string = json::parse("\"hello world\"");
        M_ASSERT_TRUE(result_string.has_value());
        M_ASSERT_EQ(result_string->type(), json::Type::eString);
        M_ASSERT_EQ(result_string->get<json::String>(), "hello world");
    }
    
    {
        auto result_empty_string = json::parse("\"\"");
        M_ASSERT_TRUE(result_empty_string.has_value());
        M_ASSERT_EQ(result_empty_string->type(), json::Type::eString);
        M_ASSERT_EQ(result_empty_string->get<json::String>(), "");
    }
    
    // Test string with escape sequences
    {
        auto result_escape = json::parse("\"Hello\\nWorld\\t!\"");
        M_ASSERT_TRUE(result_escape.has_value());
        M_ASSERT_EQ(result_escape->type(), json::Type::eString);
        M_ASSERT_EQ(result_escape->get<json::String>(), "Hello\nWorld\t!");
    }
    
    {
        auto result_quote_escape = json::parse("\"Quote: \\\"test\\\" and backslash: \\\\\"");
        M_ASSERT_TRUE(result_quote_escape.has_value());
        M_ASSERT_EQ(result_quote_escape->type(), json::Type::eString);
        M_ASSERT_EQ(result_quote_escape->get<json::String>(), "Quote: \"test\" and backslash: \\");
    }
    
    // Test boolean parsing
    {
        auto result_true = json::parse("true");
        M_ASSERT_TRUE(result_true.has_value());
        M_ASSERT_EQ(result_true->type(), json::Type::eBool);
        M_ASSERT_EQ(result_true->get<json::Bool>(), true);
    }
    
    {
        auto result_false = json::parse("false");
        M_ASSERT_TRUE(result_false.has_value());
        M_ASSERT_EQ(result_false->type(), json::Type::eBool);
        M_ASSERT_EQ(result_false->get<json::Bool>(), false);
    }
    
    // Test null parsing
    {
        auto result_null = json::parse("null");
        M_ASSERT_TRUE(result_null.has_value());
        M_ASSERT_EQ(result_null->type(), json::Type::eNull);
    }
    
    // Test simple array parsing
    {
        auto result_array = json::parse("[1, 2, 3]");
        M_ASSERT_TRUE(result_array.has_value());
        M_ASSERT_EQ(result_array->type(), json::Type::eArray);
        M_ASSERT_EQ(result_array->size(), 3);
        M_ASSERT_EQ((*result_array)[0].get<json::Number>(), 1);
        M_ASSERT_EQ((*result_array)[1].get<json::Number>(), 2);
        M_ASSERT_EQ((*result_array)[2].get<json::Number>(), 3);
    }
    
    {
        auto result_empty_array = json::parse("[]");
        M_ASSERT_TRUE(result_empty_array.has_value());
        M_ASSERT_EQ(result_empty_array->type(), json::Type::eArray);
        M_ASSERT_EQ(result_empty_array->size(), 0);
    }
    
    // Test mixed array parsing
    {
        auto result_mixed_array = json::parse("[1, \"hello\", true, null, false]");
        M_ASSERT_TRUE(result_mixed_array.has_value());
        M_ASSERT_EQ(result_mixed_array->type(), json::Type::eArray);
        M_ASSERT_EQ(result_mixed_array->size(), 5);
        M_ASSERT_EQ((*result_mixed_array)[0].get<json::Number>(), 1);
        M_ASSERT_EQ((*result_mixed_array)[1].get<json::String>(), "hello");
        M_ASSERT_EQ((*result_mixed_array)[2].get<json::Bool>(), true);
        M_ASSERT_EQ((*result_mixed_array)[3].type(), json::Type::eNull);
        M_ASSERT_EQ((*result_mixed_array)[4].get<json::Bool>(), false);
    }
    
    // Test simple object parsing
    {
        auto result_object = json::parse("{\"name\": \"John\", \"age\": 30}");
        M_ASSERT_TRUE(result_object.has_value());
        M_ASSERT_EQ(result_object->type(), json::Type::eObject);
        M_ASSERT_EQ(result_object->size(), 2);
        M_ASSERT_TRUE(result_object->contains("name"));
        M_ASSERT_TRUE(result_object->contains("age"));
        M_ASSERT_EQ((*result_object)["name"].get<json::String>(), "John");
        M_ASSERT_EQ((*result_object)["age"].get<json::Number>(), 30);
    }
    
    {
        auto result_empty_object = json::parse("{}");
        M_ASSERT_TRUE(result_empty_object.has_value());
        M_ASSERT_EQ(result_empty_object->type(), json::Type::eObject);
        M_ASSERT_EQ(result_empty_object->size(), 0);
    }
    
    // Test nested object parsing
    {
        std::string json_str = R"({
            "user": {
                "id": 123,
                "name": "Alice",
                "active": true,
                "profile": {
                    "email": "alice@example.com",
                    "preferences": {
                        "theme": "dark",
                        "notifications": false
                    }
                }
            },
            "metadata": null
        })";
        
        auto result_nested_object = json::parse(json_str);
        M_ASSERT_TRUE(result_nested_object.has_value());
        M_ASSERT_EQ(result_nested_object->type(), json::Type::eObject);
        M_ASSERT_EQ(result_nested_object->size(), 2);
        
        // Test deep access
        M_ASSERT_EQ((*result_nested_object)["user"]["id"].get<json::Number>(), 123);
        M_ASSERT_EQ((*result_nested_object)["user"]["name"].get<json::String>(), "Alice");
        M_ASSERT_EQ((*result_nested_object)["user"]["active"].get<json::Bool>(), true);
        M_ASSERT_EQ((*result_nested_object)["user"]["profile"]["email"].get<json::String>(), "alice@example.com");
        M_ASSERT_EQ((*result_nested_object)["user"]["profile"]["preferences"]["theme"].get<json::String>(), "dark");
        M_ASSERT_EQ((*result_nested_object)["user"]["profile"]["preferences"]["notifications"].get<json::Bool>(), false);
        M_ASSERT_EQ((*result_nested_object)["metadata"].type(), json::Type::eNull);
    }
    
    // Test nested array parsing
    {
        std::string json_str = R"([
            [1, 2, 3],
            ["a", "b", "c"],
            [true, false, null],
            [
                {"x": 1, "y": 2},
                {"x": 3, "y": 4}
            ]
        ])";
        
        auto result_nested_array = json::parse(json_str);
        M_ASSERT_TRUE(result_nested_array.has_value());
        M_ASSERT_EQ(result_nested_array->type(), json::Type::eArray);
        M_ASSERT_EQ(result_nested_array->size(), 4);
        
        // Test nested array access
        M_ASSERT_EQ((*result_nested_array)[0].size(), 3);
        M_ASSERT_EQ((*result_nested_array)[0][0].get<json::Number>(), 1);
        M_ASSERT_EQ((*result_nested_array)[0][1].get<json::Number>(), 2);
        M_ASSERT_EQ((*result_nested_array)[0][2].get<json::Number>(), 3);
        
        M_ASSERT_EQ((*result_nested_array)[1].size(), 3);
        M_ASSERT_EQ((*result_nested_array)[1][0].get<json::String>(), "a");
        M_ASSERT_EQ((*result_nested_array)[1][1].get<json::String>(), "b");
        M_ASSERT_EQ((*result_nested_array)[1][2].get<json::String>(), "c");
        
        M_ASSERT_EQ((*result_nested_array)[2].size(), 3);
        M_ASSERT_EQ((*result_nested_array)[2][0].get<json::Bool>(), true);
        M_ASSERT_EQ((*result_nested_array)[2][1].get<json::Bool>(), false);
        M_ASSERT_EQ((*result_nested_array)[2][2].type(), json::Type::eNull);
        
        M_ASSERT_EQ((*result_nested_array)[3].size(), 2);
        M_ASSERT_EQ((*result_nested_array)[3][0]["x"].get<json::Number>(), 1);
        M_ASSERT_EQ((*result_nested_array)[3][0]["y"].get<json::Number>(), 2);
        M_ASSERT_EQ((*result_nested_array)[3][1]["x"].get<json::Number>(), 3);
        M_ASSERT_EQ((*result_nested_array)[3][1]["y"].get<json::Number>(), 4);
    }
    
    // Test complex nested structure parsing
    {
        std::string json_str = R"({
            "application": {
                "name": "TestApp",
                "version": "1.0.0",
                "config": {
                    "database": {
                        "host": "localhost",
                        "port": 5432,
                        "credentials": {
                            "username": "admin",
                            "password": "secret"
                        },
                        "options": {
                            "ssl": true,
                            "timeout": 30000,
                            "retries": null
                        }
                    },
                    "features": ["auth", "logging", "monitoring"]
                }
            },
            "users": [
                {
                    "id": 1,
                    "name": "John Doe",
                    "roles": ["admin", "user"],
                    "settings": {
                        "notifications": true,
                        "theme": "dark"
                    }
                },
                {
                    "id": 2,
                    "name": "Jane Smith",
                    "roles": ["user"],
                    "settings": {
                        "notifications": false,
                        "theme": "light"
                    }
                }
            ],
            "statistics": {
                "total_requests": 15420,
                "success_rate": 0.987,
                "average_response_time": 125.6,
                "errors": [],
                "active": true
            }
        })";
        
        auto result_complex = json::parse(json_str);
        M_ASSERT_TRUE(result_complex.has_value());
        M_ASSERT_EQ(result_complex->type(), json::Type::eObject);
        M_ASSERT_EQ(result_complex->size(), 3);
        
        // Test complex access patterns
        M_ASSERT_EQ((*result_complex)["application"]["name"].get<json::String>(), "TestApp");
        M_ASSERT_EQ((*result_complex)["application"]["config"]["database"]["port"].get<json::Number>(), 5432);
        M_ASSERT_EQ((*result_complex)["application"]["config"]["database"]["credentials"]["username"].get<json::String>(), "admin");
        M_ASSERT_EQ((*result_complex)["application"]["config"]["database"]["options"]["ssl"].get<json::Bool>(), true);
        M_ASSERT_EQ((*result_complex)["application"]["config"]["database"]["options"]["retries"].type(), json::Type::eNull);
        M_ASSERT_EQ((*result_complex)["application"]["config"]["features"].size(), 3);
        M_ASSERT_EQ((*result_complex)["application"]["config"]["features"][0].get<json::String>(), "auth");
        M_ASSERT_EQ((*result_complex)["application"]["config"]["features"][1].get<json::String>(), "logging");
        M_ASSERT_EQ((*result_complex)["application"]["config"]["features"][2].get<json::String>(), "monitoring");
        
        M_ASSERT_EQ((*result_complex)["users"].size(), 2);
        M_ASSERT_EQ((*result_complex)["users"][0]["id"].get<json::Number>(), 1);
        M_ASSERT_EQ((*result_complex)["users"][0]["name"].get<json::String>(), "John Doe");
        M_ASSERT_EQ((*result_complex)["users"][0]["roles"].size(), 2);
        M_ASSERT_EQ((*result_complex)["users"][0]["roles"][0].get<json::String>(), "admin");
        M_ASSERT_EQ((*result_complex)["users"][0]["roles"][1].get<json::String>(), "user");
        M_ASSERT_EQ((*result_complex)["users"][0]["settings"]["notifications"].get<json::Bool>(), true);
        M_ASSERT_EQ((*result_complex)["users"][0]["settings"]["theme"].get<json::String>(), "dark");
        
        M_ASSERT_EQ((*result_complex)["users"][1]["id"].get<json::Number>(), 2);
        M_ASSERT_EQ((*result_complex)["users"][1]["name"].get<json::String>(), "Jane Smith");
        M_ASSERT_EQ((*result_complex)["users"][1]["roles"].size(), 1);
        M_ASSERT_EQ((*result_complex)["users"][1]["roles"][0].get<json::String>(), "user");
        M_ASSERT_EQ((*result_complex)["users"][1]["settings"]["notifications"].get<json::Bool>(), false);
        M_ASSERT_EQ((*result_complex)["users"][1]["settings"]["theme"].get<json::String>(), "light");
        
        M_ASSERT_EQ((*result_complex)["statistics"]["total_requests"].get<json::Number>(), 15420);
        M_ASSERT_EQ((*result_complex)["statistics"]["success_rate"].get<json::Number>(), 0.987);
        M_ASSERT_EQ((*result_complex)["statistics"]["average_response_time"].get<json::Number>(), 125.6);
        M_ASSERT_EQ((*result_complex)["statistics"]["errors"].size(), 0);
        M_ASSERT_EQ((*result_complex)["statistics"]["active"].get<json::Bool>(), true);
    }
    
    // Test whitespace handling
    {
        std::string json_str = R"(
            {
                "name"  :  "test"  ,
                "value" :   123   ,
                "array" : [ 1 , 2 , 3 ] ,
                "object": {  "key"  :  "value"  }
            }
        )";
        
        auto result_whitespace = json::parse(json_str);
        M_ASSERT_TRUE(result_whitespace.has_value());
        M_ASSERT_EQ(result_whitespace->type(), json::Type::eObject);
        M_ASSERT_EQ(result_whitespace->size(), 4);
        M_ASSERT_EQ((*result_whitespace)["name"].get<json::String>(), "test");
        M_ASSERT_EQ((*result_whitespace)["value"].get<json::Number>(), 123);
        M_ASSERT_EQ((*result_whitespace)["array"].size(), 3);
        M_ASSERT_EQ((*result_whitespace)["array"][0].get<json::Number>(), 1);
        M_ASSERT_EQ((*result_whitespace)["object"]["key"].get<json::String>(), "value");
    }
    
    // Test round-trip serialization/deserialization
    {
        json::Value original = json::Object{
            {"string", "hello"},
            {"number", 42.5},
            {"bool", true},
            {"null", nullptr},
            {"array", json::Array{1, 2, 3}},
            {"object", json::Object{{"nested", "value"}}}
        };
        
        auto serialized = original.serialize();
        M_ASSERT_TRUE(!serialized.empty());
        
        auto result_roundtrip = json::parse(serialized);
        M_ASSERT_TRUE(result_roundtrip.has_value());
        M_ASSERT_TRUE(*result_roundtrip == original);
        
        // Test access after round-trip
        M_ASSERT_EQ((*result_roundtrip)["string"].get<json::String>(), "hello");
        M_ASSERT_EQ((*result_roundtrip)["number"].get<json::Number>(), 42.5);
        M_ASSERT_EQ((*result_roundtrip)["bool"].get<json::Bool>(), true);
        M_ASSERT_EQ((*result_roundtrip)["null"].type(), json::Type::eNull);
        M_ASSERT_EQ((*result_roundtrip)["array"].size(), 3);
        M_ASSERT_EQ((*result_roundtrip)["array"][0].get<json::Number>(), 1);
        M_ASSERT_EQ((*result_roundtrip)["object"]["nested"].get<json::String>(), "value");
    }
    
    // Test parsing failures - invalid JSON formats
    {
        // Invalid JSON strings that should fail parsing
        std::vector<std::string> invalid_jsons = {
            "",                          // Empty string
            "   ",                       // Only whitespace
            "{",                         // Incomplete object
            "}",                         // Unmatched closing brace
            "[",                         // Incomplete array
            "]",                         // Unmatched closing bracket
            "{\"key\": }",              // Missing value
            "{\"key\": value}",         // Unquoted value
            "{key: \"value\"}",         // Unquoted key
            // "{\"key\": \"value\",}",    // Trailing comma   ----enabled 
            // "[1, 2, 3,]",               // Trailing comma in array  ----enabled
            "{\"key\": \"value\" \"another\": \"value\"}", // Missing comma
            "[1 2 3]",                  // Missing commas
            "\"unclosed string",         // Unclosed string
            "\"invalid\\escape\"",       // Invalid escape sequence
            "123.456.789",              // Invalid number format
            "truee",                    // Invalid boolean
            "nul",                      // Invalid null
            "{\"key\": [}",             // Mismatched brackets
            "[{]",                      // Mismatched brackets
            "\"\\u123\"",               // Invalid unicode escape
            //"{\"duplicate\": 1, \"duplicate\": 2}", // Duplicate keys ----enabled
            "123 456",                  // Multiple top-level values
            "{\"key\": \"value\"} extra", // Extra content after valid JSON
        };
        
        for (const auto& invalid_json : invalid_jsons) {
            auto result_invalid = json::parse(invalid_json);
            M_ASSERT_FALSE(result_invalid.has_value());
        }
    }
    
    // Test specific error cases
    {
        // Test deeply nested structures beyond reasonable limits (if library has limits)
        std::string deeply_nested = "{";
        for (int i = 0; i < 1000; ++i) {
            deeply_nested += "\"key" + std::to_string(i) + "\": {";
        }
        deeply_nested += "\"value\": 42";
        for (int i = 0; i < 1000; ++i) {
            deeply_nested += "}";
        }
        deeply_nested += "}";
        
        // This might succeed or fail depending on implementation limits
        auto result_deep = json::parse(deeply_nested);
        // We don't assert success/failure here as it depends on implementation
    }
    
    // Test parsing with at() method after deserialization
    {
        std::string json_str = R"({"users": [{"name": "John", "age": 30}, {"name": "Jane", "age": 25}]})";
        auto result_at_test = json::parse(json_str);
        M_ASSERT_TRUE(result_at_test.has_value());
        
        // Test at() method access
        M_ASSERT_NO_THROW(std::ignore = result_at_test->at("users").at(0).at("name"));
        M_ASSERT_EQ(result_at_test->at("users").at(0).at("name").get<json::String>(), "John");
        M_ASSERT_EQ(result_at_test->at("users").at(0).at("age").get<json::Number>(), 30);
        M_ASSERT_EQ(result_at_test->at("users").at(1).at("name").get<json::String>(), "Jane");
        M_ASSERT_EQ(result_at_test->at("users").at(1).at("age").get<json::Number>(), 25);
        
        // Test out-of-bounds access
        M_ASSERT_THROW(std::ignore = result_at_test->at("nonexistent"), std::out_of_range);
        M_ASSERT_THROW(std::ignore = result_at_test->at("users").at(2), std::out_of_range);
    }
    
    // Test modification after deserialization
    {
        std::string json_str = R"({"config": {"debug": false, "port": 8080}})";
        auto result_modify = json::parse(json_str);
        M_ASSERT_TRUE(result_modify.has_value());
        
        // Test modification
        (*result_modify)["config"]["debug"] = true;
        (*result_modify)["config"]["port"] = 9090;
        (*result_modify)["config"]["new_setting"] = "added";
        
        // Verify modifications
        M_ASSERT_EQ((*result_modify)["config"]["debug"].get<json::Bool>(), true);
        M_ASSERT_EQ((*result_modify)["config"]["port"].get<json::Number>(), 9090);
        M_ASSERT_EQ((*result_modify)["config"]["new_setting"].get<json::String>(), "added");
        M_ASSERT_EQ((*result_modify)["config"].size(), 3);
    }
    
    // Test get_ref<>() after deserialization
    {
        std::string json_str = R"({"items": [1, 2, 3], "metadata": {"count": 3}})";
        auto result_ref = json::parse(json_str);
        M_ASSERT_TRUE(result_ref.has_value());
        
        // Test get_ref<>() for modification
        auto& items_ref = (*result_ref)["items"].get_ref<json::Array>();
        items_ref.push_back(json::Number(4));
        items_ref.push_back(json::Number(5));
        
        auto& metadata_ref = (*result_ref)["metadata"].get_ref<json::Object>();
        metadata_ref["count"] = json::Number(5);
        metadata_ref["modified"] = json::Bool(true);
        
        // Verify modifications through references
        M_ASSERT_EQ((*result_ref)["items"].size(), 5);
        M_ASSERT_EQ((*result_ref)["items"][3].get<json::Number>(), 4);
        M_ASSERT_EQ((*result_ref)["items"][4].get<json::Number>(), 5);
        M_ASSERT_EQ((*result_ref)["metadata"]["count"].get<json::Number>(), 5);
        M_ASSERT_EQ((*result_ref)["metadata"]["modified"].get<json::Bool>(), true);
    }
}
