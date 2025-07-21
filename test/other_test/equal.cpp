#include <vct/test_unit_macros.hpp>

import std;
import vct.test.unit;
import vct.tools.json;

using namespace vct::tools;

// --- Null type tests ---
M_TEST(Equal, Null) {
    json::Value null_val{};
    json::Value another_null{nullptr};
    M_ASSERT_TRUE(null_val == nullptr);
    M_ASSERT_TRUE(another_null == nullptr);
    M_ASSERT_TRUE(null_val == another_null);
    M_ASSERT_FALSE(null_val == 0);
    M_ASSERT_FALSE(null_val == "");
    M_ASSERT_FALSE(null_val == false);
    M_ASSERT_FALSE(null_val == std::string("null"));
}

// --- Boolean equality tests ---
// Covers strict type comparison. No implicit conversion between bool and number.
M_TEST(Equal, Bool) {
    json::Value true_val{json::Bool(true)};
    json::Value false_val{json::Bool(false)};
    M_ASSERT_TRUE(true_val == true);      // True matches true
    M_ASSERT_FALSE(true_val == false);    // True does not match false
    M_ASSERT_TRUE(false_val == false);    // False matches false
    M_ASSERT_FALSE(false_val == true);    // False does not match true
    M_ASSERT_FALSE(true_val == false_val);// Different bool values
    M_ASSERT_TRUE(true_val == true_val);  // Reflexivity
    M_ASSERT_TRUE(false_val == false_val);// Reflexivity

    // --- No implicit conversion between bool and number ---
    // All comparisons between bool and number must be false.
    M_ASSERT_FALSE(true_val == 1);         // true != 1
    M_ASSERT_FALSE(true_val == 1.0);       // true != 1.0
    M_ASSERT_FALSE(true_val == 0);         // true != 0
    M_ASSERT_FALSE(true_val == 2);         // true != 2
    M_ASSERT_FALSE(false_val == 0);        // false != 0
    M_ASSERT_FALSE(false_val == 0.0);      // false != 0.0
    M_ASSERT_FALSE(false_val == 1);        // false != 1
    M_ASSERT_FALSE(false_val == -1);       // false != -1

    // Reverse comparison: number == bool
    M_ASSERT_FALSE(1 == true_val);
    M_ASSERT_FALSE(1.0 == true_val);
    M_ASSERT_FALSE(0 == true_val);
    M_ASSERT_FALSE(2 == true_val);
    M_ASSERT_FALSE(0 == false_val);
    M_ASSERT_FALSE(0.0 == false_val);
    M_ASSERT_FALSE(1 == false_val);
    M_ASSERT_FALSE(-1 == false_val);
}

// --- Number type tests ---
M_TEST(Equal, Number) {
    json::Value num_val{json::Number(42)};
    M_ASSERT_TRUE(num_val == 42);
    M_ASSERT_TRUE(num_val == 42.0);
    M_ASSERT_FALSE(num_val == 43);
    M_ASSERT_FALSE(num_val == 41);

    json::Value float_val{json::Number(3.14)};
    M_ASSERT_TRUE(float_val == 3.14);
    M_ASSERT_TRUE(float_val == 3);  // 3.14 rounds to 3
    M_ASSERT_FALSE(float_val == 4);
    M_ASSERT_FALSE(float_val == 2);

    json::Value negative_val{json::Number(-100)};
    M_ASSERT_TRUE(negative_val == -100);
    M_ASSERT_FALSE(negative_val == 100);
    M_ASSERT_FALSE(negative_val == -99);

    // Boundary numbers
    json::Value max_val{std::numeric_limits<double>::max()};
    json::Value min_val{std::numeric_limits<double>::lowest()};
    M_ASSERT_TRUE(max_val == std::numeric_limits<double>::max());
    M_ASSERT_TRUE(min_val == std::numeric_limits<double>::lowest());
    M_ASSERT_FALSE(max_val == min_val);

    // No implicit conversion with bool
    M_ASSERT_FALSE(num_val == true);
    M_ASSERT_FALSE(num_val == false);

    // No implicit conversion with bool for number values
    json::Value num_true{json::Number(1)};
    json::Value num_false{json::Number(0)};
    M_ASSERT_FALSE(num_true == true);
    M_ASSERT_FALSE(num_false == false);
    M_ASSERT_FALSE(num_true == false);
    M_ASSERT_FALSE(num_false == true);
}

// --- String type tests ---
M_TEST(Equal, String) {
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

    // 特殊字符
    json::Value unicode_val{json::String("你好世界")};
    M_ASSERT_TRUE(unicode_val == "你好世界");
    M_ASSERT_TRUE(unicode_val == std::string("你好世界"));
    M_ASSERT_FALSE(unicode_val == "Hello World");

    json::Value special_str{json::String("Hello\nWorld\t!")};
    M_ASSERT_TRUE(special_str == "Hello\nWorld\t!");
    M_ASSERT_TRUE(special_str == std::string("Hello\nWorld\t!"));
    M_ASSERT_FALSE(special_str == "Hello World !");
}

// --- Array type tests ---
M_TEST(Equal, Array) {
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

    // 不允许与原生类型比较
    M_ASSERT_FALSE(arr1 == 123);
    M_ASSERT_FALSE(arr1 == "array");
    M_ASSERT_FALSE(arr1 == true);
    M_ASSERT_FALSE(arr1 == nullptr);
    M_ASSERT_FALSE(empty_arr1 == 0);
    M_ASSERT_FALSE(empty_arr1 == "");
    M_ASSERT_FALSE(empty_arr1 == false);
}

// --- Object type tests ---
M_TEST(Equal, Object) {
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

    // 不允许与原生类型比较
    M_ASSERT_FALSE(obj1 == 42);
    M_ASSERT_FALSE(obj1 == "object");
    M_ASSERT_FALSE(obj1 == true);
    M_ASSERT_FALSE(obj1 == nullptr);
    M_ASSERT_FALSE(empty_obj1 == 0);
    M_ASSERT_FALSE(empty_obj1 == "");
    M_ASSERT_FALSE(empty_obj1 == false);
}

// --- Mixed type and nested structure tests ---
M_TEST(Equal, MixedAndNested) {
    // 混合数组
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
        json::Bool(false),
        json::Value{nullptr},
        json::Array{1, 2},
        json::Object{{"key", "value"}}
    }};
    M_ASSERT_TRUE(mixed1 == mixed2);
    M_ASSERT_FALSE(mixed1 == mixed3);

    // 嵌套对象
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
    json::Value nested2 = nested1;
    json::Value nested3{json::Object{
        {"user", json::Object{
            {"name", "Alice"},
            {"profile", json::Object{
                {"age", 26},
                {"active", true}
            }}
        }},
        {"data", json::Array{1, 2, 3}}
    }};
    M_ASSERT_TRUE(nested1 == nested2);
    M_ASSERT_FALSE(nested1 == nested3);
}

// --- Strict type and edge case tests ---
M_TEST(Equal, StrictTypeAndEdge) {
    // No implicit conversion between bool and number
    json::Value num_one{json::Number(1)};
    json::Value bool_true{json::Bool(true)};
    M_ASSERT_FALSE(num_one == bool_true);
    M_ASSERT_FALSE(bool_true == num_one);
    json::Value num_zero{json::Number(0)};
    json::Value bool_false{json::Bool(false)};
    M_ASSERT_FALSE(num_zero == bool_false);
    M_ASSERT_FALSE(bool_false == num_zero);

    // Strict type comparison between different types
    json::Value str_val{json::String("test")};
    json::Value arr_val{json::Array{1, 2, 3}};
    json::Value obj_val{json::Object{{"key", "value"}}};
    json::Value null_val{nullptr};

    M_ASSERT_FALSE(str_val == arr_val);      // String vs Array
    M_ASSERT_FALSE(arr_val == obj_val);      // Array vs Object
    M_ASSERT_FALSE(obj_val == null_val);     // Object vs Null

    // Reverse comparison
    M_ASSERT_TRUE(42 == json::Value{42});
    M_ASSERT_TRUE("hello" == json::Value{"hello"});
    M_ASSERT_TRUE(true == json::Value{true});
    M_ASSERT_FALSE(1 == json::Value{true});
    M_ASSERT_FALSE(0 == json::Value{false});
    M_ASSERT_FALSE(false == json::Value{0});
    M_ASSERT_FALSE(true == json::Value{1});
}

// --- Symmetry and reflexivity tests ---
M_TEST(Equal, SymmetryAndReflexivity) {
    json::Value val{json::Number(42)};
    M_ASSERT_TRUE(val == 42);
    M_ASSERT_TRUE(42 == val);

    json::Value str_val{json::String("hello")};
    M_ASSERT_TRUE(str_val == "hello");
    M_ASSERT_TRUE("hello" == str_val);

    json::Value bool_val{json::Bool(true)};
    M_ASSERT_TRUE(bool_val == true);
    M_ASSERT_TRUE(true == bool_val);
    M_ASSERT_FALSE(bool_val == 1);
    M_ASSERT_FALSE(1 == bool_val);

    json::Value false_val{json::Bool(false)};
    M_ASSERT_TRUE(false_val == false);
    M_ASSERT_TRUE(false == false_val);
    M_ASSERT_FALSE(false_val == 0);
    M_ASSERT_FALSE(0 == false_val);

    // Reflexivity
    M_ASSERT_TRUE(val == val);
    M_ASSERT_TRUE(str_val == str_val);
    M_ASSERT_TRUE(bool_val == bool_val);
    M_ASSERT_TRUE(false_val == false_val);

    // No implicit conversion between bool and number
    json::Value bool_true{json::Bool(true)};
    json::Value bool_false{json::Bool(false)};
    M_ASSERT_FALSE(bool_true == 1);
    M_ASSERT_FALSE(1 == bool_true);
    M_ASSERT_FALSE(bool_false == 0);
    M_ASSERT_FALSE(0 == bool_false);
    M_ASSERT_FALSE(bool_true == 0);
    M_ASSERT_FALSE(0 == bool_true);
    M_ASSERT_FALSE(bool_false == 1);
    M_ASSERT_FALSE(1 == bool_false);
}

// --- Transitivity tests ---
M_TEST(Equal, Transitivity) {
    json::Value val1{static_cast<json::Number>(42)};
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
