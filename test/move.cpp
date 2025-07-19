#include <vct/test_unit_macros.hpp>

import std;
import vct.test.unit;
import vct.tools.json;

using namespace vct::tools;

enum class MyEnum { A = 5, B = 6, C = -1 };

M_TEST(Value, Move) {
    // Null
    json::Value v_null{nullptr};
    auto moved_null = v_null.move<json::Null>();
    M_ASSERT_EQ(moved_null, nullptr);
    M_ASSERT_EQ(v_null.type(), json::Type::eNull);

    // Bool
    json::Value v_true{true};
    auto moved_bool = v_true.move<json::Bool>();
    M_ASSERT_EQ(moved_bool, true);
    M_ASSERT_EQ(v_true.type(), json::Type::eBool);
    M_ASSERT_EQ(v_true.to<json::Bool>(), true); // 简单类型移动后仍为原值

    // Number
    json::Value v_num{42.5};
    auto moved_num = v_num.move<json::Number>();
    M_ASSERT_EQ(moved_num, 42.5);
    M_ASSERT_EQ(v_num.type(), json::Type::eNumber);
    M_ASSERT_EQ(v_num.to<json::Number>(), 42.5);

    // String
    json::Value v_str{"hello"};
    auto moved_str = v_str.move<json::String>();
    M_ASSERT_EQ(moved_str, "hello");
    M_ASSERT_EQ(v_str.type(), json::Type::eString);
    M_ASSERT_EQ(v_str.to<json::String>(), ""); // 移动后原字符串为空

    // Array
    json::Value v_arr{json::Array{{1, 2, 3}}};
    auto moved_arr = v_arr.move<json::Array>();
    M_ASSERT_EQ(moved_arr.size(), 3);
    M_ASSERT_EQ(moved_arr[0].to<int>(), 1);
    M_ASSERT_EQ(v_arr.type(), json::Type::eArray);
    M_ASSERT_EQ(v_arr.to<json::Array>().size(), 0); // 移动后原数组为空

    // Object
    json::Value v_obj{json::Object{{"k", 1}, {"b", 2}}};
    auto moved_obj = v_obj.move<json::Object>();
    M_ASSERT_EQ(moved_obj.size(), 2);
    M_ASSERT_EQ(moved_obj.at("k").to<int>(), 1);
    M_ASSERT_EQ(v_obj.type(), json::Type::eObject);
    M_ASSERT_EQ(v_obj.to<json::Object>().size(), 0); // 移动后原对象为空

    // Enum
    json::Value v_enum{5};
    auto moved_enum = v_enum.move<MyEnum>();
    M_ASSERT_EQ(moved_enum, MyEnum::A);

    // Integral
    json::Value v_int{123};
    auto moved_int = v_int.move<int>();
    M_ASSERT_EQ(moved_int, 123);

    // Floating point
    json::Value v_fp{3.14};
    auto moved_fp = v_fp.move<double>();
    M_ASSERT_EQ(moved_fp, 3.14);

    // std::string_view
    json::Value v_sv{"abc"};
    auto moved_sv = v_sv.move<std::string_view>();
    M_ASSERT_EQ(moved_sv, std::string_view("abc"));
    M_ASSERT_EQ(v_sv.to<json::String>(), "abc"); // string_view 不修改自身

    // std::string_view
    json::Value v_sv2{"abc"};
    auto moved_sv2 = v_sv2.move<std::string>();
    M_ASSERT_EQ(moved_sv2, std::string_view("abc"));
    M_ASSERT_EQ(v_sv2.to<json::String>(), ""); // string 移动

    // move_if: 成功
    json::Value v_arr2{json::Array{{4, 5}}};
    auto opt_arr = v_arr2.move_if<json::Array>();
    M_ASSERT_TRUE(opt_arr.has_value());
    M_ASSERT_EQ(opt_arr->size(), 2);
    M_ASSERT_EQ(v_arr2.to<json::Array>().size(), 0);

    // move_if: 失败
    json::Value v_fail{true};
    auto opt_fail = v_fail.move_if<json::Array>();
    M_ASSERT_FALSE(opt_fail.has_value());

    // move_or: 成功
    json::Value v_obj2{json::Object{{"x", 7}}};
    auto moved_obj2 = v_obj2.move_or<json::Object>(json::Object{{"default", 0}});
    M_ASSERT_EQ(moved_obj2.size(), 1);
    M_ASSERT_EQ(moved_obj2.at("x").to<int>(), 7);
    M_ASSERT_EQ(v_obj2.to<json::Object>().size(), 0);

    // move_or: 失败返回默认
    json::Value v_fail2{false};
    auto moved_default = v_fail2.move_or<json::Array>(json::Array{{9}});
    M_ASSERT_EQ(moved_default.size(), 1);
    M_ASSERT_EQ(moved_default[0].to<int>(), 9);

    // 边界值
    json::Value v_empty_arr{json::Array{}};
    auto moved_empty_arr = v_empty_arr.move<json::Array>();
    M_ASSERT_EQ(moved_empty_arr.size(), 0);

    json::Value v_empty_obj{json::Object{}};
    auto moved_empty_obj = v_empty_obj.move<json::Object>();
    M_ASSERT_EQ(moved_empty_obj.size(), 0);

    json::Value v_empty_str{""};
    auto moved_empty_str = v_empty_str.move<json::String>();
    M_ASSERT_EQ(moved_empty_str, "");
    M_ASSERT_EQ(v_empty_str.to<json::String>(), "");

    // 复杂嵌套结构
    json::Value v_nested{json::Array{
        json::Object{{"a", 1}},
        json::Array{{2, 3}},
        json::String("deep")
    }};
    auto moved_nested = v_nested.move<json::Array>();
    M_ASSERT_EQ(moved_nested.size(), 3);
    M_ASSERT_EQ(moved_nested[0]["a"].to<int>(), 1);
    M_ASSERT_EQ(moved_nested[1][0].to<int>(), 2);
    M_ASSERT_EQ(moved_nested[2].to<json::String>(), "deep");
    M_ASSERT_EQ(v_nested.to<json::Array>().size(), 0);

    // move: 不兼容类型抛异常
    json::Value v_wrong{true};
    M_ASSERT_THROW(std::ignore = v_wrong.move<json::Array>(), std::runtime_error);

    // move_or: 不兼容类型返回默认
    auto moved_or_default = v_wrong.move_or<json::Array>(json::Array{{99}});
    M_ASSERT_EQ(moved_or_default.size(), 1);
    M_ASSERT_EQ(moved_or_default[0].to<int>(), 99);

    // move_if: 不兼容类型返回nullopt
    auto opt_wrong = v_wrong.move_if<json::Array>();
    M_ASSERT_FALSE(opt_wrong.has_value());
}
