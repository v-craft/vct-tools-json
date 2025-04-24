// //
// // Created by mys_vac on 25-4-19.
// import std;
import Vct.Tools.Json;
#include "test/M_TEST_MAIN.h"

using namespace Vct::Tools;

// 基础类型测试
M_TEST(JsonBase, type) {
    M_EXPECT_NO_THROW(Json::Value value);
    M_EXPECT_NO_THROW(Json::Value value{});
    M_EXPECT_NO_THROW(Json::Value value(Json::Type::object));
    M_EXPECT_NO_THROW(Json::Value value(Json::Type::array));
    M_EXPECT_NO_THROW(Json::Value value(Json::Type::string));
    M_EXPECT_NO_THROW(Json::Value value(Json::Type::number));
    M_EXPECT_NO_THROW(Json::Value value(Json::Type::boolean));
    M_EXPECT_NO_THROW(Json::Value value(Json::Type::null));

    M_EXPECT_EQ(Json::Value().type(), Json::Type::null);
    M_EXPECT_EQ(Json::Value(Json::Type::object).type(), Json::Type::object);
    M_EXPECT_EQ(Json::Value(Json::Type::array).type(), Json::Type::array);
    M_EXPECT_EQ(Json::Value(Json::Type::string).type(), Json::Type::string);
    M_EXPECT_EQ(Json::Value(Json::Type::number).type(), Json::Type::number);
    M_EXPECT_EQ(Json::Value(Json::Type::boolean).type(), Json::Type::boolean);
    M_EXPECT_EQ(Json::Value(Json::Type::null).type(), Json::Type::null);
    M_EXPECT_EQ(Json::Value{}.type(), Json::Type::null);
    M_EXPECT_EQ(Json::Value{Json::Type::object}.type(), Json::Type::object);
    M_EXPECT_EQ(Json::Value{Json::Type::array}.type(), Json::Type::array);
    M_EXPECT_EQ(Json::Value{Json::Type::string}.type(), Json::Type::string);
    M_EXPECT_EQ(Json::Value{Json::Type::number}.type(), Json::Type::number);
    M_EXPECT_EQ(Json::Value{Json::Type::boolean}.type(), Json::Type::boolean);
    M_EXPECT_EQ(Json::Value{Json::Type::null}.type(), Json::Type::null);

    M_EXPECT_EQ(Json::Value().is_object(), false);
    M_EXPECT_EQ(Json::Value().is_array(), false);
    M_EXPECT_EQ(Json::Value().is_string(), false);
    M_EXPECT_EQ(Json::Value().is_number(), false);
    M_EXPECT_EQ(Json::Value().is_bool(), false);
    M_EXPECT_EQ(Json::Value().is_int64(), false);
    M_EXPECT_EQ(Json::Value().is_double(), false);

    M_EXPECT_EQ(Json::Value().is_null(), true);
    M_EXPECT_EQ(Json::Value(Json::Type::object).is_object(), true);
    M_EXPECT_EQ(Json::Value(Json::Type::array).is_array(), true);
    M_EXPECT_EQ(Json::Value(Json::Type::string).is_string(), true);
    M_EXPECT_EQ(Json::Value(Json::Type::number).is_number(), true);
    M_EXPECT_EQ(Json::Value(Json::Type::boolean).is_bool(), true);
    M_EXPECT_EQ(Json::Value(Json::Type::null).is_null(), true);

    M_EXPECT_EQ(Json::Value(Json::Type::object).is_array(), false);
    M_EXPECT_EQ(Json::Value(Json::Type::object).is_string(), false);
    M_EXPECT_EQ(Json::Value(Json::Type::object).is_number(), false);
    M_EXPECT_EQ(Json::Value(Json::Type::object).is_bool(), false);
    M_EXPECT_EQ(Json::Value(Json::Type::object).is_null(), false);
    M_EXPECT_EQ(Json::Value(Json::Type::object).is_int64(), false);
    M_EXPECT_EQ(Json::Value(Json::Type::object).is_double(), false);

    M_EXPECT_EQ(Json::Value(Json::Type::array).is_object(), false);
    M_EXPECT_EQ(Json::Value(Json::Type::array).is_string(), false);
    M_EXPECT_EQ(Json::Value(Json::Type::array).is_number(), false);
    M_EXPECT_EQ(Json::Value(Json::Type::array).is_bool(), false);
    M_EXPECT_EQ(Json::Value(Json::Type::array).is_null(), false);
    M_EXPECT_EQ(Json::Value(Json::Type::array).is_int64(), false);
    M_EXPECT_EQ(Json::Value(Json::Type::array).is_double(), false);

    M_EXPECT_EQ(Json::Value(Json::Type::string).is_object(), false);
    M_EXPECT_EQ(Json::Value(Json::Type::string).is_array(), false);
    M_EXPECT_EQ(Json::Value(Json::Type::string).is_number(), false);
    M_EXPECT_EQ(Json::Value(Json::Type::string).is_bool(), false);
    M_EXPECT_EQ(Json::Value(Json::Type::string).is_null(), false);
    M_EXPECT_EQ(Json::Value(Json::Type::string).is_int64(), false);
    M_EXPECT_EQ(Json::Value(Json::Type::string).is_double(), false);

    M_EXPECT_EQ(Json::Value(Json::Type::number).is_object(), false);
    M_EXPECT_EQ(Json::Value(Json::Type::number).is_array(), false);
    M_EXPECT_EQ(Json::Value(Json::Type::number).is_string(), false);
    M_EXPECT_EQ(Json::Value(Json::Type::number).is_bool(), false);
    M_EXPECT_EQ(Json::Value(Json::Type::number).is_null(), false);
    M_EXPECT_EQ(Json::Value(Json::Type::number).is_int64(), true);
    M_EXPECT_EQ(Json::Value(Json::Type::number).is_double(), false);

    M_EXPECT_EQ(Json::Value(Json::Type::boolean).is_object(), false);
    M_EXPECT_EQ(Json::Value(Json::Type::boolean).is_array(), false);
    M_EXPECT_EQ(Json::Value(Json::Type::boolean).is_string(), false);
    M_EXPECT_EQ(Json::Value(Json::Type::boolean).is_number(), false);
    M_EXPECT_EQ(Json::Value(Json::Type::boolean).is_null(), false);
    M_EXPECT_EQ(Json::Value(Json::Type::boolean).is_int64(), false);
    M_EXPECT_EQ(Json::Value(Json::Type::boolean).is_double(), false);

    M_EXPECT_EQ(Json::Value(Json::Type::null).is_object(), false);
    M_EXPECT_EQ(Json::Value(Json::Type::null).is_array(), false);
    M_EXPECT_EQ(Json::Value(Json::Type::null).is_string(), false);
    M_EXPECT_EQ(Json::Value(Json::Type::null).is_number(), false);
    M_EXPECT_EQ(Json::Value(Json::Type::null).is_bool(), false);
    M_EXPECT_EQ(Json::Value(Json::Type::null).is_int64(), false);
    M_EXPECT_EQ(Json::Value(Json::Type::null).is_double(), false);

}

// 基础as运算符异常测试
M_TEST(JsonBase, as) {

    M_EXPECT_NO_THROW(Json::Value (Json::Type::object).as_object());
    M_EXPECT_NO_THROW(Json::Value (Json::Type::object).as_string());
    M_EXPECT_NO_THROW(Json::Value (Json::Type::array).as_array());
    M_EXPECT_NO_THROW(Json::Value (Json::Type::array).as_string());
    M_EXPECT_NO_THROW(Json::Value (Json::Type::number).as_int64());
    M_EXPECT_NO_THROW(Json::Value (Json::Type::number).as_double());
    M_EXPECT_NO_THROW(Json::Value (Json::Type::number).as_ldouble());
    M_EXPECT_NO_THROW(Json::Value (Json::Type::number).as_string());
    M_EXPECT_NO_THROW(Json::Value (Json::Type::boolean).as_bool());
    M_EXPECT_NO_THROW(Json::Value (Json::Type::boolean).as_string());
    M_EXPECT_NO_THROW(Json::Value (Json::Type::string).as_string());
    M_EXPECT_NO_THROW(Json::Value (Json::Type::null).as_string());


    M_EXPECT_THROW(Json::Value (Json::Type::object).as_array(), Json::TypeException);
    M_EXPECT_THROW(Json::Value (Json::Type::object).as_ldouble(), Json::TypeException);
    M_EXPECT_THROW(Json::Value (Json::Type::object).as_double(), Json::TypeException);
    M_EXPECT_THROW(Json::Value (Json::Type::object).as_int64(), Json::TypeException);
    M_EXPECT_THROW(Json::Value (Json::Type::object).as_bool(), Json::TypeException);


    M_EXPECT_THROW(Json::Value (Json::Type::array).as_object(), Json::TypeException);
    M_EXPECT_THROW(Json::Value (Json::Type::array).as_ldouble(), Json::TypeException);
    M_EXPECT_THROW(Json::Value (Json::Type::array).as_double(), Json::TypeException);
    M_EXPECT_THROW(Json::Value (Json::Type::array).as_int64(), Json::TypeException);
    M_EXPECT_THROW(Json::Value (Json::Type::array).as_bool(), Json::TypeException);


    M_EXPECT_THROW(Json::Value (Json::Type::string).as_object(), Json::TypeException);
    M_EXPECT_THROW(Json::Value (Json::Type::string).as_array(), Json::TypeException);
    M_EXPECT_THROW(Json::Value (Json::Type::string).as_ldouble(), Json::TypeException);
    M_EXPECT_THROW(Json::Value (Json::Type::string).as_double(), Json::TypeException);
    M_EXPECT_THROW(Json::Value (Json::Type::string).as_int64(), Json::TypeException);
    M_EXPECT_THROW(Json::Value (Json::Type::string).as_bool(), Json::TypeException);


    M_EXPECT_THROW(Json::Value (Json::Type::number).as_object(), Json::TypeException);
    M_EXPECT_THROW(Json::Value (Json::Type::number).as_array(), Json::TypeException);
    M_EXPECT_THROW(Json::Value (Json::Type::number).as_bool(), Json::TypeException);


    M_EXPECT_THROW(Json::Value (Json::Type::null).as_object(), Json::TypeException);
    M_EXPECT_THROW(Json::Value (Json::Type::null).as_array(), Json::TypeException);
    M_EXPECT_THROW(Json::Value (Json::Type::null).as_bool(), Json::TypeException);
    M_EXPECT_THROW(Json::Value (Json::Type::null).as_int64(), Json::TypeException);
    M_EXPECT_THROW(Json::Value (Json::Type::null).as_double(), Json::TypeException);
    M_EXPECT_THROW(Json::Value (Json::Type::null).as_ldouble(), Json::TypeException);

}

// 基础类型转换函数测试
M_TEST(JsonBase, construct) {
    M_EXPECT_NO_THROW(Json::Value v = 1);
    M_EXPECT_NO_THROW(Json::Value v = "str");
    M_EXPECT_NO_THROW(Json::Value v = 1.11111);
    M_EXPECT_NO_THROW(Json::Value v = 1234567890123456LL);
    M_EXPECT_NO_THROW(Json::Value v = 1.1f);
    M_EXPECT_NO_THROW(Json::Value v = std::string("hello world"));
    M_EXPECT_NO_THROW(Json::Value v = 0);
    M_EXPECT_NO_THROW(Json::Value v = false);
    M_EXPECT_NO_THROW(Json::Value v = true);
    M_EXPECT_NO_THROW(Json::Value v = nullptr);

    M_EXPECT_EQ(Json::Value (1).type(), Json::Type::number);
    M_EXPECT_EQ(Json::Value ("str").type(), Json::Type::string);
    M_EXPECT_EQ(Json::Value (1.11111).type(), Json::Type::number);
    M_EXPECT_EQ(Json::Value (1234567890123456LL).type(), Json::Type::number);
    M_EXPECT_EQ(Json::Value (1.1f).type(), Json::Type::number);
    M_EXPECT_EQ(Json::Value (std::string("hello world")).type(), Json::Type::string);
    M_EXPECT_EQ(Json::Value (0).type(), Json::Type::number);
    M_EXPECT_EQ(Json::Value (false).type(), Json::Type::boolean);
    M_EXPECT_EQ(Json::Value (true).type(), Json::Type::boolean);
    M_EXPECT_EQ(Json::Value (nullptr).type(), Json::Type::null);
    M_EXPECT_EQ(Json::Value {1}.type(), Json::Type::number);
    M_EXPECT_EQ(Json::Value {"str"}.type(), Json::Type::string);
    M_EXPECT_EQ(Json::Value {1.11111}.type(), Json::Type::number);
    M_EXPECT_EQ(Json::Value {1234567890123456LL}.type(), Json::Type::number);
    M_EXPECT_EQ(Json::Value {1.1f}.type(), Json::Type::number);
    M_EXPECT_EQ(Json::Value {std::string("hello world")}.type(), Json::Type::string);
    M_EXPECT_EQ(Json::Value {0}.type(), Json::Type::number);
    M_EXPECT_EQ(Json::Value {false}.type(), Json::Type::boolean);
    M_EXPECT_EQ(Json::Value {true}.type(), Json::Type::boolean);
    M_EXPECT_EQ(Json::Value {nullptr}.type(), Json::Type::null);

    M_EXPECT_EQ(Json::Value (1).as_int64(), 1);
    M_EXPECT_EQ(Json::Value ("str").as_string(), "str");
    M_EXPECT_FLOAT_EQ(Json::Value (1.11111).as_ldouble(), 1.11111, 1e-12);
    M_EXPECT_FLOAT_EQ(Json::Value (1.11111).as_double(), 1.11111, 1e-12);
    M_EXPECT_FLOAT_EQ(Json::Value (1.1f).as_double(), 1.1f, 1e-4);
    M_EXPECT_EQ(Json::Value (1234567890123456LL).as_int64(), 1234567890123456LL);
    M_EXPECT_EQ(Json::Value (std::string("hello world")).as_string(), "hello world");
    M_EXPECT_EQ(Json::Value (0).as_int64(), 0);
    M_EXPECT_EQ(Json::Value (false).as_bool(), false);
    M_EXPECT_EQ(Json::Value (true).as_bool(), true);
    M_EXPECT_EQ(Json::Value (nullptr).is_null(), true);
    M_EXPECT_EQ(Json::Value {1}.as_int64(), 1);
    M_EXPECT_EQ(Json::Value {"str"}.as_string(), "str");
    M_EXPECT_FLOAT_EQ(Json::Value {1.11111}.as_ldouble(), 1.11111, 1e-12);
    M_EXPECT_FLOAT_EQ(Json::Value {1.11111}.as_double(), 1.11111, 1e-12);
    M_EXPECT_FLOAT_EQ(Json::Value {1.1f}.as_double(), 1.1f, 1e-4);
    M_EXPECT_EQ(Json::Value {1234567890123456LL}.as_int64(), 1234567890123456LL);
    M_EXPECT_EQ(Json::Value {std::string("hello world")}.as_string(), "hello world");
    M_EXPECT_EQ(Json::Value {0}.as_int64(), 0);
    M_EXPECT_EQ(Json::Value {false}.as_bool(), false);
    M_EXPECT_EQ(Json::Value {true}.as_bool(), true);
    M_EXPECT_EQ(Json::Value {nullptr}.is_null(), true);
}

// 解除类型解析
M_TEST(JsonBase, deserialize) {
    M_EXPECT_EQ(Json::deserialize("null").type(), Json::Type::null);
    M_EXPECT_EQ(Json::deserialize("[]").type(), Json::Type::array);
    M_EXPECT_EQ(Json::deserialize("{}").type(), Json::Type::object);
    M_EXPECT_EQ(Json::deserialize("true").type(), Json::Type::boolean);
    M_EXPECT_EQ(Json::deserialize("false").type(), Json::Type::boolean);
    M_EXPECT_EQ(Json::deserialize("0").type(), Json::Type::number);
    M_EXPECT_EQ(Json::deserialize("1").type(), Json::Type::number);
    M_EXPECT_EQ(Json::deserialize("1.1414e+3").type(), Json::Type::number);
    M_EXPECT_EQ(Json::deserialize("\"1.1414e+3\"").type(), Json::Type::string);

    M_EXPECT_THROW(Json::deserialize(""), Json::StructureException);
    M_EXPECT_THROW(Json::deserialize("  "), Json::StructureException);
    M_EXPECT_THROW(Json::deserialize("True"), Json::StructureException);
    M_EXPECT_THROW(Json::deserialize("False"), Json::StructureException);
    M_EXPECT_THROW(Json::deserialize("\""), Json::StructureException);
    M_EXPECT_THROW(Json::deserialize("\"\"\""), Json::StructureException);
    M_EXPECT_THROW(Json::deserialize("[]]"), Json::StructureException);
    M_EXPECT_THROW(Json::deserialize("nul"), Json::StructureException);
    M_EXPECT_THROW(Json::deserialize("["), Json::StructureException);
    M_EXPECT_THROW(Json::deserialize("{[}"), Json::StructureException);

    M_EXPECT_EQ(Json::deserialize("null").as_string(), "null");
    M_EXPECT_EQ(Json::deserialize("[]").as_string(), "[]");
    M_EXPECT_EQ(Json::deserialize("{}").as_string(), "{}");
    M_EXPECT_EQ(Json::deserialize("true").as_string(), "true");
    M_EXPECT_EQ(Json::deserialize("true").as_bool(), true);
    M_EXPECT_EQ(Json::deserialize("false").as_bool(), false);
    M_EXPECT_EQ(Json::deserialize("0").as_int64(), 0);
    M_EXPECT_EQ(Json::deserialize("0").as_string(), "0");
    M_EXPECT_EQ(Json::deserialize("1").as_int64(), 1);
    M_EXPECT_EQ(Json::deserialize("\"1.1414e+3\"").as_string(), "1.1414e+3");
}


std::string read_file(const std::string& path) {
    const std::ifstream file(path);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file " + path);
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

// 序列化测试
M_TEST(JsonBase, serialize) {
    M_EXPECT_EQ(Json::deserialize(read_file("files/medium_1.json")).serialize(), read_file("files/medium_1_plain.json"));
    M_EXPECT_EQ(Json::deserialize(read_file("files/senior_1.json")).serialize(), read_file("files/senior_1_plain.json"));
    M_EXPECT_EQ(Json::deserialize(read_file("files/senior_2.json")).serialize(), read_file("files/senior_2_plain.json"));
    M_EXPECT_EQ(Json::deserialize(read_file("files/simple_1.json")).serialize(), read_file("files/simple_1_plain.json"));
    M_EXPECT_EQ(Json::deserialize(read_file("files/simple_2.json")).serialize(), read_file("files/simple_2_plain.json"));
    M_EXPECT_EQ(Json::deserialize(read_file("files/simple_3.json")).serialize(), read_file("files/simple_3_plain.json"));

    M_EXPECT_NO_THROW(Json::deserialize(read_file("files/medium_1.json")).serialize_pretty());
    M_EXPECT_NO_THROW(Json::deserialize(read_file("files/senior_2.json")).serialize_pretty());
    M_EXPECT_NO_THROW(Json::deserialize(read_file("files/simple_3.json")).serialize_pretty());
}

// 序列化测试
M_TEST(JsonBase, object) {
    M_EXPECT_EQ(Json::Value(Json::Object{}).serialize(), Json::Value(Json::Type::object).serialize());
    M_EXPECT_EQ(Json::Value(Json::Object{}).type(), Json::Value(Json::Type::object).type());
    M_EXPECT_NO_THROW(Json::Value(Json::Object{}).as_object());
    Json::Value v(Json::Object{});

}

// 序列化测试
M_TEST(JsonBase, array) {
    M_EXPECT_EQ(Json::Value(Json::Array{}).serialize(), Json::Value(Json::Type::array).serialize());
    M_EXPECT_EQ(Json::Value(Json::Array{}).type(), Json::Value(Json::Type::array).type());
    M_EXPECT_NO_THROW(Json::Value(Json::Array{}).as_array());
    Json::Value v(Json::Array{});
}

// clear和reset函数测试
M_TEST(JsonBase, clear) {

    Json::Value v0;
    Json::Value v1{};
    Json::Value v2(Json::Type::object);
    Json::Value v3(Json::Type::array);
    Json::Value v4(Json::Type::string);
    Json::Value v5(Json::Type::number);
    Json::Value v6(Json::Type::boolean);
    Json::Value v7(Json::Type::null);
    M_EXPECT_NO_THROW( v0.clear() );
    M_EXPECT_NO_THROW( v1.clear() );
    M_EXPECT_NO_THROW( v2.clear() );
    M_EXPECT_NO_THROW( v3.clear() );
    M_EXPECT_NO_THROW( v4.clear() );
    M_EXPECT_NO_THROW( v5.clear() );
    M_EXPECT_NO_THROW( v6.clear() );
    M_EXPECT_NO_THROW( v7.clear() );
    M_EXPECT_EQ(v0.type(), Json::Type::null);
    M_EXPECT_EQ(v1.type(), Json::Type::null);
    M_EXPECT_EQ(v2.type(), Json::Type::object);
    M_EXPECT_EQ(v3.type(), Json::Type::array);
    M_EXPECT_EQ(v4.type(), Json::Type::string);
    M_EXPECT_EQ(v5.type(), Json::Type::number);
    M_EXPECT_EQ(v6.type(), Json::Type::boolean);
    M_EXPECT_EQ(v7.type(), Json::Type::null);
    M_EXPECT_EQ(v0.serialize(), "null");
    M_EXPECT_EQ(v1.serialize(), "null");
    M_EXPECT_EQ(v2.serialize(), "{}");
    M_EXPECT_EQ(v3.serialize(), "[]");
    M_EXPECT_EQ(v4.serialize(), "\"\"");
    M_EXPECT_EQ(v5.serialize(), "0");
    M_EXPECT_EQ(v6.serialize(), "false");
    M_EXPECT_EQ(v7.serialize(), "null");
    M_EXPECT_EQ(v0.is_null(), true);
    M_EXPECT_EQ(v1.is_null(), true);
    M_EXPECT_EQ(v4.as_string(), "");
    M_EXPECT_EQ(v5.as_int64(), 0);
    M_EXPECT_EQ(v6.as_bool(), false);
    M_EXPECT_EQ(v7.is_null(), true);

    M_EXPECT_NO_THROW( v0.reset() );
    M_EXPECT_NO_THROW( v1.reset() );
    M_EXPECT_NO_THROW( v2.reset() );
    M_EXPECT_NO_THROW( v3.reset() );
    M_EXPECT_NO_THROW( v4.reset() );
    M_EXPECT_NO_THROW( v5.reset() );
    M_EXPECT_NO_THROW( v6.reset() );
    M_EXPECT_NO_THROW( v7.reset() );
    M_EXPECT_EQ(v0.type(), Json::Type::null);
    M_EXPECT_EQ(v1.type(), Json::Type::null);
    M_EXPECT_EQ(v2.type(), Json::Type::null);
    M_EXPECT_EQ(v3.type(), Json::Type::null);
    M_EXPECT_EQ(v4.type(), Json::Type::null);
    M_EXPECT_EQ(v5.type(), Json::Type::null);
    M_EXPECT_EQ(v6.type(), Json::Type::null);
    M_EXPECT_EQ(v7.type(), Json::Type::null);
    M_EXPECT_EQ(v0.serialize(), "null");
    M_EXPECT_EQ(v1.serialize(), "null");
    M_EXPECT_EQ(v2.serialize(), "null");
    M_EXPECT_EQ(v3.serialize(), "null");
    M_EXPECT_EQ(v4.serialize(), "null");
    M_EXPECT_EQ(v5.serialize(), "null");
    M_EXPECT_EQ(v6.serialize(), "null");
    M_EXPECT_EQ(v7.serialize(), "null");
}

// 增删改查
M_TEST(JsonBase, CURD) {
    Json::Value v = Json::deserialize(read_file("files/medium_1.json"));
    M_EXPECT_EQ(v.type(), Json::Type::object);
    M_EXPECT_EQ(v.size(), 4);
    M_EXPECT_EQ(v["version"].type(), Json::Type::number);
    M_EXPECT_EQ(v["version"].as_int64(), 5);
    M_EXPECT_EQ(v["configurePresets"].type(), Json::Type::array);
    M_EXPECT_EQ(v["configurePresets"][0].type(), Json::Type::object);
    M_EXPECT_EQ(v["configurePresets"][0].at("name").type(), Json::Type::string);
    M_EXPECT_EQ(v["configurePresets"][0].at("name").as_string(), "Ninja-config");
    M_EXPECT_EQ(v["configurePresets"][1].at("inherits").type(), Json::Type::array);
    M_EXPECT_EQ(v["configurePresets"][1].at("inherits")[1].type(), Json::Type::string);
    M_EXPECT_EQ(v["configurePresets"][1].at("inherits")[1].as_string(), "Qt-Default");
    M_EXPECT_EQ(v["configurePresets"][2].at("hidden").as_bool(), true);
    M_EXPECT_EQ(v.at("version").type(), Json::Type::number);
    M_EXPECT_EQ(v.at("version").as_int64(), 5);
    M_EXPECT_EQ(v.at("configurePresets").type(), Json::Type::array);
    M_EXPECT_EQ(v.at("configurePresets").at(0).type(), Json::Type::object);
    M_EXPECT_EQ(v.at("configurePresets").at(0)["name"].type(), Json::Type::string);
    M_EXPECT_EQ(v.at("configurePresets").at(0)["name"].as_string(), "Ninja-config");
    M_EXPECT_EQ(v.at("configurePresets").at(1)["inherits"].type(), Json::Type::array);
    M_EXPECT_EQ(v.at("configurePresets").at(1)["inherits"][1].type(), Json::Type::string);
    M_EXPECT_EQ(v.at("configurePresets").at(1)["inherits"][1].as_string(), "Qt-Default");
    M_EXPECT_EQ(v.at("configurePresets").at(2)["hidden"].as_bool(), true);

    M_EXPECT_THROW(v.at("null_key"),std::out_of_range);
    M_EXPECT_THROW(v["configurePresets"].at(8),std::out_of_range);
    M_EXPECT_THROW(v["configurePresets"].at(-1),std::out_of_range);
    M_EXPECT_THROW(v.at(1),Json::TypeException);
    M_EXPECT_THROW(v[1],Json::TypeException);
    M_EXPECT_NO_THROW(v["null_key"]);
    M_EXPECT_EQ(v["null_key"].type(), Json::Type::null);
    M_EXPECT_NO_THROW(v.insert("key3","test_key3"));
    M_EXPECT_NO_THROW(v.insert("key4",8848));
    M_EXPECT_NO_THROW(v.insert("key5",nullptr));
    M_EXPECT_EQ(v["key4"].type(), Json::Type::number);
    M_EXPECT_EQ(v["key5"].type(), Json::Type::null);
    M_EXPECT_EQ(v["key3"].as_string(), "test_key3");
    M_EXPECT_NO_THROW(v["configurePresets"].pop_back());
    M_EXPECT_EQ(v["configurePresets"].size(), 4);
    M_EXPECT_NO_THROW(v["configurePresets"].push_back(false));
    M_EXPECT_EQ(v["configurePresets"].size(), 5);
    M_EXPECT_EQ(v["configurePresets"][4].as_bool(), false);
    M_EXPECT_NO_THROW(v["configurePresets"].erase(3));
    M_EXPECT_EQ(v["configurePresets"][3].as_bool(), false);
    M_EXPECT_EQ(v["configurePresets"].size(), 4);
    M_EXPECT_EQ(v.contains("null_key"), true);
    M_EXPECT_NO_THROW(v.erase("null_key"));
    M_EXPECT_EQ(v.contains("null_key"), false);
}

// 初始化列表
M_TEST(JsonAdvance, initializer) {
    Json::Value v0 = {};
    Json::Value v1 = { 1 };
    Json::Value v2 = { 2, 3 };
    Json::Value v3 = { 4, 5, true, false, nullptr };
    Json::Value v4 = { "key", "value" };
    Json::Value v5 = { "key0", { "key1", { "key2", 1 } } };
    Json::Value v6 = { "key0", "value", 313 };
    Json::Value v7 = { { "key", 3.1415926}, { "?" }, { { { 1 } } }, nullptr };
    M_EXPECT_EQ(v0.type(), Json::Type::null);
    M_EXPECT_EQ(v1.type(), Json::Type::number);
    M_EXPECT_EQ(v2.type(), Json::Type::array);
    M_EXPECT_EQ(v3.type(), Json::Type::array);
    M_EXPECT_EQ(v4.type(), Json::Type::object);
    M_EXPECT_EQ(v5.type(), Json::Type::object);
    M_EXPECT_EQ(v6.type(), Json::Type::array);
    M_EXPECT_EQ(v7.type(), Json::Type::array);
    M_EXPECT_EQ(v7.size(), 4);
    M_EXPECT_EQ(v7[0].type(), Json::Type::object);
    M_EXPECT_EQ(v7[1].type(), Json::Type::string);
    M_EXPECT_EQ(v7[2].type(), Json::Type::number);
    M_EXPECT_EQ(v7[3].type(), Json::Type::null);
    M_EXPECT_EQ(v7[0]["key"].type(), Json::Type::number);
    M_EXPECT_EQ(v7[0]["key"].as_int64(), 3);
    M_EXPECT_FLOAT_EQ(v7[0]["key"].as_double(), 3.1415926, 1e-5);



    Json::Value v8 = Json::Object{
        { "key0",Json::Object{{ "key1",{ "key2", 1 } } } },
        {"key1", Json::Array{"key4", "value4"}},
        {"key2", {"key4", "value4"}}
    };
    M_EXPECT_EQ(v8.type(), Json::Type::object);
    M_EXPECT_TRUE(v8.contains("key0"));
    M_EXPECT_TRUE(v8.contains("key1"));
    M_EXPECT_TRUE(v8.contains("key2"));
    M_EXPECT_FALSE(v8.contains("key3"));

    M_EXPECT_EQ(v8["key1"].type(), Json::Type::array);
    M_EXPECT_EQ(v8["key2"].type(), Json::Type::object);
}

struct A {
    std::string serialize() const {
        return "A";
    }
};

// 序列化
M_TEST(JsonAdvance, serializable) {
    Json::Value j0 = {1,4,5,2};
    M_EXPECT_NO_THROW( Json::serialize(j0) );
    Json::Array j1 = { 1, 2, "string", true, false, nullptr};
    M_EXPECT_NO_THROW( Json::serialize(j1) );
    Json::Object j2= { {"1", 1}, {"2", 22}, {"3", false}};
    M_EXPECT_NO_THROW( Json::serialize(j2) );

    M_EXPECT_NO_THROW( Json::serialize("string") );
    M_EXPECT_NO_THROW( Json::serialize(std::string{"string"}) );
    M_EXPECT_NO_THROW( Json::serialize(1 ) );
    M_EXPECT_NO_THROW( Json::serialize(2222.22 ) );
    M_EXPECT_NO_THROW( Json::serialize(2222.22f ) );
    M_EXPECT_NO_THROW( Json::serialize(true ) );
    M_EXPECT_NO_THROW( Json::serialize(false ) );
    M_EXPECT_NO_THROW( Json::serialize( nullptr ) );

    std::vector<int> v0 = { 1, 2, 3, 4};
    std::vector<bool> v1 = { true, false, true, false};
    std::vector<std::string> v2 = { "1", "2", "3", "4"};
    std::list<std::string> v3 = { "1", "2", "3", "4"};
    std::deque<double> v4 = { 1, 2.2, 3, -4};
    std::list<Json::Value> v5 = { "1", 2, false, nullptr};
    M_EXPECT_NO_THROW( Json::serialize( v0 ) );
    M_EXPECT_NO_THROW( Json::serialize( v1 ) );
    M_EXPECT_NO_THROW( Json::serialize( v2 ) );
    M_EXPECT_NO_THROW( Json::serialize( v3 ) );
    M_EXPECT_NO_THROW( Json::serialize( v4 ) );
    M_EXPECT_NO_THROW( Json::serialize( v5 ) );
    M_EXPECT_EQ( Json::serialize( v0 ), "[1,2,3,4]" );
    M_EXPECT_EQ( Json::serialize( v1 ), "[true,false,true,false]" );
    M_EXPECT_EQ( Json::serialize( v2 ), R"(["1","2","3","4"])" );
    M_EXPECT_EQ( Json::serialize( v3 ), R"(["1","2","3","4"])" );
    M_EXPECT_EQ( Json::serialize( v5 ), R"(["1",2,false,null])" );

    // 失败测试↓
    // M_EXPECT_EQ( Json::serialize( v5 ), R"(["1",2,false,nul])" );

    std::map<std::string, std::string> m0 = { {"1", "2"}, {"3", "4"}, {"5", "6"}};
    std::unordered_map<std::string, std::string> m1 = { {"1", "2"}, {"3", "4"}, {"5", "6"}};
    std::map<std::string, int> m2 = { {"1", 2}, {"3", 4}, {"5", 6}};
    std::unordered_map<std::string, bool> m3 = { {"1", true}, {"3", false}, {"5", true}};
    std::unordered_map<std::string, Json::Value> m4 = { {"1", true}, {"3", 1}, {"5", nullptr}, {"6", "7"}};
    M_EXPECT_NO_THROW( Json::serialize( m0 ) );
    M_EXPECT_NO_THROW( Json::serialize( m1 ) );
    M_EXPECT_NO_THROW( Json::serialize( m2 ) );
    M_EXPECT_NO_THROW( Json::serialize( m3 ) );
    M_EXPECT_NO_THROW( Json::serialize( m4 ) );
    M_EXPECT_EQ( Json::serialize( m0 ), R"({"1":"2","3":"4","5":"6"})" );
    M_EXPECT_EQ( Json::serialize( m2 ), R"({"1":2,"3":4,"5":6})" );

    A a;
    std::vector<A> va(5);
    std::map<std::string, A> ma ={ { "1", {} }, {"2", {} }, {"3", {}}, {"4", {}}, {"5", {}} };
    M_EXPECT_EQ( Json::serialize( a ), "A" );
    M_EXPECT_EQ( Json::serialize( va ), "[A,A,A,A,A]" );
    M_EXPECT_EQ( Json::serialize( ma ), R"({"1":A,"2":A,"3":A,"4":A,"5":A})" );
}



struct B{
    std::string name;
    int value;
    bool check;
    // 编写类型转换运算符
    operator Json::Value() const {
        Json::Value result(Json::Type::object);
        result["name"] = name;
        result["value"] = value;
        result["check"] = check;
        return result;
    };
};
// 测试README.md中的样例代码
M_TEST(README, md) {
    {
        Json::Value json = {
            { "key", nullptr },
            { 1, 2, 3, 4, 5 },
            "string",
            true,
            false,
            1234.5
        };
    }

    {
        Json::Value json = Json::Array{
            Json::Object { {"key", nullptr} },
            Json::Array{ 1, 2, 3, 4, 5 },
            "string",
            true,
            false,
            1234.5
        };
    }

    {
        B b;
        Json::serialize(b);
    }

}
