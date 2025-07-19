#include <vct/test_unit_macros.hpp>

import std;
import vct.test.unit;
import vct.tools.json;


using namespace vct::tools;

M_TEST(Value, Complex) {
    // Test complex nested structures with all 6 JSON types
    // Number, String, Bool, Null, Array, Object

    json::Value complex_data = json::Object{
        {"application", json::Object{
            {"name", "MyApp"},
            {"version", "2.1.0"},
            {"active", true},
            {"config", json::Object{
                {"debug", false},
                {"port", 8080},
                {"host", "localhost"},
                {"features", json::Array{{
                    json::Object{{"name", "auth"}, {"enabled", true}, {"priority", 1}},
                    json::Object{{"name", "logging"}, {"enabled", false}, {"priority", 2}},
                    json::Object{{"name", "cache"}, {"enabled", true}, {"priority", 3}}
                }}},
                {"database", json::Object{
                    {"type", "postgresql"},
                    {"connection", json::Object{
                        {"host", "db.example.com"},
                        {"port", 5432},
                        {"database", "myapp_db"},
                        {"ssl", true},
                        {"pool", json::Object{
                            {"min", 5},
                            {"max", 20},
                            {"idle_timeout", 30000},
                            {"connection_timeout", nullptr}
                        }}
                    }},
                    {"migrations", json::Array{{
                        json::Object{{"version", "001"}, {"applied", true}, {"timestamp", 1640995200}},
                        json::Object{{"version", "002"}, {"applied", true}, {"timestamp", 1641081600}},
                        json::Object{{"version", "003"}, {"applied", false}, {"timestamp", nullptr}}
                    }}}
                }}
            }}
        }},
        { "users" , json::Array{{
                json::Object{
                    {"id", 1},
                    {"name", "John Doe"},
                    {"email", "john@example.com"},
                    {"active", true},
                    {"permissions", json::Array{{"read", "write", "admin"}}},
                    {"profile", json::Object{
                        {"age", 30},
                        {"country", "USA"},
                        {"preferences", json::Object{
                            {"theme", "dark"},
                            {"notifications", true},
                            {"language", "en"},
                            {"timezone", "UTC-5"}
                        }}
                    }},
                    {"last_login", 1640995200},
                    {"metadata", nullptr}
            }},
            json::Object{
                {"id", 2},
                {"name", "Jane Smith"},
                {"email", "jane@example.com"},
                {"active", false},
                {"permissions", json::Array{{"read"}}},
                {"profile", json::Object{
                    {"age", 25},
                    {"country", "Canada"},
                    {"preferences", json::Object{
                        {"theme", "light"},
                        {"notifications", false},
                        {"language", "fr"},
                        {"timezone", "UTC-4"}
                    }}
                }},
                {"last_login", nullptr},
                {"metadata", json::Object{{"notes", "Inactive user"}, {"created", 1640908800}}}
            }
        }},
        {"statistics", json::Object{
            {"total_users", 2},
            {"active_users", 1},
            {"avg_login_time", 1640995200},
            {"performance", json::Object{
                {"response_times", json::Array{{120, 85, 95, 110, 88}}},
                {"error_rates", json::Array{{0.01, 0.02, 0.015, 0.008, 0.012}}},
                {"uptime", 99.95},
                {"last_restart", nullptr}
            }},
            {"features_usage", json::Array{{
                json::Object{{"feature", "auth"}, {"usage_count", 1500}, {"success_rate", 0.98}},
                json::Object{{"feature", "logging"}, {"usage_count", 0}, {"success_rate", nullptr}},
                json::Object{{"feature", "cache"}, {"usage_count", 800}, {"success_rate", 0.95}}
            }}}
        }}
        // Additional nested structures can be added here
    };
    
    
    // Test basic structure validation
    M_ASSERT_EQ( complex_data.type(), json::Type::eObject );
    M_ASSERT_EQ( complex_data.size(), 3 );
    M_ASSERT_TRUE( complex_data.contains("application") );
    M_ASSERT_TRUE( complex_data.contains("users") );
    M_ASSERT_TRUE( complex_data.contains("statistics") );
    
    // Test deep access through [] operator
    M_ASSERT_EQ( complex_data["application"]["name"].to<json::String>(), "MyApp" );
    M_ASSERT_EQ( complex_data["application"]["version"].to<json::String>(), "2.1.0" );
    M_ASSERT_EQ( complex_data["application"]["active"].to<json::Bool>(), true );
    M_ASSERT_EQ( complex_data["application"]["config"]["port"].to<json::Number>(), 8080 );
    
    // Test array access within nested structure
    M_ASSERT_EQ( complex_data["application"]["config"]["features"].type(), json::Type::eArray );
    M_ASSERT_EQ( complex_data["application"]["config"]["features"].size(), 3 );
    M_ASSERT_EQ( complex_data["application"]["config"]["features"][0]["name"].to<json::String>(), "auth" );
    M_ASSERT_EQ( complex_data["application"]["config"]["features"][0]["enabled"].to<json::Bool>(), true );
    M_ASSERT_EQ( complex_data["application"]["config"]["features"][0]["priority"].to<json::Number>(), 1 );
    
    // Test very deep nesting access
    M_ASSERT_EQ( complex_data["application"]["config"]["database"]["connection"]["pool"]["min"].to<json::Number>(), 5 );
    M_ASSERT_EQ( complex_data["application"]["config"]["database"]["connection"]["pool"]["max"].to<json::Number>(), 20 );
    M_ASSERT_EQ( complex_data["application"]["config"]["database"]["connection"]["pool"]["connection_timeout"].type(), json::Type::eNull );
    
    // Test migrations array access
    M_ASSERT_EQ( complex_data["application"]["config"]["database"]["migrations"].size(), 3 );
    M_ASSERT_EQ( complex_data["application"]["config"]["database"]["migrations"][0]["version"].to<json::String>(), "001" );
    M_ASSERT_EQ( complex_data["application"]["config"]["database"]["migrations"][0]["applied"].to<json::Bool>(), true );
    M_ASSERT_EQ( complex_data["application"]["config"]["database"]["migrations"][2]["timestamp"].type(), json::Type::eNull );
    
    // Test users array access
    M_ASSERT_EQ( complex_data["users"].type(), json::Type::eArray );
    M_ASSERT_EQ( complex_data["users"].size(), 2 );
    M_ASSERT_EQ( complex_data["users"][0]["name"].to<json::String>(), "John Doe" );
    M_ASSERT_EQ( complex_data["users"][0]["permissions"].size(), 3 );
    M_ASSERT_EQ( complex_data["users"][0]["permissions"][0].to<json::String>(), "read" );
    M_ASSERT_EQ( complex_data["users"][0]["permissions"][1].to<json::String>(), "write" );
    M_ASSERT_EQ( complex_data["users"][0]["permissions"][2].to<json::String>(), "admin" );
    
    // Test deep user profile access
    M_ASSERT_EQ( complex_data["users"][0]["profile"]["preferences"]["theme"].to<json::String>(), "dark" );
    M_ASSERT_EQ( complex_data["users"][0]["profile"]["preferences"]["notifications"].to<json::Bool>(), true );
    M_ASSERT_EQ( complex_data["users"][1]["profile"]["preferences"]["theme"].to<json::String>(), "light" );
    M_ASSERT_EQ( complex_data["users"][1]["last_login"].type(), json::Type::eNull );
    
    // Test statistics access
    M_ASSERT_EQ( complex_data["statistics"]["total_users"].to<json::Number>(), 2 );
    M_ASSERT_EQ( complex_data["statistics"]["performance"]["response_times"].size(), 5 );
    M_ASSERT_EQ( complex_data["statistics"]["performance"]["response_times"][0].to<json::Number>(), 120 );
    M_ASSERT_EQ( complex_data["statistics"]["performance"]["uptime"].to<json::Number>(), 99.95 );
    
    // Test deep access through at() method
    M_ASSERT_NO_THROW( std::ignore = complex_data.at("application").at("config").at("database").at("connection").at("pool").at("min") );
    M_ASSERT_EQ( complex_data.at("application").at("config").at("database").at("connection").at("pool").at("min").to<json::Number>(), 5 );
    
    // Test array at() access
    M_ASSERT_NO_THROW( std::ignore = complex_data.at("users").at(0).at("permissions").at(1) );
    M_ASSERT_EQ( complex_data.at("users").at(0).at("permissions").at(1).to<json::String>(), "write" );
    
    // Test bounds checking with at()
    M_ASSERT_THROW( std::ignore = complex_data.at("nonexistent"), std::out_of_range );
    M_ASSERT_THROW( std::ignore = complex_data.at("users").at(0).at("permissions").at(10), std::out_of_range );
    M_ASSERT_THROW( std::ignore = complex_data.at("users").at(5), std::out_of_range );
    
    // Test deep modification through [] operator
    complex_data["application"]["config"]["port"] = 9090;
    M_ASSERT_EQ( complex_data["application"]["config"]["port"].to<json::Number>(), 9090 );
    
    complex_data["application"]["config"]["features"][0]["enabled"] = false;
    M_ASSERT_EQ( complex_data["application"]["config"]["features"][0]["enabled"].to<json::Bool>(), false );
    
    complex_data["users"][0]["profile"]["preferences"]["theme"] = "auto";
    M_ASSERT_EQ( complex_data["users"][0]["profile"]["preferences"]["theme"].to<json::String>(), "auto" );
    
    // Test deep modification through at() method
    complex_data.at("statistics").at("total_users") = 3;
    M_ASSERT_EQ( complex_data.at("statistics").at("total_users").to<json::Number>(), 3 );
    
    complex_data.at("users").at(1).at("active") = true;
    M_ASSERT_EQ( complex_data.at("users").at(1).at("active").to<json::Bool>(), true );
    
    // Test adding new nested elements
    complex_data["application"]["config"]["new_feature"] = json::Object{
        {"name", "monitoring"},
        {"enabled", true},
        {"settings", json::Array{{
            json::Object{{"metric", "cpu"}, {"threshold", 0.8}},
            json::Object{{"metric", "memory"}, {"threshold", 0.9}}
        }}}
    };
    
    M_ASSERT_TRUE( complex_data["application"]["config"].contains("new_feature") );
    M_ASSERT_EQ( complex_data["application"]["config"]["new_feature"]["name"].to<json::String>(), "monitoring" );
    M_ASSERT_EQ( complex_data["application"]["config"]["new_feature"]["settings"].size(), 2 );
    M_ASSERT_EQ( complex_data["application"]["config"]["new_feature"]["settings"][0]["metric"].to<json::String>(), "cpu" );
    
    // Test copy semantics
    json::Value complex_copy = complex_data;
    M_ASSERT_TRUE( complex_copy == complex_data );
    M_ASSERT_EQ( complex_copy.size(), complex_data.size() );
    
    // Modify copy and ensure original is unchanged
    complex_copy["application"]["name"] = "CopiedApp";
    M_ASSERT_EQ( complex_copy["application"]["name"].to<json::String>(), "CopiedApp" );
    M_ASSERT_EQ( complex_data["application"]["name"].to<json::String>(), "MyApp" );
    M_ASSERT_FALSE( complex_copy == complex_data );
    
    // Test move semantics for std::map, std::vector, std::string
    std::map<std::string, json::Value> test_map{
        {"key1", json::Number(100)},
        {"key2", json::String("value2")},
        {"key3", json::Bool(true)}
    };
    
    std::vector<json::Value> test_vector{
        json::Number(1),
        json::String("two"),
        json::Bool(false),
        json::Object{{"nested", "value"}}
    };
    
    std::string test_string = "movable_string";
    
    // Test move construction
    json::Value moved_object{std::move(test_map)};
    json::Value moved_array{std::move(test_vector)};
    json::Value moved_string{std::move(test_string)};
    
    M_ASSERT_EQ( moved_object.type(), json::Type::eObject );
    M_ASSERT_EQ( moved_object.size(), 3 );
    M_ASSERT_EQ( moved_object["key1"].to<json::Number>(), 100 );
    M_ASSERT_EQ( moved_object["key2"].to<json::String>(), "value2" );
    M_ASSERT_EQ( moved_object["key3"].to<json::Bool>(), true );
    
    M_ASSERT_EQ( moved_array.type(), json::Type::eArray );
    M_ASSERT_EQ( moved_array.size(), 4 );
    M_ASSERT_EQ( moved_array[0].to<json::Number>(), 1 );
    M_ASSERT_EQ( moved_array[1].to<json::String>(), "two" );
    M_ASSERT_EQ( moved_array[2].to<json::Bool>(), false );
    M_ASSERT_EQ( moved_array[3]["nested"].to<json::String>(), "value" );
    
    M_ASSERT_EQ( moved_string.type(), json::Type::eString );
    M_ASSERT_EQ( moved_string.to<json::String>(), "movable_string" );
    
    // Test move assignment
    json::Value move_target{json::Type::eNull};
    
    std::map<std::string, json::Value> another_map{{"move_key", json::Number(999)}};
    move_target = std::move(another_map);
    
    M_ASSERT_EQ( move_target.type(), json::Type::eObject );
    M_ASSERT_EQ( move_target.size(), 1 );
    M_ASSERT_EQ( move_target["move_key"].to<json::Number>(), 999 );
    
    // Test complex structure with move semantics
    std::vector<json::Value> complex_array{
        json::Object{
            {"item1", json::Array{{1, 2, 3}}},
            {"item2", json::Object{{"nested", true}}}
        },
        json::Array{{
            json::String("first"),
            42,
            json::Object{{"deep", json::Array{{"very", "deep", "nesting"}}}}
        }}
    };
    
    json::Value moved_complex{std::move(complex_array)};
    M_ASSERT_EQ( moved_complex.type(), json::Type::eArray );
    M_ASSERT_EQ( moved_complex.size(), 2 );
    M_ASSERT_EQ( moved_complex[0]["item1"].size(), 3 );
    M_ASSERT_EQ( moved_complex[0]["item1"][0].to<json::Number>(), 1 );
    M_ASSERT_EQ( moved_complex[0]["item2"]["nested"].to<json::Bool>(), true );
    M_ASSERT_EQ( moved_complex[1][0].to<json::String>(), "first" );
    M_ASSERT_EQ( moved_complex[1][2]["deep"][0].to<json::String>(), "very" );
    
    // Test get<T>() with complex structures (returns copies)
    auto complex_obj_copy = complex_data.to<json::Object>();
    M_ASSERT_EQ( complex_obj_copy.size(), 3 );
    M_ASSERT_TRUE( complex_obj_copy.contains("application") );
    
    auto users_array_copy = complex_data["users"].to<json::Array>();
    M_ASSERT_EQ( users_array_copy.size(), 2 );
    M_ASSERT_EQ( users_array_copy[0]["name"].to<json::String>(), "John Doe" );
    
    auto features_array_copy = complex_data["application"]["config"]["features"].to<json::Array>();
    M_ASSERT_EQ( features_array_copy.size(), 3 );
    M_ASSERT_EQ( features_array_copy[0]["name"].to<json::String>(), "auth" );
    
    // Test get_ref<T>() with complex structures (returns references)
    auto& complex_obj_ref = complex_data.get<json::Object>();
    M_ASSERT_EQ( complex_obj_ref.size(), 3 );
    M_ASSERT_TRUE( complex_obj_ref.contains("application") );
    
    auto& users_array_ref = complex_data["users"].get<json::Array>();
    M_ASSERT_EQ( users_array_ref.size(), 2 );
    M_ASSERT_EQ( users_array_ref[0]["name"].to<json::String>(), "John Doe" );
    
    auto& features_array_ref = complex_data["application"]["config"]["features"].get<json::Array>();
    M_ASSERT_EQ( features_array_ref.size(), 3 );
    M_ASSERT_EQ( features_array_ref[0]["name"].to<json::String>(), "auth" );
    
    // Test get_ref<T>() with modifications
    auto& stats_ref = complex_data["statistics"].get<json::Object>();
    stats_ref["new_metric"] = json::Number(123.45);
    M_ASSERT_TRUE( complex_data["statistics"].contains("new_metric") );
    M_ASSERT_EQ( complex_data["statistics"]["new_metric"].to<json::Number>(), 123.45 );
    
    auto& response_times_ref = complex_data["statistics"]["performance"]["response_times"].get<json::Array>();
    response_times_ref.push_back(json::Number(75));
    M_ASSERT_EQ( complex_data["statistics"]["performance"]["response_times"].size(), 6 );
    M_ASSERT_EQ( complex_data["statistics"]["performance"]["response_times"][5].to<json::Number>(), 75 );
    
    // Test size() method on nested structures
    M_ASSERT_EQ( complex_data.size(), 3 );
    M_ASSERT_EQ( complex_data["application"].size(), 4 );
    M_ASSERT_EQ( complex_data["application"]["config"].size(), 6 );  // debug, port, host, features, database, new_feature
    M_ASSERT_EQ( complex_data["application"]["config"]["features"].size(), 3 );
    M_ASSERT_EQ( complex_data["users"].size(), 2 );
    M_ASSERT_EQ( complex_data["users"][0].size(), 8 );
    M_ASSERT_EQ( complex_data["users"][0]["permissions"].size(), 3 );
    M_ASSERT_EQ( complex_data["users"][0]["profile"].size(), 3 );
    M_ASSERT_EQ( complex_data["users"][0]["profile"]["preferences"].size(), 4 );
    M_ASSERT_EQ( complex_data["statistics"].size(), 6 );  // total_users, active_users, avg_login_time, performance, features_usage, new_metric
    M_ASSERT_EQ( complex_data["statistics"]["performance"].size(), 4 );
    M_ASSERT_EQ( complex_data["statistics"]["performance"]["response_times"].size(), 6 );
    M_ASSERT_EQ( complex_data["statistics"]["features_usage"].size(), 3 );
    
    // Test equality comparison with complex structures
    json::Value complex_clone{json::Object{
        {"application", json::Object{
            {"name", "MyApp"},
            {"version", "2.1.0"},
            {"active", true},
            {"config", json::Object{
                {"debug", false},
                {"port", 9090},  // Modified value
                {"host", "localhost"},
                {"features", json::Array{{
                    json::Object{{"name", "auth"}, {"enabled", false}, {"priority", 1}},  // Modified
                    json::Object{{"name", "logging"}, {"enabled", false}, {"priority", 2}},
                    json::Object{{"name", "cache"}, {"enabled", true}, {"priority", 3}}
                }}},
                {"database", json::Object{
                    {"type", "postgresql"},
                    {"connection", json::Object{
                        {"host", "db.example.com"},
                        {"port", 5432},
                        {"database", "myapp_db"},
                        {"ssl", true},
                        {"pool", json::Object{
                            {"min", 5},
                            {"max", 20},
                            {"idle_timeout", 30000},
                            {"connection_timeout", nullptr}
                        }}
                    }},
                    {"migrations", json::Array{{
                        json::Object{{"version", "001"}, {"applied", true}, {"timestamp", 1640995200}},
                        json::Object{{"version", "002"}, {"applied", true}, {"timestamp", 1641081600}},
                        json::Object{{"version", "003"}, {"applied", false}, {"timestamp", nullptr}}
                    }}}
                }},
                {"new_feature", json::Object{  // Added feature
                    {"name", "monitoring"},
                    {"enabled", true},
                    {"settings", json::Array{{
                        json::Object{{"metric", "cpu"}, {"threshold", 0.8}},
                        json::Object{{"metric", "memory"}, {"threshold", 0.9}}
                    }}}
                }}
            }}
        }},
        {"users", json::Array{{
            json::Object{
                {"id", 1},
                {"name", "John Doe"},
                {"email", "john@example.com"},
                {"active", true},
                {"permissions", json::Array{{"read", "write", "admin"}}},
                {"profile", json::Object{
                    {"age", 30},
                    {"country", "USA"},
                    {"preferences", json::Object{
                        {"theme", "auto"},  // Modified
                        {"notifications", true},
                        {"language", "en"},
                        {"timezone", "UTC-5"}
                    }}
                }},
                {"last_login", 1640995200},
                {"metadata", nullptr}
            }},
            json::Object{
                {"id", 2},
                {"name", "Jane Smith"},
                {"email", "jane@example.com"},
                {"active", true},  // Modified
                {"permissions", json::Array{{"read"}}},
                {"profile", json::Object{
                    {"age", 25},
                    {"country", "Canada"},
                    {"preferences", json::Object{
                        {"theme", "light"},
                        {"notifications", false},
                        {"language", "fr"},
                        {"timezone", "UTC-4"}
                    }}
                }},
                {"last_login", nullptr},
                {"metadata", json::Object{{"notes", "Inactive user"}, {"created", 1640908800}}}
            }
        }},
        {"statistics", json::Object{
            {"total_users", 3},  // Modified
            {"active_users", 1},
            {"avg_login_time", 1640995200},
            {"performance", json::Object{
                {"response_times", json::Array{{120, 85, 95, 110, 88, 75}}},  // Added element
                {"error_rates", json::Array{{0.01, 0.02, 0.015, 0.008, 0.012}}},
                {"uptime", 99.95},
                {"last_restart", nullptr}
            }},
            {"features_usage", json::Array{{
                json::Object{{"feature", "auth"}, {"usage_count", 1500}, {"success_rate", 0.98}},
                json::Object{{"feature", "logging"}, {"usage_count", 0}, {"success_rate", nullptr}},
                json::Object{{"feature", "cache"}, {"usage_count", 800}, {"success_rate", 0.95}}
            }}},
            {"new_metric", 123.45}  // Added metric
        }}
    }};
    
    M_ASSERT_TRUE( complex_data == complex_clone );
    
    // Test inequality by changing one deep value
    complex_clone["users"][0]["profile"]["preferences"]["theme"] = "different";
    M_ASSERT_FALSE( complex_data == complex_clone );
    
    // Test serialization of complex structure
    M_ASSERT_NO_THROW( std::ignore = complex_data.serialize() );
    auto serialized = complex_data.serialize();
    M_ASSERT_TRUE( serialized.size() > 1000 );  // Should be quite large
    
    // Test pretty serialization
    M_ASSERT_NO_THROW( std::ignore = complex_data.serialize_pretty() );
    auto pretty_serialized = complex_data.serialize_pretty();
    if (pretty_serialized.has_value()) {
        M_ASSERT_TRUE( pretty_serialized->size() > serialized.size() );
    }
    
    // Test round-trip parsing
    auto parsed_complex = json::parse(serialized);
    if (parsed_complex.has_value()) {
        M_ASSERT_TRUE( *parsed_complex == complex_data );
        
        // Test deep access on parsed data
        M_ASSERT_EQ( (*parsed_complex)["application"]["config"]["port"].to<json::Number>(), 9090 );
        M_ASSERT_EQ( (*parsed_complex)["users"][0]["profile"]["preferences"]["theme"].to<json::String>(), "auto" );
        M_ASSERT_EQ( (*parsed_complex)["statistics"]["performance"]["response_times"].size(), 6 );
        M_ASSERT_EQ( (*parsed_complex)["statistics"]["new_metric"].to<json::Number>(), 123.45 );
    } else {
        M_ASSERT_FAIL("Failed to parse complex structure");
    }
    
    // Test type validation throughout complex structure
    M_ASSERT_EQ( complex_data.type(), json::Type::eObject );
    M_ASSERT_EQ( complex_data["application"].type(), json::Type::eObject );
    M_ASSERT_EQ( complex_data["application"]["name"].type(), json::Type::eString );
    M_ASSERT_EQ( complex_data["application"]["active"].type(), json::Type::eBool );
    M_ASSERT_EQ( complex_data["application"]["config"]["port"].type(), json::Type::eNumber );
    M_ASSERT_EQ( complex_data["application"]["config"]["features"].type(), json::Type::eArray );
    M_ASSERT_EQ( complex_data["application"]["config"]["database"]["connection"]["pool"]["connection_timeout"].type(), json::Type::eNull );
    M_ASSERT_EQ( complex_data["users"].type(), json::Type::eArray );
    M_ASSERT_EQ( complex_data["users"][0].type(), json::Type::eObject );
    M_ASSERT_EQ( complex_data["users"][0]["permissions"].type(), json::Type::eArray );
    M_ASSERT_EQ( complex_data["users"][0]["permissions"][0].type(), json::Type::eString );
    M_ASSERT_EQ( complex_data["users"][1]["last_login"].type(), json::Type::eNull );
    M_ASSERT_EQ( complex_data["statistics"]["performance"]["uptime"].type(), json::Type::eNumber );
    
    // Test contains() method on various levels
    M_ASSERT_TRUE( complex_data.contains("application") );
    M_ASSERT_TRUE( complex_data.contains("users") );
    M_ASSERT_TRUE( complex_data.contains("statistics") );
    M_ASSERT_FALSE( complex_data.contains("nonexistent") );
    
    M_ASSERT_TRUE( complex_data["application"].contains("config") );
    M_ASSERT_TRUE( complex_data["application"]["config"].contains("database") );
    M_ASSERT_TRUE( complex_data["application"]["config"]["database"].contains("connection") );
    M_ASSERT_FALSE( complex_data["application"]["config"]["database"].contains("nonexistent") );
    
    M_ASSERT_TRUE( complex_data["users"][0].contains("profile") );
    M_ASSERT_TRUE( complex_data["users"][0]["profile"].contains("preferences") );
    M_ASSERT_FALSE( complex_data["users"][0]["profile"].contains("nonexistent") );
    
    // Test performance with large nested structures
    json::Value large_nested{json::Object{}};
    for (int i = 0; i < 10; ++i) {
        std::string section_name = "section_" + std::to_string(i);
        large_nested[section_name] = json::Object{};
        
        for (int j = 0; j < 10; ++j) {
            std::string subsection_name = "subsection_" + std::to_string(j);
            large_nested[section_name][subsection_name] = json::Array{};

            for (int k = 0; k < 10; ++k) {
                large_nested[section_name][subsection_name].get<json::Array>().push_back(
                    json::Object{
                        {"id", k},
                        {"name", "item_" + std::to_string(k)},
                        {"active", k % 2 == 0},
                        {"value", k * 10.5},
                        {"metadata", (k % 3 == 0) ? json::Value{nullptr} : json::Value{json::Object{{"info", "data"}}}}
                    }
                );
            }
        }
    }
    
    // Test access to large nested structure
    M_ASSERT_EQ( large_nested.size(), 10 );
    M_ASSERT_EQ( large_nested["section_5"]["subsection_7"].size(), 10 );
    M_ASSERT_EQ( large_nested["section_5"]["subsection_7"][8]["id"].to<json::Number>(), 8 );
    M_ASSERT_EQ( large_nested["section_5"]["subsection_7"][8]["name"].to<json::String>(), "item_8" );
    M_ASSERT_EQ( large_nested["section_5"]["subsection_7"][8]["active"].to<json::Bool>(), true );
    M_ASSERT_EQ( large_nested["section_5"]["subsection_7"][8]["value"].to<json::Number>(), 84.0 );
    M_ASSERT_EQ( large_nested["section_5"]["subsection_7"][9]["metadata"].type(), json::Type::eNull );
    M_ASSERT_EQ( large_nested["section_5"]["subsection_7"][7]["metadata"]["info"].to<json::String>(), "data" );
    
    // Test serialization performance on large structure
    M_ASSERT_NO_THROW( std::ignore = large_nested.serialize() );
    auto large_serialized = large_nested.serialize();
    M_ASSERT_TRUE( large_serialized.size() > 10000 );
    
    // Test final validation of all modifications
    M_ASSERT_EQ( complex_data["application"]["config"]["port"].to<json::Number>(), 9090 );
    M_ASSERT_EQ( complex_data["application"]["config"]["features"][0]["enabled"].to<json::Bool>(), false );
    M_ASSERT_EQ( complex_data["users"][0]["profile"]["preferences"]["theme"].to<json::String>(), "auto" );
    M_ASSERT_EQ( complex_data["statistics"]["total_users"].to<json::Number>(), 3 );
    M_ASSERT_EQ( complex_data["users"][1]["active"].to<json::Bool>(), true );
    M_ASSERT_TRUE( complex_data["statistics"].contains("new_metric") );
    M_ASSERT_EQ( complex_data["statistics"]["new_metric"].to<json::Number>(), 123.45 );
    M_ASSERT_EQ( complex_data["statistics"]["performance"]["response_times"].size(), 6 );
}
