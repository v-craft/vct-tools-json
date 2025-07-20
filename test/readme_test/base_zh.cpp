#include <vct/test_unit_macros.hpp>

import std;
import vct.test.unit;
import vct.tools.json;

using namespace vct::tools;

M_TEST( README, zh_base ) {
    {
        json::Value value;
        // std::cout << value.type() == json::Type::eNull << std::endl; // 输出 1
        // std::cout << value.type_name() << std::endl; // 输出 "Null"
        M_EXPECT_TRUE( (value.type() == json::Type::eNull));
        M_EXPECT_STREQ(value.type_name(), "Null");
    }

    {
        json::Value value(json::Type::eNumber); // 创建一个数值类型的 Value
        M_EXPECT_EQ( value.type(), json::Type::eNumber ); // 检查类型是否为数值
        M_EXPECT_STREQ(value.type_name(), "Number"); // 检查类型名称是否为 "Number"
        value = json::Type::eArray; // 强制转换为数组类型，并重载内部数据
        M_EXPECT_EQ( value.type(), json::Type::eArray ); // 检查类型是否为数组
        M_EXPECT_STREQ(value.type_name(), "Array"); // 检查类型名称是否为 "Array"
        value.clear_data(); // 清空内部数据，但不修改类型。比如数组会变成空数组。
        M_EXPECT_EQ( value.type(), json::Type::eArray ); // 检查类型是否为数组
        value.reset(); // 重置为 null 类型，并清空内部数据。
        M_EXPECT_EQ( value.type(), json::Type::eNull ); // 检查类型是否为 null
    }

    {
        json::Value smp_val = json::Object{
                { "key1", 42 },
                {"key2", "value2"},
                {"key3", true },
                {"arr", json::Array{ { 2, 3, 4 } } },
                {"obj", json::Object{ {"nested_key", "nested_value"} } }
        };

        json::Value val = json::Array{ json::Array{ json::Array{ 1 } } };
        M_EXPECT_EQ( val.type(), json::Type::eArray );
        M_EXPECT_EQ( val[0].type(), json::Type::eNumber );

        M_EXPECT_EQ(smp_val["arr"][1], 3); // 输出 3
    }

    {
        json::Value str = "Hello, World!";
        auto& inner_str = str.get<json::String>();
        inner_str = "new, str!";
        M_EXPECT_EQ( str.get<json::String>(), "new, str!" );

    }

    {
        std::string str = R"( [1,  false, { "k": null } ]  )";
        std::expected<json::Value, json::ParseError> result = json::deserialize(str); // or json::parse(str);
        if (result.has_value()) {
            (*result)[0].get<json::Number>() = 2; // 修改第一个元素
            M_EXPECT_STREQ(result->serialize(), R"([2,false,{"k":null}])");
            std::println("pretty output: \n{}", result->serialize_pretty().value_or("fail"));
        }

        std::ignore = json::deserialize(str).value_or( nullptr );


    }



}
