#include <vct/test_unit_macros.hpp>

import std;
import vct.test.unit;
import vct.tools.json;

using namespace vct::tools;

enum class MyEnum { A = 5, B = 6, C = -1 };

M_TEST(Value, To) {
    // Null
    Json v_null{nullptr};
    M_ASSERT_EQ(v_null.to<Json::Null>(), nullptr);
    M_ASSERT_THROW(std::ignore = v_null.to<bool>(), std::runtime_error); // Null->Bool抛异常
    M_ASSERT_EQ(v_null.to_or<bool>(true), true); // Null->Bool不可转换，返回默认值true
    M_ASSERT_EQ(v_null.to_or<bool>(false), false); // Null->Bool不可转换，返回默认值false
    M_ASSERT_FALSE(v_null.to_if<bool>().has_value());
    M_ASSERT_TRUE(v_null.to_if<Json::Null>().has_value());
    M_ASSERT_EQ(v_null.to_if<Json::Null>().value(), nullptr);

    // Bool
    Json v_true{true};
    Json v_false{false};
    M_ASSERT_EQ(v_true.to<Json::Bool>(), true);
    M_ASSERT_EQ(v_false.to<Json::Bool>(), false);
    M_ASSERT_EQ(v_true.to_or<Json::Bool>(false), true);
    M_ASSERT_EQ(v_false.to_or<Json::Bool>(true), false);
    M_ASSERT_EQ(v_true.to_or<int>(-1), 1); // Bool->int可转换，true->1
    M_ASSERT_EQ(v_false.to_or<int>(-1), 0); // Bool->int可转换，false->0
    M_ASSERT_TRUE(v_true.to_if<int>().has_value());
    M_ASSERT_EQ(v_true.to_if<int>().value(), 1);
    M_ASSERT_EQ(v_false.to_if<int>().value(), 0);
    M_ASSERT_EQ(v_true.to_or<double>(-1.0), 1.0); // Bool->double可转换，返回1.0
    M_ASSERT_EQ(v_false.to_or<double>(-1.0), 0.0);
    M_ASSERT_EQ(v_true.to<Json::Number>(), 1.0); // Bool->Number可转换
    M_ASSERT_EQ(v_false.to<Json::Number>(), 0.0);
    M_ASSERT_EQ(v_true.to<double>(), 1.0);
    M_ASSERT_EQ(v_false.to<double>(), 0.0);
    M_ASSERT_EQ(v_true.to_or<Json::String>("default"), "default"); // Bool->String不可转换
    M_ASSERT_FALSE(v_true.to_if<Json::String>().has_value());

    // Number
    Json v_num{42.5};
    Json v_int{123};
    Json v_zero{0};
    Json v_neg{-7.8};
    Json v_big{1e100};
    M_ASSERT_EQ(v_num.to<Json::Number>(), 42.5);
    M_ASSERT_EQ(v_int.to<Json::Number>(), 123.0);
    M_ASSERT_EQ(v_zero.to<Json::Number>(), 0.0);
    M_ASSERT_EQ(v_neg.to<Json::Number>(), -7.8);
    M_ASSERT_EQ(v_big.to<Json::Number>(), 1e100);
    M_ASSERT_EQ(v_num.to<int>(), 43); // 四舍五入
    M_ASSERT_EQ(v_neg.to<int>(), -8);
    M_ASSERT_EQ(v_num.to_or<int>(-1), 43);
    M_ASSERT_EQ(v_num.to_or<double>(-1.0), 42.5);
    M_ASSERT_EQ(v_num.to_or<MyEnum>(MyEnum::C), static_cast<MyEnum>(43)); // Number->enum转换，返回43对应的枚举值
    M_ASSERT_TRUE(v_num.to_if<MyEnum>().has_value()); // 42.5可转换为MyEnum，值为static_cast<MyEnum>(43)
    M_ASSERT_EQ(v_num.to_if<MyEnum>().value(), static_cast<MyEnum>(43));
    Json v_enum{5};
    M_ASSERT_EQ(v_enum.to<MyEnum>(), MyEnum::A);
    M_ASSERT_EQ(v_enum.to_or<MyEnum>(MyEnum::C), MyEnum::A);
    M_ASSERT_TRUE(v_enum.to_if<MyEnum>().has_value());
    M_ASSERT_EQ(v_enum.to_if<MyEnum>().value(), MyEnum::A);
    M_ASSERT_EQ(v_num.to_or<Json::String>("default"), "default"); // Number->String不可转换
    M_ASSERT_FALSE(v_num.to_if<Json::String>().has_value());

    // String
    Json v_str{"hello"};
    Json v_empty_str{""};
    M_ASSERT_EQ(v_str.to<Json::String>(), "hello");
    M_ASSERT_EQ(v_empty_str.to<Json::String>(), "");
    M_ASSERT_EQ(v_str.to_or<Json::String>("default"), "hello");
    M_ASSERT_EQ(v_empty_str.to_or<Json::String>("default"), "");
    M_ASSERT_EQ(v_str.to_or<int>(-99), -99); // String->int不可转换
    M_ASSERT_FALSE(v_str.to_if<int>().has_value());
    M_ASSERT_EQ(v_str.to_or<Json::Bool>(false), false); // String->Bool不可转换
    M_ASSERT_FALSE(v_str.to_if<Json::Bool>().has_value());

    // Array
    Json v_arr{Json::Array{{1, 2, 3}}};
    Json v_empty_arr{Json::Array{}};
    M_ASSERT_EQ(v_arr.to<Json::Array>().size(), 3);
    M_ASSERT_EQ(v_empty_arr.to<Json::Array>().size(), 0);
    M_ASSERT_EQ(v_arr.to_or<Json::Array>(Json::Array{{4,5}}).size(), 3);
    M_ASSERT_EQ(v_empty_arr.to_or<Json::Array>(Json::Array{{4,5}}).size(), 0);
    M_ASSERT_EQ(v_arr.to_or<Json::Object>(Json::Object{{"k",1}}).size(), 1); // Array->Object不可转换，返回默认
    M_ASSERT_FALSE(v_arr.to_if<Json::Object>().has_value());
    M_ASSERT_EQ(v_arr.to_or<Json::String>("default"), "default");
    M_ASSERT_FALSE(v_arr.to_if<Json::String>().has_value());

    // Object
    Json v_obj{Json::Object{{"k", 1}}};
    Json v_empty_obj{Json::Object{}};
    M_ASSERT_EQ(v_obj.to<Json::Object>().size(), 1);
    M_ASSERT_EQ(v_empty_obj.to<Json::Object>().size(), 0);
    M_ASSERT_EQ(v_obj.to_or<Json::Object>(Json::Object{{"x",2}}).size(), 1);
    M_ASSERT_EQ(v_empty_obj.to_or<Json::Object>(Json::Object{{"x",2}}).size(), 0);
    M_ASSERT_EQ(v_obj.to_or<Json::Array>(Json::Array{{7,8}}).size(), 2); // Object->Array不可转换，返回默认
    M_ASSERT_FALSE(v_obj.to_if<Json::Array>().has_value());
    M_ASSERT_EQ(v_obj.to_or<Json::String>("default"), "default");
    M_ASSERT_FALSE(v_obj.to_if<Json::String>().has_value());

    // Value->Value
    M_ASSERT_EQ(v_num.to<Json>(), v_num);
    M_ASSERT_EQ(v_num.to_or<Json>(Json{0}), v_num);
    auto opt_val = v_num.to_if<Json>();
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

    M_ASSERT_THROW(std::ignore = v_arr.to<Json::Object>(), std::runtime_error);
    M_ASSERT_EQ(v_arr.to_or<Json::Object>(Json::Object{{"k",1}}).size(), 1);
    M_ASSERT_FALSE(v_arr.to_if<Json::Object>().has_value());

    M_ASSERT_THROW(std::ignore = v_obj.to<Json::Array>(), std::runtime_error);
    M_ASSERT_EQ(v_obj.to_or<Json::Array>(Json::Array{{1,2}}).size(), 2);
    M_ASSERT_FALSE(v_obj.to_if<Json::Array>().has_value());

    // 边界值测试
    Json v_min{-1e308};
    Json v_max{1e308};
    M_ASSERT_EQ(v_min.to<double>(), -1e308);
    M_ASSERT_EQ(v_max.to<double>(), 1e308);
    M_ASSERT_EQ(v_min.to_or<int>(-1), static_cast<int>(std::llround(-1e308)));
    M_ASSERT_EQ(v_max.to_or<int>(-1), static_cast<int>(std::llround(1e308)));

    // 容器类型转换（std::vector, std::map, std::string）
    std::vector<Json> vec{Json::Number(1), Json::String("two")};
    Json v_vec{vec};
    M_ASSERT_EQ(v_vec.to<Json::Array>().size(), 2);
    std::map<std::string, Json> mp{{"a", Json::Number(1)}};
    Json v_mp{mp};
    M_ASSERT_EQ(v_mp.to<Json::Object>().size(), 1);
    std::string s = "abc";
    Json v_s{s};
    M_ASSERT_EQ(v_s.to<Json::String>(), "abc");

    // 复杂嵌套类型
    Json v_nested{Json::Array{{Json::Object{{"x", 1}}, Json::Array{{2,3}}}}};
    M_ASSERT_EQ(v_nested.to<Json::Array>().size(), 2);
    M_ASSERT_EQ(v_nested[0]["x"].to<int>(), 1);
    M_ASSERT_EQ(v_nested[1][0].to<int>(), 2);

    // Null->其他类型
    M_ASSERT_THROW(std::ignore = v_null.to<Json::Array>(), std::runtime_error);
    M_ASSERT_EQ(v_null.to_or<Json::Array>(Json::Array{{1,2}}).size(), 2);
    M_ASSERT_FALSE(v_null.to_if<Json::Array>().has_value());

    // Bool->其他类型
    M_ASSERT_THROW(std::ignore = v_true.to<Json::Array>(), std::runtime_error);
    M_ASSERT_EQ(v_true.to_or<Json::Array>(Json::Array{{1,2}}).size(), 2);
    M_ASSERT_FALSE(v_true.to_if<Json::Array>().has_value());

    // Number->其他类型
    M_ASSERT_THROW(std::ignore = v_num.to<Json::Array>(), std::runtime_error);
    M_ASSERT_EQ(v_num.to_or<Json::Array>(Json::Array{{1,2}}).size(), 2);
    M_ASSERT_FALSE(v_num.to_if<Json::Array>().has_value());

    // String->其他类型
    M_ASSERT_THROW(std::ignore = v_str.to<Json::Array>(), std::runtime_error);
    M_ASSERT_EQ(v_str.to_or<Json::Array>(Json::Array{{1,2}}).size(), 2);
    M_ASSERT_FALSE(v_str.to_if<Json::Array>().has_value());

    // Array->其他类型
    M_ASSERT_THROW(std::ignore = v_arr.to<Json::String>(), std::runtime_error);
    M_ASSERT_EQ(v_arr.to_or<Json::String>("default"), "default");
    M_ASSERT_FALSE(v_arr.to_if<Json::String>().has_value());

    // Object->其他类型
    M_ASSERT_THROW(std::ignore = v_obj.to<Json::String>(), std::runtime_error);
    M_ASSERT_EQ(v_obj.to_or<Json::String>("default"), "default");
    M_ASSERT_FALSE(v_obj.to_if<Json::String>().has_value());
}
