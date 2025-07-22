#include <vct/test_unit_macros.hpp>

import std;
import vct.test.unit;
import vct.tools.json;


using namespace vct::tools;

// --- Basic JSON type deserialization ---
M_TEST(Value, Deserialize) {
    // --- Number parsing ---
    {
        auto result_int = json::read("42");
        M_ASSERT_TRUE(result_int.has_value());
        M_ASSERT_EQ(result_int->type(), json::Type::eNumber);
        M_ASSERT_EQ(result_int->num(), 42);
    }
    {
        auto result_float = json::read("3.14159");
        M_ASSERT_TRUE(result_float.has_value());
        M_ASSERT_EQ(result_float->type(), json::Type::eNumber);
        M_ASSERT_EQ(result_float->num(), 3.14159);
    }
    {
        auto result_negative = json::read("-123.45");
        M_ASSERT_TRUE(result_negative.has_value());
        M_ASSERT_EQ(result_negative->type(), json::Type::eNumber);
        M_ASSERT_EQ(result_negative->num(), -123.45);
    }

    // --- String parsing ---
    {
        auto result_string = json::read("\"hello world\"");
        M_ASSERT_TRUE(result_string.has_value());
        M_ASSERT_EQ(result_string->type(), json::Type::eString);
        M_ASSERT_EQ(result_string->str(), "hello world");
    }
    {
        auto result_empty_string = json::read("\"\"");
        M_ASSERT_TRUE(result_empty_string.has_value());
        M_ASSERT_EQ(result_empty_string->type(), json::Type::eString);
        M_ASSERT_EQ(result_empty_string->str(), "");
    }
    // String with escape sequences
    {
        auto result_escape = json::read(R"("Hello\nWorld\t!")");
        M_ASSERT_TRUE(result_escape.has_value());
        M_ASSERT_EQ(result_escape->type(), json::Type::eString);
        M_ASSERT_EQ(result_escape->str(), "Hello\nWorld\t!");
    }
    {
        auto result_quote_escape = json::read(R"("Quote: \"test\" and backslash: \\")");
        M_ASSERT_TRUE(result_quote_escape.has_value());
        M_ASSERT_EQ(result_quote_escape->type(), json::Type::eString);
        M_ASSERT_EQ(result_quote_escape->str(), "Quote: \"test\" and backslash: \\");
    }

    // --- Boolean parsing ---
    {
        auto result_true = json::read("true");
        M_ASSERT_TRUE(result_true.has_value());
        M_ASSERT_EQ(result_true->type(), json::Type::eBool);
        M_ASSERT_EQ(result_true->bol(), true);
    }
    {
        auto result_false = json::read("false");
        M_ASSERT_TRUE(result_false.has_value());
        M_ASSERT_EQ(result_false->type(), json::Type::eBool);
        M_ASSERT_EQ(result_false->bol(), false);
    }

    // --- Null parsing ---
    {
        auto result_null = json::read("null");
        M_ASSERT_TRUE(result_null.has_value());
        M_ASSERT_EQ(result_null->type(), json::Type::eNull);
    }

    // --- Array parsing ---
    {
        auto result_array = json::read("[1, 2, 3]");
        M_ASSERT_TRUE(result_array.has_value());
        M_ASSERT_EQ(result_array->type(), json::Type::eArray);
        M_ASSERT_EQ(result_array->size(), 3);
        M_ASSERT_EQ((*result_array)[0].to<json::Number>(), 1);
        M_ASSERT_EQ((*result_array)[1].to<json::Number>(), 2);
        M_ASSERT_EQ((*result_array)[2].to<json::Number>(), 3);
    }
    {
        auto result_empty_array = json::read("[]");
        M_ASSERT_TRUE(result_empty_array.has_value());
        M_ASSERT_EQ(result_empty_array->type(), json::Type::eArray);
        M_ASSERT_EQ(result_empty_array->size(), 0);
    }
    // Mixed array
    {
        auto result_mixed_array = json::read("[1, \"hello\", true, null, false]");
        M_ASSERT_TRUE(result_mixed_array.has_value());
        M_ASSERT_EQ(result_mixed_array->type(), json::Type::eArray);
        M_ASSERT_EQ(result_mixed_array->size(), 5);
        M_ASSERT_EQ((*result_mixed_array)[0].to<json::Number>(), 1);
        M_ASSERT_EQ((*result_mixed_array)[1].to<json::String>(), "hello");
        M_ASSERT_EQ((*result_mixed_array)[2].to<json::Bool>(), true);
        M_ASSERT_EQ((*result_mixed_array)[3].type(), json::Type::eNull);
        M_ASSERT_EQ((*result_mixed_array)[4].to<json::Bool>(), false);
    }

    // --- Object parsing ---
    {
        auto result_object = json::read(R"({"name": "John", "age": 30})");
        M_ASSERT_TRUE(result_object.has_value());
        M_ASSERT_EQ(result_object->type(), json::Type::eObject);
        M_ASSERT_EQ(result_object->size(), 2);
        M_ASSERT_TRUE(result_object->contains("name"));
        M_ASSERT_TRUE(result_object->contains("age"));
        M_ASSERT_EQ((*result_object)["name"].to<json::String>(), "John");
        M_ASSERT_EQ((*result_object)["age"].to<json::Number>(), 30);
    }
    {
        auto result_empty_object = json::read("{}");
        M_ASSERT_TRUE(result_empty_object.has_value());
        M_ASSERT_EQ(result_empty_object->type(), json::Type::eObject);
        M_ASSERT_EQ(result_empty_object->size(), 0);
    }

    // --- Nested object parsing ---
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
        auto result_nested_object = json::read(json_str);
        M_ASSERT_TRUE(result_nested_object.has_value());
        M_ASSERT_EQ(result_nested_object->type(), json::Type::eObject);
        M_ASSERT_EQ(result_nested_object->size(), 2);
        // Deep access
        M_ASSERT_EQ((*result_nested_object)["user"]["id"].to<json::Number>(), 123);
        M_ASSERT_EQ((*result_nested_object)["user"]["name"].to<json::String>(), "Alice");
        M_ASSERT_EQ((*result_nested_object)["user"]["active"].to<json::Bool>(), true);
        M_ASSERT_EQ((*result_nested_object)["user"]["profile"]["email"].to<json::String>(), "alice@example.com");
        M_ASSERT_EQ((*result_nested_object)["user"]["profile"]["preferences"]["theme"].to<json::String>(), "dark");
        M_ASSERT_EQ((*result_nested_object)["user"]["profile"]["preferences"]["notifications"].to<json::Bool>(), false);
        M_ASSERT_EQ((*result_nested_object)["metadata"].type(), json::Type::eNull);
    }

    // --- Nested array parsing ---
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
        auto result_nested_array = json::read(json_str);
        M_ASSERT_TRUE(result_nested_array.has_value());
        M_ASSERT_EQ(result_nested_array->type(), json::Type::eArray);
        M_ASSERT_EQ(result_nested_array->size(), 4);
        // Nested array access
        M_ASSERT_EQ((*result_nested_array)[0].size(), 3);
        M_ASSERT_EQ((*result_nested_array)[0][0].to<json::Number>(), 1);
        M_ASSERT_EQ((*result_nested_array)[0][1].to<json::Number>(), 2);
        M_ASSERT_EQ((*result_nested_array)[0][2].to<json::Number>(), 3);
        M_ASSERT_EQ((*result_nested_array)[1][0].to<json::String>(), "a");
        M_ASSERT_EQ((*result_nested_array)[1][1].to<json::String>(), "b");
        M_ASSERT_EQ((*result_nested_array)[1][2].to<json::String>(), "c");
        M_ASSERT_EQ((*result_nested_array)[2][0].to<json::Bool>(), true);
        M_ASSERT_EQ((*result_nested_array)[2][1].to<json::Bool>(), false);
        M_ASSERT_EQ((*result_nested_array)[2][2].type(), json::Type::eNull);
        M_ASSERT_EQ((*result_nested_array)[3][0]["x"].to<json::Number>(), 1);
        M_ASSERT_EQ((*result_nested_array)[3][0]["y"].to<json::Number>(), 2);
        M_ASSERT_EQ((*result_nested_array)[3][1]["x"].to<json::Number>(), 3);
        M_ASSERT_EQ((*result_nested_array)[3][1]["y"].to<json::Number>(), 4);
    }

    // --- Complex nested structure parsing ---
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
        auto result_complex = json::read(json_str);
        M_ASSERT_TRUE(result_complex.has_value());
        M_ASSERT_EQ(result_complex->type(), json::Type::eObject);
        M_ASSERT_EQ(result_complex->size(), 3);
        // Complex access patterns
        M_ASSERT_EQ((*result_complex)["application"]["name"].to<json::String>(), "TestApp");
        M_ASSERT_EQ((*result_complex)["application"]["config"]["database"]["port"].to<json::Number>(), 5432);
        M_ASSERT_EQ((*result_complex)["application"]["config"]["database"]["credentials"]["username"].to<json::String>(), "admin");
        M_ASSERT_EQ((*result_complex)["application"]["config"]["database"]["options"]["ssl"].to<json::Bool>(), true);
        M_ASSERT_EQ((*result_complex)["application"]["config"]["database"]["options"]["retries"].type(), json::Type::eNull);
        M_ASSERT_EQ((*result_complex)["application"]["config"]["features"].size(), 3);
        M_ASSERT_EQ((*result_complex)["application"]["config"]["features"][0].to<json::String>(), "auth");
        M_ASSERT_EQ((*result_complex)["application"]["config"]["features"][1].to<json::String>(), "logging");
        M_ASSERT_EQ((*result_complex)["application"]["config"]["features"][2].to<json::String>(), "monitoring");
        M_ASSERT_EQ((*result_complex)["users"].size(), 2);
        M_ASSERT_EQ((*result_complex)["users"][0]["id"].to<json::Number>(), 1);
        M_ASSERT_EQ((*result_complex)["users"][0]["name"].to<json::String>(), "John Doe");
        M_ASSERT_EQ((*result_complex)["users"][0]["roles"].size(), 2);
        M_ASSERT_EQ((*result_complex)["users"][0]["roles"][0].to<json::String>(), "admin");
        M_ASSERT_EQ((*result_complex)["users"][0]["roles"][1].to<json::String>(), "user");
        M_ASSERT_EQ((*result_complex)["users"][0]["settings"]["notifications"].to<json::Bool>(), true);
        M_ASSERT_EQ((*result_complex)["users"][0]["settings"]["theme"].to<json::String>(), "dark");
        M_ASSERT_EQ((*result_complex)["users"][1]["id"].to<json::Number>(), 2);
        M_ASSERT_EQ((*result_complex)["users"][1]["name"].to<json::String>(), "Jane Smith");
        M_ASSERT_EQ((*result_complex)["users"][1]["roles"].size(), 1);
        M_ASSERT_EQ((*result_complex)["users"][1]["roles"][0].to<json::String>(), "user");
        M_ASSERT_EQ((*result_complex)["users"][1]["settings"]["notifications"].to<json::Bool>(), false);
        M_ASSERT_EQ((*result_complex)["users"][1]["settings"]["theme"].to<json::String>(), "light");
        M_ASSERT_EQ((*result_complex)["statistics"]["total_requests"].to<json::Number>(), 15420);
        M_ASSERT_EQ((*result_complex)["statistics"]["success_rate"].to<json::Number>(), 0.987);
        M_ASSERT_EQ((*result_complex)["statistics"]["average_response_time"].to<json::Number>(), 125.6);
        M_ASSERT_EQ((*result_complex)["statistics"]["errors"].size(), 0);
        M_ASSERT_EQ((*result_complex)["statistics"]["active"].to<json::Bool>(), true);
    }

    // --- Whitespace handling ---
    {
        std::string json_str = R"(
            {
                "name"  :  "test"  ,
                "value" :   123   ,
                "array" : [ 1 , 2 , 3 ] ,
                "object": {  "key"  :  "value"  }
            }
        )";
        auto result_whitespace = json::read(json_str);
        M_ASSERT_TRUE(result_whitespace.has_value());
        M_ASSERT_EQ(result_whitespace->type(), json::Type::eObject);
        M_ASSERT_EQ(result_whitespace->size(), 4);
        M_ASSERT_EQ((*result_whitespace)["name"].to<json::String>(), "test");
        M_ASSERT_EQ((*result_whitespace)["value"].to<json::Number>(), 123);
        M_ASSERT_EQ((*result_whitespace)["array"].size(), 3);
        M_ASSERT_EQ((*result_whitespace)["array"][0].to<json::Number>(), 1);
        M_ASSERT_EQ((*result_whitespace)["object"]["key"].to<json::String>(), "value");
    }

    // --- Round-trip serialization/deserialization ---
    {
        json::Value original = json::Object{
            {"string", "hello"},
            {"number", 42.5},
            {"bool", true},
            {"null", nullptr},
            {"array", json::Array{1, 2, 3}},
            {"object", json::Object{{"nested", "value"}}}
        };
        auto serialized = original.dump();
        M_ASSERT_TRUE(!serialized.empty());
        auto result_roundtrip = json::read(serialized);
        M_ASSERT_TRUE(result_roundtrip.has_value());
        M_ASSERT_TRUE(*result_roundtrip == original);
        // Access after round-trip
        M_ASSERT_EQ((*result_roundtrip)["string"].to<json::String>(), "hello");
        M_ASSERT_EQ((*result_roundtrip)["number"].to<json::Number>(), 42.5);
        M_ASSERT_EQ((*result_roundtrip)["bool"].to<json::Bool>(), true);
        M_ASSERT_EQ((*result_roundtrip)["null"].type(), json::Type::eNull);
        M_ASSERT_EQ((*result_roundtrip)["array"].size(), 3);
        M_ASSERT_EQ((*result_roundtrip)["array"][0].to<json::Number>(), 1);
        M_ASSERT_EQ((*result_roundtrip)["object"]["nested"].to<json::String>(), "value");
    }

    // --- Parsing failures: invalid JSON formats ---
    {
        std::vector<std::string> invalid_jsons = {
            "", "   ", "{", "}", "[", "]",
            "{\"key\": }", "{\"key\": value}", "{key: \"value\"}",
            "{\"key\": \"value\" \"another\": \"value\"}", "[1 2 3]",
            "\"unclosed string", "\"invalid\\escape\"", "123.456.789",
            "truee", "nul", "{\"key\": [}", "[{]", "\"\\u123\"",
            "123 456", "{\"key\": \"value\"} extra"
        };
        for (const auto& invalid_json : invalid_jsons) {
            auto result_invalid = json::read(invalid_json);
            M_ASSERT_FALSE(result_invalid.has_value());
        }
    }

    // --- Deeply nested structure test (limit check) ---
    {
        std::string deeply_nested = "{";
        for (int i = 0; i < 1000; ++i) deeply_nested += "\"key" + std::to_string(i) + "\": {";
        deeply_nested += "\"value\": 42";
        for (int i = 0; i < 1000; ++i) deeply_nested += "}";
        deeply_nested += "}";
        auto result_deep = json::read(deeply_nested);
        // No assertion: success/failure depends on implementation limits
    }

    // --- Access with at() after deserialization ---
    {
        std::string json_str = R"({"users": [{"name": "John", "age": 30}, {"name": "Jane", "age": 25}]})";
        auto result_at_test = json::read(json_str);
        M_ASSERT_TRUE(result_at_test.has_value());
        // at() method access
        M_ASSERT_NO_THROW(std::ignore = result_at_test->at("users").at(0).at("name"));
        M_ASSERT_EQ(result_at_test->at("users").at(0).at("name").to<json::String>(), "John");
        M_ASSERT_EQ(result_at_test->at("users").at(0).at("age").to<json::Number>(), 30);
        M_ASSERT_EQ(result_at_test->at("users").at(1).at("name").to<json::String>(), "Jane");
        M_ASSERT_EQ(result_at_test->at("users").at(1).at("age").to<json::Number>(), 25);
        // Out-of-bounds access
        M_ASSERT_THROW(std::ignore = result_at_test->at("nonexistent"), std::out_of_range);
        M_ASSERT_THROW(std::ignore = result_at_test->at("users").at(2), std::out_of_range);
    }

    // --- Modification after deserialization ---
    {
        std::string json_str = R"({"config": {"debug": false, "port": 8080}})";
        auto result_modify = json::read(json_str);
        M_ASSERT_TRUE(result_modify.has_value());
        // Modify values
        (*result_modify)["config"]["debug"] = true;
        (*result_modify)["config"]["port"] = 9090;
        (*result_modify)["config"]["new_setting"] = "added";
        // Verify modifications
        M_ASSERT_EQ((*result_modify)["config"]["debug"].to<json::Bool>(), true);
        M_ASSERT_EQ((*result_modify)["config"]["port"].to<json::Number>(), 9090);
        M_ASSERT_EQ((*result_modify)["config"]["new_setting"].to<json::String>(), "added");
        M_ASSERT_EQ((*result_modify)["config"].size(), 3);
    }

    // --- Reference modification after deserialization ---
    {
        std::string json_str = R"({"items": [1, 2, 3], "metadata": {"count": 3}})";
        auto result_ref = json::read(json_str);
        M_ASSERT_TRUE(result_ref.has_value());
        // Modify array and object via reference
        auto& items_ref = (*result_ref)["items"].arr();
        items_ref.push_back(json::Number(4));
        items_ref.push_back(json::Number(5));
        auto& metadata_ref = (*result_ref)["metadata"].obj();
        metadata_ref["count"] = json::Number(5);
        metadata_ref["modified"] = json::Bool(true);
        // Verify modifications
        M_ASSERT_EQ((*result_ref)["items"].size(), 5);
        M_ASSERT_EQ((*result_ref)["items"][3].to<json::Number>(), 4);
        M_ASSERT_EQ((*result_ref)["items"][4].to<json::Number>(), 5);
        M_ASSERT_EQ((*result_ref)["metadata"]["count"].to<json::Number>(), 5);
        M_ASSERT_EQ((*result_ref)["metadata"]["modified"].to<json::Bool>(), true);
    }
}
