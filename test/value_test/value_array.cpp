#include <vct/test_unit_macros.hpp>

import std;
import vct.test.unit;
import vct.tools.json;

using namespace vct::tools;

// Array1: 基础构造与访问
M_TEST(Value, Array) {
    M_ASSERT_NO_THROW(json::Value arr_default{ json::Array{} });
    M_ASSERT_EQ(json::Value{json::Type::eArray}.type(), json::Type::eNumber);
    M_ASSERT_FALSE(json::Value{json::Type::eArray}.is_arr());
    M_ASSERT_TRUE(json::Value{json::Type::eArray}.is_num());
    M_ASSERT_EQ(json::Value{json::Array{}}.size(), 0);
    M_ASSERT_TRUE(json::Value{json::Array{}}.to<json::Array>().empty());
    M_ASSERT_NO_THROW(json::Value arr_explicit{json::Array{{}}});
    M_ASSERT_NO_THROW(json::Value arr_init{json::Array{{1, 2, 3}}});
    json::Value arr_val{json::Array{{1, 2, 3}}};
    M_ASSERT_TRUE(arr_val.is_arr());
    M_ASSERT_FALSE(arr_val.is_str());
    M_EXPECT_STREQ(arr_val.type_name(), "Array");


    // Array2: 简单与混合类型访问

    json::Value simple_arr{json::Array{{10, 20, 30}}};
    M_ASSERT_EQ(simple_arr.type(), json::Type::eArray);
    M_ASSERT_EQ(simple_arr.size(), 3);
    M_ASSERT_EQ(simple_arr[0].to<json::Number>(), 10);
    M_ASSERT_EQ(simple_arr[1].to<json::Number>(), 20);
    M_ASSERT_EQ(simple_arr[2].to<json::Number>(), 30);

    json::Value mixed_arr{json::Array{{42, "hello", true, nullptr}}};
    M_ASSERT_EQ(mixed_arr.size(), 4);
    M_ASSERT_EQ(mixed_arr[0].to<json::Number>(), 42);
    M_ASSERT_EQ(mixed_arr[1].to<json::String>(), "hello");
    M_ASSERT_EQ(mixed_arr[2].to<json::Bool>(), true);
    M_ASSERT_EQ(mixed_arr[3].type(), json::Type::eNull);

    const json::Value const_arr{json::Array{{1, 2, 3}}};
    M_ASSERT_EQ(const_arr[2].to<json::Number>(), 3);


    // Array3: 元素修改与赋值
    json::Value modify_test{json::Array{{1, 2, 3}}};
    modify_test[0] = 100;
    modify_test[1] = "modified";
    modify_test[2] = false;
    M_ASSERT_EQ(modify_test[0].to<json::Number>(), 100);
    M_ASSERT_EQ(modify_test[1].to<json::String>(), "modified");
    M_ASSERT_EQ(modify_test[2].to<json::Bool>(), false);

    json::Value assign_val{json::Array{}};
    M_ASSERT_NO_THROW(assign_val = json::Array{{1, 2, 3}});
    M_ASSERT_EQ(assign_val.size(), 3);
    M_ASSERT_EQ(assign_val[0].to<json::Number>(), 1);
    M_ASSERT_NO_THROW(assign_val = json::Array{{10, 20}});
    M_ASSERT_EQ(assign_val.size(), 2);
    M_ASSERT_EQ(assign_val[0].to<json::Number>(), 10);
    M_ASSERT_EQ(assign_val[1].to<json::Number>(), 20);


    // Array4: 安全访问与异常
    json::Value at_test{json::Array{{100, 200, 300}}};
    M_ASSERT_NO_THROW(std::ignore = at_test.at(0));
    M_ASSERT_NO_THROW(std::ignore = at_test.at(1));
    M_ASSERT_NO_THROW(std::ignore = at_test.at(2));
    M_ASSERT_EQ(at_test.at(1).to<json::Number>(), 200);
    M_ASSERT_THROW(std::ignore = at_test.at(3), std::out_of_range);

    const json::Value const_at_test{json::Array{{1, 2, 3}}};
    M_ASSERT_EQ(const_at_test.at(1).to<json::Number>(), 2);

    json::Value wrong_type_val{42};
    M_ASSERT_THROW(std::ignore = wrong_type_val.get_arr(), std::bad_variant_access);

    json::Value string_val{"not array"};
    M_ASSERT_THROW(std::ignore = string_val.get_arr(), std::bad_variant_access);


    // Array5: get<T>引用与容器操作
    json::Value get_test{json::Array{{1, 2, 3}}};
    M_ASSERT_NO_THROW(std::ignore = get_test.to<json::Array>());
    auto& arr_ref = get_test.get_arr();
    arr_ref[0] = 100;
    arr_ref.push_back(40);
    M_ASSERT_EQ(get_test.size(), 4);
    M_ASSERT_EQ(get_test[0].to<json::Number>(), 100);
    M_ASSERT_EQ(get_test[3].to<json::Number>(), 40);

    const json::Value const_ref_test{json::Array{{1, 2, 3}}};
    M_ASSERT_NO_THROW(std::ignore = const_ref_test.get_arr());
    const auto& const_arr_ref = const_ref_test.get_arr();
    M_ASSERT_EQ(const_arr_ref.size(), 3);
    M_ASSERT_EQ(const_arr_ref[0].to<json::Number>(), 1);


    // Array6: 多维嵌套结构
    json::Value nested_2d{json::Array{{
        json::Array{{1, 2, 3}},
        json::Array{{4, 5, 6}},
        json::Array{{7, 8, 9}}
    }}};
    M_ASSERT_EQ(nested_2d.type(), json::Type::eArray);
    M_ASSERT_EQ(nested_2d.size(), 3);
    M_ASSERT_EQ(nested_2d[1][0].to<json::Number>(), 4);

    json::Value nested_3d{json::Array{{
        json::Array{{
            json::Array{{1, 2}},
            json::Array{{3, 4}}
        }},
        json::Array{{
            json::Array{{5, 6}},
            json::Array{{7, 8}}
        }}
    }}};
    M_ASSERT_EQ(nested_3d.size(), 2);
    M_ASSERT_EQ(nested_3d[0].size(), 2);
    M_ASSERT_EQ(nested_3d[0][0].size(), 2);
    M_ASSERT_EQ(nested_3d[0][1][1].to<json::Number>(), 4);


// Array7: 混合嵌套与不规则结构
    json::Value mixed_nested{json::Array{{
        json::Array{{1, 2, 3}},
        json::String("nested string"),
        json::Array{{
            json::Bool(true),
            json::Array{{4, 5}},
            nullptr
        }}
    }}};
    M_ASSERT_EQ(mixed_nested.size(), 3);
    M_ASSERT_EQ(mixed_nested[0].size(), 3);
    M_ASSERT_EQ(mixed_nested[1].type(), json::Type::eString);
    M_ASSERT_EQ(mixed_nested[2].size(), 3);
    M_ASSERT_EQ(mixed_nested[2][1][1].to<json::Number>(), 5);

    json::Value irregular{json::Array{{
        json::Array{{1}},
        json::Array{{2, 3}},
        json::Array{{4, 5, 6}},
        json::Array{{7, 8, 9, 10}}
    }}};
    M_ASSERT_EQ(irregular.size(), 4);
    M_ASSERT_EQ(irregular[0].size(), 1);
    M_ASSERT_EQ(irregular[1].size(), 2);
    M_ASSERT_EQ(irregular[2].size(), 3);
    M_ASSERT_EQ(irregular[3].size(), 4);
    M_ASSERT_EQ(irregular[1][1].to<json::Number>(), 3);


// Array8: 比较与序列化
    json::Value arr1{json::Array{{1, 2, 3}}};
    json::Value arr2{json::Array{{1, 2, 3}}};
    json::Value arr3{json::Array{{1, 2, 4}}};
    M_ASSERT_TRUE(arr1 == arr2);
    M_ASSERT_FALSE(arr1 == arr3);
    M_ASSERT_TRUE(arr1 != arr3);

    json::Value empty1{json::Array{{}}};
    json::Value empty2{json::Array{{}}};
    M_ASSERT_TRUE(empty1 == empty2);

    json::Value short_arr{ json::Array{{1, 2}}};
    json::Value long_arr{ json::Array{{1, 2, 3}}};
    M_ASSERT_FALSE(short_arr == long_arr);

    json::Value nested1{json::Array{{
        json::Array{{1, 2}},
        json::Array{{3, 4}}
    }}};
    json::Value nested2{json::Array{{
        json::Array{{1, 2}},
        json::Array{{3, 4}}
    }}};
    json::Value nested3{json::Array{{
        json::Array{{1, 2}},
        json::Array{{3, 5}}
    }}};
    M_ASSERT_TRUE(nested1 == nested2);
    M_ASSERT_FALSE(nested1 == nested3);

    json::Value simple3_arr{json::Array{{1, 2, 3}}};
    M_ASSERT_EQ(simple3_arr.serialize(), "[1,2,3]");

    // empty must be use json::Array{} to avoid ambiguity with json::Value
    json::Value empty_serial{json::Array{}};
    M_ASSERT_EQ(empty_serial.serialize(), "[]");

    json::Value empty_serial_2{json::Array{{} }};
    M_ASSERT_EQ(empty_serial_2.serialize(), "[null]");

    json::Value mixed{json::Array{{42, "test", true, nullptr}}};
    M_ASSERT_EQ(mixed.serialize(), "[42,\"test\",true,null]");
    json::Value nested{json::Array{{
        json::Array{{1, 2}},
        json::Array{{3, 4}}
    }}};
    M_ASSERT_EQ(nested.serialize(), "[[1,2],[3,4]]");


// Array9: 解析与序列化回环
    auto parsed_simple = json::parse("[1,2,3]");
    if (parsed_simple.has_value()) {
        M_ASSERT_EQ(parsed_simple->type(), json::Type::eArray);
        M_ASSERT_EQ(parsed_simple->size(), 3);
        M_ASSERT_EQ((*parsed_simple)[0].to<json::Number>(), 1);
        M_ASSERT_EQ((*parsed_simple)[2].to<json::Number>(), 3);
    } else {
        M_ASSERT_FAIL("Failed to parse simple array");
    }

    json::Value simple2_arr{json::Array{{1, 2, 3}}};
    auto serialized_arr = simple2_arr.serialize();
    auto parsed_back_arr = json::parse(serialized_arr);
    if (parsed_back_arr.has_value()) {
        M_ASSERT_TRUE(*parsed_back_arr == simple2_arr);
    } else {
        M_ASSERT_FAIL("Failed to parse back serialized array");
    }


// Array10: 多维混合结构与边界
    json::Value simplified_multi_dim{json::Array{{
        json::Array{{
            json::Array{{1, 2, 3}},
            json::Array{{4, 5, 6}},
            json::Array{{7, 8, 9}}
        }},
        json::Array{{
            json::Array{{"a", "b", "c"}},
            json::Array{{"d", "e", "f"}},
            json::Array{{"g", "h", "i"}}
        }},
        json::Array{{
            json::Array{{true, false, nullptr}},
            json::Array{{100, "mixed", 3.14}}
        }}
    }}};
    M_ASSERT_EQ(simplified_multi_dim.size(), 3);
    M_ASSERT_EQ(simplified_multi_dim[0].size(), 3);
    M_ASSERT_EQ(simplified_multi_dim[1].size(), 3);
    M_ASSERT_EQ(simplified_multi_dim[2].size(), 2);
    M_ASSERT_EQ(simplified_multi_dim[0][0][0].to<json::Number>(), 1);
    M_ASSERT_EQ(simplified_multi_dim[1][1][1].to<json::String>(), "e");
    M_ASSERT_EQ(simplified_multi_dim[2][0][2].type(), json::Type::eNull);
    M_ASSERT_EQ(simplified_multi_dim[2][0][2].is_nul(), true);
    M_ASSERT_EQ(simplified_multi_dim[2][1][1].to<json::String>(), "mixed");
    M_ASSERT_EQ(simplified_multi_dim[2][1][2].to<json::Number>(), 3.14);
    M_ASSERT_THROW(std::ignore = simplified_multi_dim.at(3), std::out_of_range);
    M_ASSERT_THROW(std::ignore = simplified_multi_dim.at(0).at(3), std::out_of_range);
    M_ASSERT_THROW(std::ignore = simplified_multi_dim.at(1).at(0).at(3), std::out_of_range);
    M_ASSERT_THROW(std::ignore = simplified_multi_dim.at(2).at(2), std::out_of_range);
}
