#define M_VCT_JSON_SIMPLIFY_MACROS
#include <vct/tools/json_macros.hpp>
import std;
import vct.tools.json;

using namespace vct::tools;

struct MyData{
    int id{};
    std::string m_name{};
    bool active{false};
    double m_value{};

    MyData() = default; // default constructor is needed

    M_JSON_CV_FUN( MyData,  // 转换函数，必须在 public 作用域
        M_JSON_CV_MEM( id );    // 注意，MyData 后面必须有 `,`
        M_JSON_CV_MAP( name, m_name )   // 但是剩余的字段后面不能有逗号 `,` ，分号 `;` 则是可选的
        M_JSON_CV_MEM( active )
        M_JSON_CV_MAP( value, m_value )
    )
    M_JSON_CS_FUN( MyData,  // 构造函数，必须在 public 作用域
        M_JSON_CS_MEM( id )
        M_JSON_CS_MAP( name, m_name )
        M_JSON_CS_MEM_OR( active, true, nullptr ) // default value is `true`, nullptr means no range value
        M_JSON_CS_MAP_OR( value, m_value, 64.0, nullptr ) // default value is `true`, nullptr means no range value
    )
};

struct MyData2 {
    std::string name;   // std::string 等于 json::String，因此可以直接使用
    MyData my_data;     // MyData 已经有转换函数和构造函数，因此可以直接使用
    std::vector<MyData> data_list;  // 能够直接使用的类型构成的列表也能直接使用（但再套一层列表就不行了）
    MyData2() = default;
    M_JSON_CV_FUN( MyData2,
        M_JSON_CV_MEM( name )
        M_JSON_CV_MAP( data, my_data )
        M_JSON_CV_MEM( data_list )
    )
    M_JSON_CS_FUN( MyData2,
        M_JSON_CS_MEM( name )
        M_JSON_CS_MAP( data, my_data )
        M_JSON_CS_MEM_OR( data_list, std::vector<MyData>{}, MyData{} ) // 变量名，默认值，内部子元素的默认值
    )
};

int main() {

    std::println("===================README======================");
    json::Value smp_val = json::Object{
        { "key1", 42 },
        {"key2", "value2"},
        {"key3", true },
        {"arr", json::Array{ { 2, 3.14, nullptr } } },
        {"obj", json::Object{ {"nested_k", "nested_v"} } }
    };
    smp_val.type(); // 返回 Type::eObject
    smp_val.is(json::Type::eArray);
    smp_val.type_name();// 返回 "Object" ,没有前缀 `e`

    json::Value vi_42 = smp_val.get<json::Object>()["key1"]; // 注意 Object 的 mapped 值依然是 Value 类型
    double i_42 = vi_42.get<json::Number>(); // 获取内部值, json::Number 等价于 double

    auto str_view = smp_val["key2"].to<std::string_view>(); // 返回内部字符串的视图
    auto str = smp_val["key2"].move<std::string>(); // 将内部字符串移动了出来，现在内部变为空字符串，之前的视图不再可用
    int int_42 = smp_val["key1"].to<int>(); // 返回内部整数的拷贝

    auto opt_str = smp_val["key1"].to_if<std::string>(); // opt_str 是 std::optional<std::string> ，转换失败为空，但不抛出异常
    if(opt_str) std::cout << *opt_str << std::endl; // 如果转换成功，输出字符串

    std::string json_str1 = R"( [ 1, false, null, { "Hello": "World" } ] )";
    std::string json_str2 = R"( false )"; // 允许顶层类型是任一 JSON 类型
    json::Value val1 = json::deserialize(json_str1).value_or( nullptr ); // 解析 JSON 字符串
    std::cout << val1[1].to<bool>();; // 输出 0 （没有指定 boolalpha）

    std::string str_ser = val1.serialize(); // 不含无效空白字符的高效序列化，返回 std::string
    std::string str_back;
    val1.serialize_to( str_back ); // 将序列化结果写入 std::string 的末尾
    val1.serialize_to( std::cout ); // 将序列化结果直接输出到 `ostream` 中

    auto pretty_str = val1.serialize_pretty().value_or( "fail" );
    bool res = val1.serialize_pretty_to( std::cout ); // 输出到 `ostream`，返回是否成功
    if (!res){ std::cerr << "fail"; }

    json::Value val_arr_1 = json::Array{{ 1, 2, 3 }};
    json::Value val_arr_2 = json::Array{{ 1, 2, 3 }};
    json::Value val_arr_3 = json::Array{{ 1, true, 3 }};
    val_arr_1 == val_arr_2; // true
    val_arr_1 == val_arr_3; // false



    {
        json::Value v_null;
        MyData d_null{ v_null }; // 什么都没有，因此全部字段都是默认值
        d_null.active; // true，因为构造函数指定了默认值为 true

        json::Value v_object{ json::Type::eObject };
        v_object["id"] = 42;
        v_object["name"] = "Test User";
        v_object["active"] = false;
        v_object["value"] = 128.0;
        MyData d_object{ v_object };
        d_object.m_name == "Test User"; // true

        json::Value v_data{ d_object }; // 将 MyData 转换为 JSON 对象
        v_data["id"] = 42; // true
    }

    {
        MyData2 data2;
        json::Value v_data2{ data2 };
        v_data2.serialize_pretty_to( std::cout );
    }

    std::println("==============================================");
}
