#include <vct/test_unit_macros.hpp>
#define M_VCT_TOOLS_JSON_SIMPLIFY_MACROS
#include <vct/tools/json_macros.hpp>
import std;
import vct.test.unit;
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


M_TEST( README, readme ) {
    // std::println("===================README======================");
    //
    // {
    //     json::Value null_val;                  // 默认构造，类型为 Null
    //     json::Value bool_val(3.3);             // 浮点初始化，类型为 Number
    //     json::Value obj_val = json::Object{};  // 直接使用 Object 初始化
    //     json::Value enum_val{ json::Type::eObject }; // 危险
    //     // 这会生成一个 Number 类型的值，具体值取决于枚举值的整数表示。
    // }
    //
    // {
    //     json::Value smp_val = json::Object{
    //     { "key1", 42 },
    //     {"key2", "value2"},
    //     {"key3", true },
    //     {"arr", json::Array{ { 2, 3.14, nullptr } } },
    //     {"obj", json::Object{ {"nested_k", "nested_v"} } }
    //     };
    //
    //     std::ignore = smp_val.type();        // 返回 Type::eObject
    //     std::ignore = smp_val.type_name();  // 返回 "Object" ,没有前缀 `e`
    //     std::ignore = smp_val.is_arr();     // 返回 false
    //
    //     // 注意 Object 的 mapped 依然是 Value 类型
    //     json::Value& vi_42 = smp_val.obj()["key1"];
    //
    //     // 虽然返回引用，但也可以用于赋值
    //     double i_42 = vi_42.num();
    //
    //
    //     std::ignore = smp_val["arr"][1].num(); // 返回 3.14
    //
    //     std::ignore = smp_val.at("obj").at("nested_k") = nullptr; // 修改对象，变为 Null 类型
    //     std::ignore = smp_val["obj"].at("nested_k").is_nul(); // 返回true, [] 和 at 可以随意混合使用
    //     auto str_view = smp_val["key2"].to<std::string_view>(); // 返回内部字符串的视图
    //     auto str = smp_val["key2"].move<std::string>(); // 将内部字符串移动了出来，现在内部变为空字符串，之前的视图不再可用
    //     int int_42 = smp_val["key1"].to<int>(); // 返回内部整数的拷贝
    //
    //
    //     auto opt_str = smp_val["key1"].to_if<std::string>(); // opt_str 是 std::optional<std::string> ，转换失败为空，但不抛出异常
    //     if(opt_str) std::cout << *opt_str << std::endl; // 如果转换成功，输出字符串
    //     std::string or_str = smp_val["key1"].to_or<std::string>("default"); // 如果转换失败，返回 "default"
    //
    //
    //     std::string json_str1 = R"( [ 1, false, null, { "Hello": "World" } ] )";
    //     std::string json_str2 = R"( false )"; // 允许顶层类型是任一 JSON 类型
    //     json::Value val1 = json::read(json_str1).value_or( nullptr ); // 解析 JSON 字符串
    //     std::cout << val1[1].to<bool>() << std::endl; // 输出 0 （没有指定 boolaplha）
    // }
    //
    // {
    //     json::Value v_data2{ MyData2{} };
    //     std::println("");
    //     v_data2.writef( std::cout );
    //     std::println("");
    //     v_data2["data"]["id"] = 8848;
    //     v_data2["data"]["name"] = "Mount Everest";
    //     v_data2["data"]["active"] = true;
    //     v_data2["data_list"].push_back( v_data2["data"] );
    //     v_data2["name"] = "name_name";
    //     MyData2 d_data2{ v_data2 };
    //     M_EXPECT_TRUE( d_data2.my_data.id == 8848 ); // true
    //     M_EXPECT_TRUE( d_data2.my_data.m_name == "Mount Everest" ); // true
    //     M_EXPECT_TRUE( d_data2.data_list.size() == 1 ); // true
    //     M_EXPECT_TRUE( d_data2.data_list[0].id == 8848 ); // true
    //     M_EXPECT_TRUE( d_data2.data_list[0].m_name == "Mount Everest" ); // true
    //     M_EXPECT_TRUE( d_data2.name == "name_name" ); // true
    // }
    //
}
