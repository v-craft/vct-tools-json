#include <vct/test_unit_macros.hpp>

import std;
import vct.test.unit;
import vct.tools.json;

using namespace vct::tools;

enum class MyEnum { A = 5, B = 6, C = -1 };

M_TEST(Value, To) {
    // Null
    json::Value v_null{nullptr};
    M_ASSERT_EQ(v_null.to<json::Null>(), nullptr);
    M_ASSERT_THROW(std::ignore = v_null.to<bool>(), std::runtime_error); // Null->Bool抛异常
    M_ASSERT_EQ(v_null.to_or<bool>(true), true); // Null->Bool不可转换，返回默认值true
    M_ASSERT_EQ(v_null.to_or<bool>(false), false); // Null->Bool不可转换，返回默认值false
    M_ASSERT_FALSE(v_null.to_if<bool>().has_value());
    M_ASSERT_TRUE(v_null.to_if<json::Null>().has_value());
    M_ASSERT_EQ(v_null.to_if<json::Null>().value(), nullptr);

    // Bool
    json::Value v_true{true};
    json::Value v_false{false};
    M_ASSERT_EQ(v_true.to<json::Bool>(), true);
    M_ASSERT_EQ(v_false.to<json::Bool>(), false);
    M_ASSERT_EQ(v_true.to_or<json::Bool>(false), true);
    M_ASSERT_EQ(v_false.to_or<json::Bool>(true), false);
    M_ASSERT_EQ(v_true.to_or<int>(-1), 1); // Bool->int可转换，true->1
    M_ASSERT_EQ(v_false.to_or<int>(-1), 0); // Bool->int可转换，false->0
    M_ASSERT_TRUE(v_true.to_if<int>().has_value());
    M_ASSERT_EQ(v_true.to_if<int>().value(), 1);
    M_ASSERT_EQ(v_false.to_if<int>().value(), 0);
    M_ASSERT_EQ(v_true.to_or<double>(-1.0), 1.0); // Bool->double可转换，返回1.0
    M_ASSERT_EQ(v_false.to_or<double>(-1.0), 0.0);
    M_ASSERT_EQ(v_true.to<json::Number>(), 1.0); // Bool->Number可转换
    M_ASSERT_EQ(v_false.to<json::Number>(), 0.0);
    M_ASSERT_EQ(v_true.to<double>(), 1.0);
    M_ASSERT_EQ(v_false.to<double>(), 0.0);
    M_ASSERT_EQ(v_true.to_or<json::String>("default"), "default"); // Bool->String不可转换
    M_ASSERT_FALSE(v_true.to_if<json::String>().has_value());

    // Number
    json::Value v_num{42.5};
    json::Value v_int{123};
    json::Value v_zero{0};
    json::Value v_neg{-7.8};
    json::Value v_big{1e100};
    M_ASSERT_EQ(v_num.to<json::Number>(), 42.5);
    M_ASSERT_EQ(v_int.to<json::Number>(), 123.0);
    M_ASSERT_EQ(v_zero.to<json::Number>(), 0.0);
    M_ASSERT_EQ(v_neg.to<json::Number>(), -7.8);
    M_ASSERT_EQ(v_big.to<json::Number>(), 1e100);
    M_ASSERT_EQ(v_num.to<int>(), 43); // 四舍五入
    M_ASSERT_EQ(v_neg.to<int>(), -8);
    M_ASSERT_EQ(v_num.to_or<int>(-1), 43);
    M_ASSERT_EQ(v_num.to_or<double>(-1.0), 42.5);
    M_ASSERT_EQ(v_num.to_or<MyEnum>(MyEnum::C), static_cast<MyEnum>(43)); // Number->enum转换，返回43对应的枚举值
    M_ASSERT_TRUE(v_num.to_if<MyEnum>().has_value()); // 42.5可转换为MyEnum，值为static_cast<MyEnum>(43)
    M_ASSERT_EQ(v_num.to_if<MyEnum>().value(), static_cast<MyEnum>(43));
    json::Value v_enum{5};
    M_ASSERT_EQ(v_enum.to<MyEnum>(), MyEnum::A);
    M_ASSERT_EQ(v_enum.to_or<MyEnum>(MyEnum::C), MyEnum::A);
    M_ASSERT_TRUE(v_enum.to_if<MyEnum>().has_value());
    M_ASSERT_EQ(v_enum.to_if<MyEnum>().value(), MyEnum::A);
    M_ASSERT_EQ(v_num.to_or<json::String>("default"), "default"); // Number->String不可转换
    M_ASSERT_FALSE(v_num.to_if<json::String>().has_value());

    // String
    json::Value v_str{"hello"};
    json::Value v_empty_str{""};
    M_ASSERT_EQ(v_str.to<json::String>(), "hello");
    M_ASSERT_EQ(v_empty_str.to<json::String>(), "");
    M_ASSERT_EQ(v_str.to_or<json::String>("default"), "hello");
    M_ASSERT_EQ(v_empty_str.to_or<json::String>("default"), "");
    M_ASSERT_EQ(v_str.to_or<int>(-99), -99); // String->int不可转换
    M_ASSERT_FALSE(v_str.to_if<int>().has_value());
    M_ASSERT_EQ(v_str.to_or<json::Bool>(false), false); // String->Bool不可转换
    M_ASSERT_FALSE(v_str.to_if<json::Bool>().has_value());

    // Array
    json::Value v_arr{json::Array{{1, 2, 3}}};
    json::Value v_empty_arr{json::Array{}};
    M_ASSERT_EQ(v_arr.to<json::Array>().size(), 3);
    M_ASSERT_EQ(v_empty_arr.to<json::Array>().size(), 0);
    M_ASSERT_EQ(v_arr.to_or<json::Array>(json::Array{{4,5}}).size(), 3);
    M_ASSERT_EQ(v_empty_arr.to_or<json::Array>(json::Array{{4,5}}).size(), 0);
    M_ASSERT_EQ(v_arr.to_or<json::Object>(json::Object{{"k",1}}).size(), 1); // Array->Object不可转换，返回默认
    M_ASSERT_FALSE(v_arr.to_if<json::Object>().has_value());
    M_ASSERT_EQ(v_arr.to_or<json::String>("default"), "default");
    M_ASSERT_FALSE(v_arr.to_if<json::String>().has_value());

    // Object
    json::Value v_obj{json::Object{{"k", 1}}};
    json::Value v_empty_obj{json::Object{}};
    M_ASSERT_EQ(v_obj.to<json::Object>().size(), 1);
    M_ASSERT_EQ(v_empty_obj.to<json::Object>().size(), 0);
    M_ASSERT_EQ(v_obj.to_or<json::Object>(json::Object{{"x",2}}).size(), 1);
    M_ASSERT_EQ(v_empty_obj.to_or<json::Object>(json::Object{{"x",2}}).size(), 0);
    M_ASSERT_EQ(v_obj.to_or<json::Array>(json::Array{{7,8}}).size(), 2); // Object->Array不可转换，返回默认
    M_ASSERT_FALSE(v_obj.to_if<json::Array>().has_value());
    M_ASSERT_EQ(v_obj.to_or<json::String>("default"), "default");
    M_ASSERT_FALSE(v_obj.to_if<json::String>().has_value());

    // Value->Value
    M_ASSERT_EQ(v_num.to<json::Value>(), v_num);
    M_ASSERT_EQ(v_num.to_or<json::Value>(json::Value{0}), v_num);
    auto opt_val = v_num.to_if<json::Value>();
    M_ASSERT_TRUE(opt_val.has_value());
    M_ASSERT_EQ(opt_val.value(), v_num);

    // Convertible类型（如std::string_view）
    M_ASSERT_EQ(v_str.to<std::string_view>(), std::string_view("hello"));
    M_ASSERT_EQ(v_str.to_or<std::string_view>("default"), std::string_view("hello"));
    auto opt_sv = v_str.to_if<std::string_view>();
    M_ASSERT_TRUE(opt_sv.has_value());
    M_ASSERT_EQ(opt_sv.value(), std::string_view("hello"));

    // 不兼容类型抛异常（to），返回默认（to_or），返回nullopt（to_if）
    M_ASSERT_THROW(std::ignore = v_str.to<int>(), std::runtime_error);
    M_ASSERT_EQ(v_str.to_or<int>(-99), -99);
    M_ASSERT_FALSE(v_str.to_if<int>().has_value());

    M_ASSERT_THROW(std::ignore = v_arr.to<json::Object>(), std::runtime_error);
    M_ASSERT_EQ(v_arr.to_or<json::Object>(json::Object{{"k",1}}).size(), 1);
    M_ASSERT_FALSE(v_arr.to_if<json::Object>().has_value());

    M_ASSERT_THROW(std::ignore = v_obj.to<json::Array>(), std::runtime_error);
    M_ASSERT_EQ(v_obj.to_or<json::Array>(json::Array{{1,2}}).size(), 2);
    M_ASSERT_FALSE(v_obj.to_if<json::Array>().has_value());

    // 边界值测试
    json::Value v_min{-1e308};
    json::Value v_max{1e308};
    M_ASSERT_EQ(v_min.to<double>(), -1e308);
    M_ASSERT_EQ(v_max.to<double>(), 1e308);
    M_ASSERT_EQ(v_min.to_or<int>(-1), static_cast<int>(std::llround(-1e308)));
    M_ASSERT_EQ(v_max.to_or<int>(-1), static_cast<int>(std::llround(1e308)));

    // 容器类型转换（std::vector, std::map, std::string）
    std::vector<json::Value> vec{json::Number(1), json::String("two")};
    json::Value v_vec{vec};
    M_ASSERT_EQ(v_vec.to<json::Array>().size(), 2);
    std::map<std::string, json::Value> mp{{"a", json::Number(1)}};
    json::Value v_mp{mp};
    M_ASSERT_EQ(v_mp.to<json::Object>().size(), 1);
    std::string s = "abc";
    json::Value v_s{s};
    M_ASSERT_EQ(v_s.to<json::String>(), "abc");

    // 复杂嵌套类型
    json::Value v_nested{json::Array{{json::Object{{"x", 1}}, json::Array{{2,3}}}}};
    M_ASSERT_EQ(v_nested.to<json::Array>().size(), 2);
    M_ASSERT_EQ(v_nested[0]["x"].to<int>(), 1);
    M_ASSERT_EQ(v_nested[1][0].to<int>(), 2);

    // Null->其他类型
    M_ASSERT_THROW(std::ignore = v_null.to<json::Array>(), std::runtime_error);
    M_ASSERT_EQ(v_null.to_or<json::Array>(json::Array{{1,2}}).size(), 2);
    M_ASSERT_FALSE(v_null.to_if<json::Array>().has_value());

    // Bool->其他类型
    M_ASSERT_THROW(std::ignore = v_true.to<json::Array>(), std::runtime_error);
    M_ASSERT_EQ(v_true.to_or<json::Array>(json::Array{{1,2}}).size(), 2);
    M_ASSERT_FALSE(v_true.to_if<json::Array>().has_value());

    // Number->其他类型
    M_ASSERT_THROW(std::ignore = v_num.to<json::Array>(), std::runtime_error);
    M_ASSERT_EQ(v_num.to_or<json::Array>(json::Array{{1,2}}).size(), 2);
    M_ASSERT_FALSE(v_num.to_if<json::Array>().has_value());

    // String->其他类型
    M_ASSERT_THROW(std::ignore = v_str.to<json::Array>(), std::runtime_error);
    M_ASSERT_EQ(v_str.to_or<json::Array>(json::Array{{1,2}}).size(), 2);
    M_ASSERT_FALSE(v_str.to_if<json::Array>().has_value());

    // Array->其他类型
    M_ASSERT_THROW(std::ignore = v_arr.to<json::String>(), std::runtime_error);
    M_ASSERT_EQ(v_arr.to_or<json::String>("default"), "default");
    M_ASSERT_FALSE(v_arr.to_if<json::String>().has_value());

    // Object->其他类型
    M_ASSERT_THROW(std::ignore = v_obj.to<json::String>(), std::runtime_error);
    M_ASSERT_EQ(v_obj.to_or<json::String>("default"), "default");
    M_ASSERT_FALSE(v_obj.to_if<json::String>().has_value());
}
