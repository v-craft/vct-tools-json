#include <vct/test_unit_macros.hpp>

import std;
import vct.test.unit;
import vct.tools.json;

using namespace vct::tools;

int main() {
    vct::test::unit::start();
}

// 基础类型测试
M_TEST(jsonBase, type) {
    M_EXPECT_NO_THROW(json::Value value);
    M_EXPECT_NO_THROW(json::Value value{});
    M_EXPECT_NO_THROW(json::Value value(json::Type::eObject));
    M_EXPECT_NO_THROW(json::Value value(json::Type::eArray));
    M_EXPECT_NO_THROW(json::Value value(json::Type::eString));
    M_EXPECT_NO_THROW(json::Value value(json::Type::eNumber));
    M_EXPECT_NO_THROW(json::Value value(json::Type::eBool));
    M_EXPECT_NO_THROW(json::Value value(json::Type::eNull));

    M_EXPECT_EQ(json::Value().type(), json::Type::eNull);
    M_EXPECT_EQ(json::Value(json::Type::eObject).type(), json::Type::eObject);
    M_EXPECT_EQ(json::Value(json::Type::eArray).type(), json::Type::eArray);
    M_EXPECT_EQ(json::Value(json::Type::eString).type(), json::Type::eString);
    M_EXPECT_EQ(json::Value(json::Type::eNumber).type(), json::Type::eNumber);
    M_EXPECT_EQ(json::Value(json::Type::eBool).type(), json::Type::eBool);
    M_EXPECT_EQ(json::Value(json::Type::eNull).type(), json::Type::eNull);
    
    M_EXPECT_STREQ(json::Value{json::Type::eObject}.type_name(), "Object");
    M_EXPECT_STREQ(json::Value{json::Type::eArray}.type_name(), "Array");
    M_EXPECT_STREQ(json::Value{json::Type::eString}.type_name(), "String");
    M_EXPECT_STREQ(json::Value{json::Type::eNumber}.type_name(), "Number");
    M_EXPECT_STREQ(json::Value{json::Type::eBool}.type_name(), "Bool");
    M_EXPECT_STREQ(json::Value{json::Type::eNull}.type_name(), "Null");

    M_EXPECT_EQ(json::Value().is(json::Type::eNull), true);
    M_EXPECT_EQ(json::Value().is(json::Type::eObject), false);
    M_EXPECT_EQ(json::Value().is(json::Type::eArray), false);
    M_EXPECT_EQ(json::Value().is(json::Type::eString), false);
    M_EXPECT_EQ(json::Value().is(json::Type::eNumber), false);
    M_EXPECT_EQ(json::Value().is(json::Type::eBool), false);

    M_EXPECT_EQ(json::Value(json::Type::eObject).is(json::Type::eObject), true);
    M_EXPECT_EQ(json::Value(json::Type::eObject).is(json::Type::eNull), false);
    M_EXPECT_EQ(json::Value(json::Type::eObject).is(json::Type::eArray), false);
    M_EXPECT_EQ(json::Value(json::Type::eObject).is(json::Type::eString), false);
    M_EXPECT_EQ(json::Value(json::Type::eObject).is(json::Type::eNumber), false);
    M_EXPECT_EQ(json::Value(json::Type::eObject).is(json::Type::eBool), false);

    M_EXPECT_EQ(json::Value(json::Type::eArray).is(json::Type::eNull), false);
    M_EXPECT_EQ(json::Value(json::Type::eArray).is(json::Type::eObject), false);
    M_EXPECT_EQ(json::Value(json::Type::eArray).is(json::Type::eArray), true);
    M_EXPECT_EQ(json::Value(json::Type::eArray).is(json::Type::eString), false);
    M_EXPECT_EQ(json::Value(json::Type::eArray).is(json::Type::eNumber), false);
    M_EXPECT_EQ(json::Value(json::Type::eArray).is(json::Type::eBool), false);

    M_EXPECT_EQ(json::Value(json::Type::eString).is(json::Type::eNull), false);
    M_EXPECT_EQ(json::Value(json::Type::eString).is(json::Type::eObject), false);
    M_EXPECT_EQ(json::Value(json::Type::eString).is(json::Type::eArray), false);
    M_EXPECT_EQ(json::Value(json::Type::eString).is(json::Type::eString), true);
    M_EXPECT_EQ(json::Value(json::Type::eString).is(json::Type::eNumber), false);
    M_EXPECT_EQ(json::Value(json::Type::eString).is(json::Type::eBool), false);


    M_EXPECT_EQ(json::Value(json::Type::eNumber).is(json::Type::eNull), false);
    M_EXPECT_EQ(json::Value(json::Type::eNumber).is(json::Type::eObject), false);
    M_EXPECT_EQ(json::Value(json::Type::eNumber).is(json::Type::eArray), false);
    M_EXPECT_EQ(json::Value(json::Type::eNumber).is(json::Type::eString), false);
    M_EXPECT_EQ(json::Value(json::Type::eNumber).is(json::Type::eNumber), true);
    M_EXPECT_EQ(json::Value(json::Type::eNumber).is(json::Type::eBool), false);

    M_EXPECT_EQ(json::Value(json::Type::eBool).is(json::Type::eNull), false);
    M_EXPECT_EQ(json::Value(json::Type::eBool).is(json::Type::eObject), false);
    M_EXPECT_EQ(json::Value(json::Type::eBool).is(json::Type::eArray), false);
    M_EXPECT_EQ(json::Value(json::Type::eBool).is(json::Type::eString), false);
    M_EXPECT_EQ(json::Value(json::Type::eBool).is(json::Type::eNumber), false);
    M_EXPECT_EQ(json::Value(json::Type::eBool).is(json::Type::eBool), true);
}

// 构造函数测试
M_TEST(jsonBase, construct) {
    M_EXPECT_NO_THROW(json::Value v = 1);
    M_EXPECT_NO_THROW(json::Value v = "str");
    M_EXPECT_NO_THROW(json::Value v = 1.11111);
    M_EXPECT_NO_THROW(json::Value v = 1234567890123456LL);
    M_EXPECT_NO_THROW(json::Value v = 1.1f);
    M_EXPECT_NO_THROW(json::Value v = std::string("hello world"));
    M_EXPECT_NO_THROW(json::Value v = 0);
    M_EXPECT_NO_THROW(json::Value v = false);
    M_EXPECT_NO_THROW(json::Value v = true);
    M_EXPECT_NO_THROW(json::Value v = nullptr);

    M_EXPECT_EQ(json::Value(1).type(), json::Type::eNumber);
    M_EXPECT_EQ(json::Value("str").type(), json::Type::eString);
    M_EXPECT_EQ(json::Value(1.11111).type(), json::Type::eNumber);
    M_EXPECT_EQ(json::Value(1234567890123456LL).type(), json::Type::eNumber);
    M_EXPECT_EQ(json::Value(1.1f).type(), json::Type::eNumber);
    M_EXPECT_EQ(json::Value(std::string("hello world")).type(), json::Type::eString);
    M_EXPECT_EQ(json::Value(0).type(), json::Type::eNumber);
    M_EXPECT_EQ(json::Value(false).type(), json::Type::eBool);
    M_EXPECT_EQ(json::Value(true).type(), json::Type::eBool);
    M_EXPECT_EQ(json::Value(nullptr).type(), json::Type::eNull);

    M_EXPECT_EQ(json::Value(1).get<int>(), 1);
    M_EXPECT_EQ(json::Value("str").get<std::string>(), "str");
    M_EXPECT_FLOAT_EQ(json::Value(1.11111).get<double>(), 1.11111, 1e-12);
    M_EXPECT_EQ(json::Value(1234567890123456LL).get<long long>(), 1234567890123456LL);
    M_EXPECT_EQ(json::Value(std::string("hello world")).get<std::string>(), "hello world");
    M_EXPECT_EQ(json::Value(0).get<int>(), 0);
    M_EXPECT_EQ(json::Value(false).get<bool>(), false);
    M_EXPECT_EQ(json::Value(true).get<bool>(), true);
    M_EXPECT_EQ(json::Value(nullptr).get<std::nullptr_t>(), nullptr);

    M_EXPECT_NO_THROW(json::Value v = json::Object{});
    M_EXPECT_NO_THROW(json::Value v = json::Array{});
    M_EXPECT_EQ(json::Value(json::Object{}).type(), json::Type::eObject);
    M_EXPECT_EQ(json::Value(json::Array{}).type(), json::Type::eArray);
}

// 类型转换测试
M_TEST(jsonBase, get) {
    json::Value v_num = 42;
    json::Value v_str = "hello";
    json::Value v_bool = true;
    json::Value v_null = nullptr;

    M_EXPECT_EQ(v_num.get<int>(), 42);
    M_EXPECT_DOUBLE_EQ_DEFAULT(v_num.get<double>(), 42.0);
    M_EXPECT_FLOAT_EQ_DEFAULT(v_num.get<float>(), 42.0f);
    M_EXPECT_EQ(v_num.get<long>(), 42L);

    M_EXPECT_STREQ(v_str.get<std::string>(), "hello");
    M_EXPECT_EQ(v_bool.get<bool>(), true);
    M_EXPECT_EQ(v_null.get<std::nullptr_t>(), nullptr);

    M_EXPECT_NO_THROW(v_num.get<bool>());
    M_EXPECT_THROW(v_str.get<int>(), std::runtime_error);
    M_EXPECT_THROW(v_bool.get<std::string>(), std::runtime_error);
    M_EXPECT_THROW(v_null.get<int>(), std::runtime_error);
}

// get_ref 测试
M_TEST(jsonBase, get_ref) {
    json::Value v_num = 42.5;
    json::Value v_str = "hello";
    json::Value v_bool = true;
    json::Value v_null = nullptr;

    M_EXPECT_EQ(v_num.get_ref<double>(), 42.5);
    M_EXPECT_EQ(v_str.get_ref<std::string>(), "hello");
    M_EXPECT_EQ(v_bool.get_ref<bool>(), true);
    M_EXPECT_EQ(v_null.get_ref<std::nullptr_t>(), nullptr);

    // 测试可变引用
    v_num.get_ref<double>() = 100.0;
    M_EXPECT_EQ(v_num.get<double>(), 100.0);

    v_str.get_ref<std::string>() = "world";
    M_EXPECT_EQ(v_str.get<std::string>(), "world");

    M_EXPECT_THROW((void)v_num.get_ref<bool>(), std::runtime_error);
    M_EXPECT_THROW((void)v_str.get_ref<double>(), std::runtime_error);
}

// 对象操作测试
M_TEST(jsonBase, object) {
    json::Value obj(json::Type::eObject);
    
    M_EXPECT_EQ(obj.type(), json::Type::eObject);
    M_EXPECT_EQ(obj.size(), 0);
    
    obj["key1"] = "value1";
    obj["key2"] = 42;
    obj["key3"] = true;
    
    M_EXPECT_EQ(obj.size(), 3);
    M_EXPECT_EQ(obj["key1"].get<std::string>(), "value1");
    M_EXPECT_EQ(obj["key2"].get<int>(), 42);
    M_EXPECT_EQ(obj["key3"].get<bool>(), true);
    
    M_EXPECT_EQ(obj.contains("key1"), true);
    M_EXPECT_EQ(obj.contains("key4"), false);


    M_EXPECT_EQ(obj.at("key1").get<std::string>(), "value1");
    M_EXPECT_THROW((void)obj.at("key4"), std::out_of_range);
    
    // 测试const访问
    const json::Value& const_obj = obj;
    M_EXPECT_EQ(const_obj["key1"].get<std::string>(), "value1");
    M_EXPECT_THROW((void)const_obj["key4"], std::out_of_range);
    
    // 测试类型错误
    json::Value arr(json::Type::eArray);
    M_EXPECT_THROW((void)arr["key"], std::runtime_error);
    M_EXPECT_THROW((void)arr.at("key"), std::runtime_error);
    M_EXPECT_EQ(arr.contains("key"), false);
}

// 数组操作测试
M_TEST(jsonBase, array) {
    json::Value arr(json::Type::eArray);
    
    M_EXPECT_EQ(arr.type(), json::Type::eArray);
    M_EXPECT_EQ(arr.size(), 0);
    
    arr.get_ref<json::Array>().push_back(json::Value("hello"));
    arr.get_ref<json::Array>().push_back(json::Value(42));
    arr.get_ref<json::Array>().push_back(json::Value(true));
    
    M_EXPECT_EQ(arr.size(), 3);
    M_EXPECT_EQ(arr[0].get<std::string>(), "hello");
    M_EXPECT_EQ(arr[1].get<int>(), 42);
    M_EXPECT_EQ(arr[2].get<bool>(), true);
    
    M_EXPECT_EQ(arr.at(0).get<std::string>(), "hello");
    M_EXPECT_THROW((void)arr.at(5), std::out_of_range);
    
    // 测试const访问
    const json::Value& const_arr = arr;
    M_EXPECT_EQ(const_arr[0].get<std::string>(), "hello");
    M_EXPECT_THROW((void)const_arr[5], std::out_of_range);
    
    // 测试类型错误
    json::Value obj(json::Type::eObject);
    M_EXPECT_THROW((void)obj[0], std::runtime_error);
    M_EXPECT_THROW((void)obj.at(0), std::runtime_error);
}

// 序列化测试
M_TEST(jsonBase, serialize) {
    json::Value v_null = nullptr;
    json::Value v_bool = true;
    json::Value v_num = 42.5;
    json::Value v_str = "hello";
    
    M_EXPECT_EQ(v_null.serialize(), "null");
    M_EXPECT_EQ(v_bool.serialize(), "true");
    M_EXPECT_EQ(json::Value(false).serialize(), "false");
    M_EXPECT_EQ(v_str.serialize(), "\"hello\"");
    
    json::Value obj(json::Type::eObject);
    obj["key1"] = "value1";
    obj["key2"] = 42;
    
    json::Value arr(json::Type::eArray);
    arr.get_ref<json::Array>().push_back(json::Value(1));
    arr.get_ref<json::Array>().push_back(json::Value(2));
    arr.get_ref<json::Array>().push_back(json::Value(3));
    
    M_EXPECT_EQ(arr.serialize(), "[1,2,3]");
    
    // 测试pretty序列化
    auto pretty = arr.serialize_pretty(2, 0, 1024);
    M_EXPECT_TRUE(pretty.has_value());
    M_EXPECT_TRUE(pretty->find("[\n") != std::string::npos);
}

// 反序列化测试
M_TEST(jsonBase, deserialize) {
    auto result_null = json::deserialize("null");
    M_EXPECT_TRUE(result_null.has_value());
    M_EXPECT_EQ(result_null->type(), json::Type::eNull);
    
    auto result_bool = json::deserialize("true");
    M_EXPECT_TRUE(result_bool.has_value());
    M_EXPECT_EQ(result_bool->type(), json::Type::eBool);
    M_EXPECT_EQ(result_bool->get<bool>(), true);
    
    auto result_num = json::deserialize("42.5");
    M_EXPECT_TRUE(result_num.has_value());
    M_EXPECT_EQ(result_num->type(), json::Type::eNumber);
    M_EXPECT_EQ(result_num->get<double>(), 42.5);
    
    auto result_str = json::deserialize("\"hello\"");
    M_EXPECT_TRUE(result_str.has_value());
    M_EXPECT_EQ(result_str->type(), json::Type::eString);
    M_EXPECT_EQ(result_str->get<std::string>(), "hello");
    
    auto result_arr = json::deserialize("[1,2,3]");
    M_EXPECT_TRUE(result_arr.has_value());
    M_EXPECT_EQ(result_arr->type(), json::Type::eArray);
    M_EXPECT_EQ(result_arr->size(), 3);
    M_EXPECT_EQ((*result_arr)[0].get<int>(), 1);
    M_EXPECT_EQ((*result_arr)[1].get<int>(), 2);
    M_EXPECT_EQ((*result_arr)[2].get<int>(), 3);
    
    auto result_obj = json::deserialize(R"({"key1":"value1","key2":42})");
    M_EXPECT_TRUE(result_obj.has_value());
    M_EXPECT_EQ(result_obj->type(), json::Type::eObject);
    M_EXPECT_EQ(result_obj->size(), 2);
    M_EXPECT_EQ((*result_obj)["key1"].get<std::string>(), "value1");
    M_EXPECT_EQ((*result_obj)["key2"].get<int>(), 42);
    
    // 测试错误情况
    auto result_error = json::deserialize("invalid json");
    M_EXPECT_FALSE(result_error.has_value());
    M_EXPECT_EQ(result_error.error(), json::ParseError::eUnknownFormat);
    
    auto result_empty = json::deserialize("");
    M_EXPECT_FALSE(result_empty.has_value());
    M_EXPECT_EQ(result_empty.error(), json::ParseError::eEmptyData);
}

// 复杂嵌套测试
M_TEST(jsonBase, complex) {
    json::Value complex_obj(json::Type::eObject);
    complex_obj["name"] = "test";
    complex_obj["version"] = 1;
    complex_obj["active"] = true;
    
    json::Value arr(json::Type::eArray);
    arr.get_ref<json::Array>().push_back(json::Value("item1"));
    arr.get_ref<json::Array>().push_back(json::Value("item2"));
    
    complex_obj["items"] = arr;
    
    json::Value nested_obj(json::Type::eObject);
    nested_obj["nested_key"] = "nested_value";
    complex_obj["nested"] = nested_obj;
    
    M_EXPECT_EQ(complex_obj["name"].get<std::string>(), "test");
    M_EXPECT_EQ(complex_obj["version"].get<int>(), 1);
    M_EXPECT_EQ(complex_obj["active"].get<bool>(), true);
    M_EXPECT_EQ(complex_obj["items"].size(), 2);
    M_EXPECT_EQ(complex_obj["items"][0].get<std::string>(), "item1");
    M_EXPECT_EQ(complex_obj["nested"]["nested_key"].get<std::string>(), "nested_value");
    
    // 测试序列化和反序列化
    std::string serialized = complex_obj.serialize();
    auto deserialized = json::deserialize(serialized);
    M_EXPECT_TRUE(deserialized.has_value());
    M_EXPECT_EQ(deserialized->type(), json::Type::eObject);
    M_EXPECT_EQ((*deserialized)["name"].get<std::string>(), "test");
    M_EXPECT_EQ((*deserialized)["items"].size(), 2);
}

// 移动语义测试
M_TEST(jsonBase, move) {
    json::Value original = "hello";
    json::Value moved = std::move(original);
    
    M_EXPECT_EQ(moved.get<std::string>(), "hello");
    M_EXPECT_EQ(original.type(), json::Type::eNull);
    
    json::Value target;
    target = std::move(moved);
    M_EXPECT_EQ(target.get<std::string>(), "hello");
    M_EXPECT_EQ(moved.type(), json::Type::eNull);
}

// reset 和 clear_data 测试
M_TEST(jsonBase, reset) {
    json::Value v = "hello";
    M_EXPECT_EQ(v.type(), json::Type::eString);
    
    v.reset();
    M_EXPECT_EQ(v.type(), json::Type::eNull);
    
    v = json::Value(json::Type::eString);
    M_EXPECT_EQ(v.type(), json::Type::eString);
    M_EXPECT_EQ(v.get<std::string>(), "");
    
    v = "world";
    M_EXPECT_EQ(v.get<std::string>(), "world");
    
    v.clear_data();
    M_EXPECT_EQ(v.type(), json::Type::eString);
    M_EXPECT_EQ(v.get<std::string>(), "");
}

// 解析错误测试
M_TEST(jsonBase, parse_errors) {
    auto test_error = [](const std::string& json_str, json::ParseError expected_error) {
        auto result = json::deserialize(json_str);
        M_EXPECT_FALSE(result.has_value());
        M_EXPECT_EQ(result.error(), expected_error);
    };
    
    test_error("", json::ParseError::eEmptyData);
    test_error("   ", json::ParseError::eEmptyData);
    test_error("invalid", json::ParseError::eUnknownFormat);
    test_error("{", json::ParseError::eUnclosedObject);
    test_error("[", json::ParseError::eUnclosedArray);
    test_error("\"unclosed string", json::ParseError::eUnclosedString);
    test_error("\"\\invalid\"", json::ParseError::eIllegalEscape);
    test_error("123.456.789", json::ParseError::eUnknownFormat);
    
    // 测试深度限制
    std::string deep_json = "[";
    for (int i = 0; i < 600; ++i) {
        deep_json += "[";
    }
    for (int i = 0; i < 600; ++i) {
        deep_json += "]";
    }
    deep_json += "]";
    
    auto deep_result = json::deserialize(deep_json, 500);
    M_EXPECT_FALSE(deep_result.has_value());
    M_EXPECT_EQ(deep_result.error(), json::ParseError::eDepthExceeded);
}

// 容器构造测试
M_TEST(jsonBase, containers) {
    json::Object obj_data;
    obj_data["key1"] = json::Value("value1");
    obj_data["key2"] = json::Value(42);
    
    json::Value obj_from_map = obj_data;
    M_EXPECT_EQ(obj_from_map.type(), json::Type::eObject);
    M_EXPECT_EQ(obj_from_map["key1"].get<std::string>(), "value1");
    M_EXPECT_EQ(obj_from_map["key2"].get<int>(), 42);
    
    json::Array arr_data;
    arr_data.push_back(json::Value("item1"));
    arr_data.push_back(json::Value(123));
    
    json::Value arr_from_vector = arr_data;
    M_EXPECT_EQ(arr_from_vector.type(), json::Type::eArray);
    M_EXPECT_EQ(arr_from_vector[0].get<std::string>(), "item1");
    M_EXPECT_EQ(arr_from_vector[1].get<int>(), 123);
    
    // 测试移动构造
    json::Value moved_obj = std::move(obj_data);
    M_EXPECT_EQ(moved_obj.type(), json::Type::eObject);
    M_EXPECT_EQ(moved_obj["key1"].get<std::string>(), "value1");
    
    json::Value moved_arr = std::move(arr_data);
    M_EXPECT_EQ(moved_arr.type(), json::Type::eArray);
    M_EXPECT_EQ(moved_arr[0].get<std::string>(), "item1");
}

