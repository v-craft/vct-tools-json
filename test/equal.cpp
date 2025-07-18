#include <vct/test_unit_macros.hpp>

import std;
import vct.test.unit;
import vct.tools.json;


using namespace vct::tools;

M_TEST(Equal, All) {
    // Test json::Value == json::Value comparisons
    {
        json::Value val1{json::Number(42)};
        json::Value val2{json::Number(42)};
        json::Value val3{json::Number(43)};
        
        M_ASSERT_TRUE(val1 == val2);
        M_ASSERT_FALSE(val1 == val3);
        M_ASSERT_FALSE(val2 == val3);
    }
    
    // Test json::Value == primitive types
    {
        // Number comparisons
        json::Value num_val{json::Number(42)};
        M_ASSERT_TRUE(num_val == 42);
        M_ASSERT_TRUE(num_val == 42.0);
        M_ASSERT_FALSE(num_val == 43);
        M_ASSERT_FALSE(num_val == 41);
        
        json::Value float_val{json::Number(3.14)};
        M_ASSERT_TRUE(float_val == 3.14);
        M_ASSERT_TRUE(float_val == 3);  // 3.14 rounds to 3
        M_ASSERT_FALSE(float_val == 4);  // 3.14 doesn't round to 4
        M_ASSERT_FALSE(float_val == 2);  // 3.14 doesn't round to 2
        
        json::Value negative_val{json::Number(-100)};
        M_ASSERT_TRUE(negative_val == -100);
        M_ASSERT_FALSE(negative_val == 100);
        M_ASSERT_FALSE(negative_val == -99);
        
        // Test edge cases for rounding
        json::Value round_up{json::Number(2.5)};
        M_ASSERT_TRUE(round_up == 2.5);
        M_ASSERT_TRUE(round_up == 3);  // 2.5 rounds up to 3
        M_ASSERT_FALSE(round_up == 2);
        
        json::Value round_down{json::Number(2.4)};
        M_ASSERT_TRUE(round_down == 2.4);
        M_ASSERT_TRUE(round_down == 2);  // 2.4 rounds down to 2
        M_ASSERT_FALSE(round_down == 3);
        
        json::Value negative_round{json::Number(-2.7)};
        M_ASSERT_TRUE(negative_round == -2.7);
        M_ASSERT_TRUE(negative_round == -3);  // -2.7 rounds to -3
        M_ASSERT_FALSE(negative_round == -2);
    }
    
    {
        // String comparisons
        json::Value str_val{json::String("hello")};
        M_ASSERT_TRUE(str_val == "hello");
        M_ASSERT_TRUE(str_val == std::string("hello"));
        M_ASSERT_FALSE(str_val == "Hello");
        M_ASSERT_FALSE(str_val == "world");
        M_ASSERT_FALSE(str_val == std::string("HELLO"));
        
        json::Value empty_str_val{json::String("")};
        M_ASSERT_TRUE(empty_str_val == "");
        M_ASSERT_TRUE(empty_str_val == std::string(""));
        M_ASSERT_FALSE(empty_str_val == " ");
        M_ASSERT_FALSE(empty_str_val == "empty");
        
        json::Value unicode_val{json::String("你好世界")};
        M_ASSERT_TRUE(unicode_val == "你好世界");
        M_ASSERT_TRUE(unicode_val == std::string("你好世界"));
        M_ASSERT_FALSE(unicode_val == "Hello World");
    }
    
    {
        // Boolean comparisons
        json::Value true_val{json::Bool(true)};
        json::Value false_val{json::Bool(false)};
        
        M_ASSERT_TRUE(true_val == true);
        M_ASSERT_FALSE(true_val == false);
        M_ASSERT_TRUE(false_val == false);
        M_ASSERT_FALSE(false_val == true);
        
        M_ASSERT_FALSE(true_val == false_val);
        M_ASSERT_TRUE(true_val == true_val);
        M_ASSERT_TRUE(false_val == false_val);
        
        // Boolean to numeric conversion
        M_ASSERT_TRUE(true_val == 1);    // true converts to 1
        M_ASSERT_TRUE(true_val == 1.0);  // true converts to 1.0
        M_ASSERT_FALSE(true_val == 0);   // true doesn't equal 0
        M_ASSERT_FALSE(true_val == 2);   // true doesn't equal 2
        
        M_ASSERT_TRUE(false_val == 0);   // false converts to 0
        M_ASSERT_TRUE(false_val == 0.0); // false converts to 0.0
        M_ASSERT_FALSE(false_val == 1);  // false doesn't equal 1
        M_ASSERT_FALSE(false_val == -1); // false doesn't equal -1
    }
    
    {
        // Null comparisons
        json::Value null_val{json::Type::eNull};
        json::Value another_null{nullptr};
        
        M_ASSERT_TRUE(null_val == nullptr);
        M_ASSERT_TRUE(another_null == nullptr);
        M_ASSERT_TRUE(null_val == another_null);
        
        // Null should not equal any other type
        M_ASSERT_FALSE(null_val == 0);
        M_ASSERT_FALSE(null_val == "");
        M_ASSERT_FALSE(null_val == false);
        M_ASSERT_FALSE(null_val == std::string("null"));
    }
    
    // Test numeric-boolean conversions
    {
        json::Value num_zero{json::Number(0)};
        json::Value num_one{json::Number(1)};
        json::Value num_two{json::Number(2)};
        json::Value bool_true{json::Bool(true)};
        json::Value bool_false{json::Bool(false)};
        
        // Test number to boolean conversion
        M_ASSERT_TRUE(num_zero == false);   // 0 converts to false
        M_ASSERT_TRUE(num_one == true);     // 1 converts to true
        M_ASSERT_TRUE(num_two == true);    // 2 convert to true
        M_ASSERT_FALSE(num_two == false);   // 2 doesn't convert to false
        
        // Test boolean to number conversion
        M_ASSERT_TRUE(bool_true == 1);      // true converts to 1
        M_ASSERT_TRUE(bool_false == 0);     // false converts to 0
        M_ASSERT_FALSE(bool_true == 0);     // true doesn't equal 0
        M_ASSERT_FALSE(bool_false == 1);    // false doesn't equal 1
        
        // Test float to boolean conversion
        json::Value float_zero{json::Number(0.0)};
        json::Value float_one{json::Number(1.0)};
        json::Value float_nonzero{json::Number(0.5)};
        
        M_ASSERT_TRUE(float_zero == false);  // 0.0 converts to false
        M_ASSERT_TRUE(float_one == true);    // 1.0 converts to true
        M_ASSERT_FALSE(float_nonzero == false); // 0.5 rounds to 1, so not false
        M_ASSERT_TRUE(float_nonzero == true);   // 0.5 rounds to 1, so true
    }
    
    // Test cross-type comparisons (should be false)
    {
        json::Value num_val{json::Number(42)};
        json::Value str_val{json::String("42")};
        json::Value bool_val{json::Bool(true)};
        json::Value null_val{nullptr};
        
        // Number vs other types
        M_ASSERT_FALSE(num_val == "42");
        M_ASSERT_FALSE(num_val == std::string("42"));
        M_ASSERT_FALSE(num_val == nullptr);
        
        // String vs other types
        M_ASSERT_FALSE(str_val == 42);
        M_ASSERT_FALSE(str_val == true);
        M_ASSERT_FALSE(str_val == false);
        M_ASSERT_FALSE(str_val == nullptr);
        
        // Bool vs other types
        M_ASSERT_TRUE(bool_val == 1);   // true converts to 1
        M_ASSERT_FALSE(bool_val == 0);  // true doesn't equal 0
        M_ASSERT_FALSE(bool_val == 2);  // true doesn't equal 2
        M_ASSERT_FALSE(bool_val == "true");
        M_ASSERT_FALSE(bool_val == std::string("true"));
        M_ASSERT_FALSE(bool_val == nullptr);
        
        // Test false boolean conversions
        json::Value false_bool{json::Bool(false)};
        M_ASSERT_TRUE(false_bool == 0);   // false converts to 0
        M_ASSERT_FALSE(false_bool == 1);  // false doesn't equal 1
        M_ASSERT_FALSE(false_bool == -1); // false doesn't equal -1
        
        // Null vs other types (test if null converts to anything)
        M_ASSERT_FALSE(null_val == 1);   // null should not equal 1
        M_ASSERT_FALSE(null_val == "");  // null should not equal empty string
        M_ASSERT_FALSE(null_val == true); // null should not equal true
        
        // Test if null equals 0 or false (common conversions)
        // These might be true depending on implementation
        // M_ASSERT_FALSE(null_val == 0);     // Uncomment if null != 0
        // M_ASSERT_FALSE(null_val == false); // Uncomment if null != false
    }
    
    // Test array comparisons
    {
        json::Value arr1{json::Array{1, 2, 3}};
        json::Value arr2{json::Array{1, 2, 3}};
        json::Value arr3{json::Array{1, 2, 4}};
        json::Value arr4{json::Array{1, 2}};
        json::Value empty_arr1{json::Array{}};
        json::Value empty_arr2{json::Array{}};
        
        M_ASSERT_TRUE(arr1 == arr2);
        M_ASSERT_FALSE(arr1 == arr3);
        M_ASSERT_FALSE(arr1 == arr4);
        M_ASSERT_FALSE(arr1 == empty_arr1);
        M_ASSERT_TRUE(empty_arr1 == empty_arr2);
        
        // Array should not equal primitive types
        M_ASSERT_FALSE(arr1 == 123);
        M_ASSERT_FALSE(arr1 == "array");
        M_ASSERT_FALSE(arr1 == true);
        M_ASSERT_FALSE(arr1 == nullptr);
        M_ASSERT_FALSE(empty_arr1 == 0);
        M_ASSERT_FALSE(empty_arr1 == "");
        M_ASSERT_FALSE(empty_arr1 == false);
    }
    
    // Test object comparisons
    {
        json::Value obj1{json::Object{{"name", "John"}, {"age", 30}}};
        json::Value obj2{json::Object{{"name", "John"}, {"age", 30}}};
        json::Value obj3{json::Object{{"name", "Jane"}, {"age", 30}}};
        json::Value obj4{json::Object{{"name", "John"}, {"age", 31}}};
        json::Value obj5{json::Object{{"name", "John"}}};
        json::Value empty_obj1{json::Object{}};
        json::Value empty_obj2{json::Object{}};
        
        M_ASSERT_TRUE(obj1 == obj2);
        M_ASSERT_FALSE(obj1 == obj3);
        M_ASSERT_FALSE(obj1 == obj4);
        M_ASSERT_FALSE(obj1 == obj5);
        M_ASSERT_FALSE(obj1 == empty_obj1);
        M_ASSERT_TRUE(empty_obj1 == empty_obj2);
        
        // Object should not equal primitive types
        M_ASSERT_FALSE(obj1 == 42);
        M_ASSERT_FALSE(obj1 == "object");
        M_ASSERT_FALSE(obj1 == true);
        M_ASSERT_FALSE(obj1 == nullptr);
        M_ASSERT_FALSE(empty_obj1 == 0);
        M_ASSERT_FALSE(empty_obj1 == "");
        M_ASSERT_FALSE(empty_obj1 == false);
    }
    
    // Test nested structure comparisons
    {
        json::Value nested1{json::Object{
            {"user", json::Object{
                {"name", "Alice"},
                {"profile", json::Object{
                    {"age", 25},
                    {"active", true}
                }}
            }},
            {"data", json::Array{1, 2, 3}}
        }};
        
        json::Value nested2{json::Object{
            {"user", json::Object{
                {"name", "Alice"},
                {"profile", json::Object{
                    {"age", 25},
                    {"active", true}
                }}
            }},
            {"data", json::Array{1, 2, 3}}
        }};
        
        json::Value nested3{json::Object{
            {"user", json::Object{
                {"name", "Alice"},
                {"profile", json::Object{
                    {"age", 26},  // Different age
                    {"active", true}
                }}
            }},
            {"data", json::Array{1, 2, 3}}
        }};
        
        json::Value nested4{json::Object{
            {"user", json::Object{
                {"name", "Alice"},
                {"profile", json::Object{
                    {"age", 25},
                    {"active", true}
                }}
            }},
            {"data", json::Array{1, 2, 4}}  // Different array
        }};
        
        M_ASSERT_TRUE(nested1 == nested2);
        M_ASSERT_FALSE(nested1 == nested3);
        M_ASSERT_FALSE(nested1 == nested4);
        M_ASSERT_FALSE(nested2 == nested3);
        M_ASSERT_FALSE(nested2 == nested4);
        M_ASSERT_FALSE(nested3 == nested4);
    }
    
    // Test mixed array comparisons
    {
        json::Value mixed1{json::Array{
            json::Number(42),
            json::String("hello"),
            json::Bool(true),
            json::Value{nullptr},
            json::Array{1, 2},
            json::Object{{"key", "value"}}
        }};
        
        json::Value mixed2{json::Array{
            json::Number(42),
            json::String("hello"),
            json::Bool(true),
            json::Value{nullptr},
            json::Array{1, 2},
            json::Object{{"key", "value"}}
        }};
        
        json::Value mixed3{json::Array{
            json::Number(42),
            json::String("hello"),
            json::Bool(false),  // Different bool
            json::Value{nullptr},
            json::Array{1, 2},
            json::Object{{"key", "value"}}
        }};
        
        M_ASSERT_TRUE(mixed1 == mixed2);
        M_ASSERT_FALSE(mixed1 == mixed3);
        M_ASSERT_FALSE(mixed2 == mixed3);
    }
    
    // Test complex nested object comparisons
    {
        json::Value complex1{json::Object{
            {"application", json::Object{
                {"name", "MyApp"},
                {"version", "1.0.0"},
                {"config", json::Object{
                    {"debug", false},
                    {"features", json::Array{"auth", "logging"}},
                    {"database", json::Object{
                        {"host", "localhost"},
                        {"port", 5432},
                        {"credentials", json::Object{
                            {"username", "admin"},
                            {"password", "secret"}
                        }}
                    }}
                }}
            }},
            {"users", json::Array{
                json::Object{
                    {"id", 1},
                    {"name", "John"},
                    {"active", true}
                },
                json::Object{
                    {"id", 2},
                    {"name", "Jane"},
                    {"active", false}
                }
            }}
        }};
        
        json::Value complex2{json::Object{
            {"application", json::Object{
                {"name", "MyApp"},
                {"version", "1.0.0"},
                {"config", json::Object{
                    {"debug", false},
                    {"features", json::Array{"auth", "logging"}},
                    {"database", json::Object{
                        {"host", "localhost"},
                        {"port", 5432},
                        {"credentials", json::Object{
                            {"username", "admin"},
                            {"password", "secret"}
                        }}
                    }}
                }}
            }},
            {"users", json::Array{
                json::Object{
                    {"id", 1},
                    {"name", "John"},
                    {"active", true}
                },
                json::Object{
                    {"id", 2},
                    {"name", "Jane"},
                    {"active", false}
                }
            }}
        }};
        
        json::Value complex3{json::Object{
            {"application", json::Object{
                {"name", "MyApp"},
                {"version", "1.0.1"},  // Different version
                {"config", json::Object{
                    {"debug", false},
                    {"features", json::Array{"auth", "logging"}},
                    {"database", json::Object{
                        {"host", "localhost"},
                        {"port", 5432},
                        {"credentials", json::Object{
                            {"username", "admin"},
                            {"password", "secret"}
                        }}
                    }}
                }}
            }},
            {"users", json::Array{
                json::Object{
                    {"id", 1},
                    {"name", "John"},
                    {"active", true}
                },
                json::Object{
                    {"id", 2},
                    {"name", "Jane"},
                    {"active", false}
                }
            }}
        }};
        
        M_ASSERT_TRUE(complex1 == complex2);
        M_ASSERT_FALSE(complex1 == complex3);
        M_ASSERT_FALSE(complex2 == complex3);
    }
    
    // Test string variations
    {
        json::Value str_val{json::String("test")};
        
        // Test different string types
        std::string std_str = "test";
        const char* c_str = "test";
        std::string_view str_view = "test";
        
        M_ASSERT_TRUE(str_val == std_str);
        M_ASSERT_TRUE(str_val == c_str);
        M_ASSERT_TRUE(str_val == str_view);
        M_ASSERT_TRUE(str_val == "test");
        
        // Test case sensitivity
        M_ASSERT_FALSE(str_val == "Test");
        M_ASSERT_FALSE(str_val == "TEST");
        M_ASSERT_FALSE(str_val == std::string("Test"));
        
        // Test special characters
        json::Value special_str{json::String("Hello\nWorld\t!")};
        M_ASSERT_TRUE(special_str == "Hello\nWorld\t!");
        M_ASSERT_TRUE(special_str == std::string("Hello\nWorld\t!"));
        M_ASSERT_FALSE(special_str == "Hello World !");
    }
    
    // Test numeric precision and rounding
    {
        json::Value precise_val{json::Number(3.14159265359)};
        M_ASSERT_TRUE(precise_val == 3.14159265359);
        M_ASSERT_TRUE(precise_val == 3);  // Should round to 3
        M_ASSERT_FALSE(precise_val == 4);
        
        json::Value zero_val{json::Number(0.0)};
        M_ASSERT_TRUE(zero_val == 0);
        M_ASSERT_TRUE(zero_val == 0.0);
        M_ASSERT_FALSE(zero_val == 1);
        
        json::Value negative_zero{json::Number(-0.0)};
        M_ASSERT_TRUE(negative_zero == 0.0);
        M_ASSERT_TRUE(negative_zero == -0.0);
        M_ASSERT_TRUE(negative_zero == zero_val);
        M_ASSERT_TRUE(negative_zero == 0);  // Should equal integer 0
        
        // Test specific rounding cases
        json::Value half_up{json::Number(0.5)};
        M_ASSERT_TRUE(half_up == 0.5);
        M_ASSERT_TRUE(half_up == 1);  // 0.5 rounds up to 1
        M_ASSERT_FALSE(half_up == 0);
        
        json::Value half_down{json::Number(0.4)};
        M_ASSERT_TRUE(half_down == 0.4);
        M_ASSERT_TRUE(half_down == 0);  // 0.4 rounds down to 0
        M_ASSERT_FALSE(half_down == 1);
        
        json::Value negative_half{json::Number(-0.5)};
        M_ASSERT_TRUE(negative_half == -0.5);
        M_ASSERT_TRUE(negative_half == -1);  // -0.5 rounds to -1
        M_ASSERT_FALSE(negative_half == 0);
    }
    
    // Test equality symmetry (a == b should equal b == a)
    {
        json::Value val{json::Number(42)};
        M_ASSERT_TRUE(val == 42);
        M_ASSERT_TRUE(42 == val);  // Test reverse comparison if supported
        
        json::Value str_val{json::String("hello")};
        M_ASSERT_TRUE(str_val == "hello");
        M_ASSERT_TRUE("hello" == str_val);  // Test reverse comparison if supported
        
        json::Value bool_val{json::Bool(true)};
        M_ASSERT_TRUE(bool_val == true);
        M_ASSERT_TRUE(true == bool_val);  // Test reverse comparison if supported
        M_ASSERT_TRUE(bool_val == 1);     // Test boolean to number conversion
        M_ASSERT_TRUE(1 == bool_val);     // Test reverse number to boolean conversion
        
        json::Value false_val{json::Bool(false)};
        M_ASSERT_TRUE(false_val == false);
        M_ASSERT_TRUE(false == false_val); // Test reverse comparison if supported
        M_ASSERT_TRUE(false_val == 0);     // Test boolean to number conversion
        M_ASSERT_TRUE(0 == false_val);     // Test reverse number to boolean conversion
    }
    
    // Test reflexivity (a == a should always be true)
    {
        json::Value val1{json::Number(42)};
        json::Value val2{json::String("hello")};
        json::Value val3{json::Bool(true)};
        json::Value val4{nullptr};
        json::Value val5{json::Array{1, 2, 3}};
        json::Value val6{json::Object{{"key", "value"}}};
        
        M_ASSERT_TRUE(val1 == val1);
        M_ASSERT_TRUE(val2 == val2);
        M_ASSERT_TRUE(val3 == val3);
        M_ASSERT_TRUE(val4 == val4);
        M_ASSERT_TRUE(val5 == val5);
        M_ASSERT_TRUE(val6 == val6);
    }
    
    // Test transitivity (if a == b and b == c, then a == c)
    {
        json::Value val1{json::Number(42)};
        json::Value val2{json::Number(42)};
        json::Value val3{json::Number(42)};
        
        M_ASSERT_TRUE(val1 == val2);
        M_ASSERT_TRUE(val2 == val3);
        M_ASSERT_TRUE(val1 == val3);
        
        json::Value str1{json::String("test")};
        json::Value str2{json::String("test")};
        json::Value str3{json::String("test")};
        
        M_ASSERT_TRUE(str1 == str2);
        M_ASSERT_TRUE(str2 == str3);
        M_ASSERT_TRUE(str1 == str3);
    }
}
