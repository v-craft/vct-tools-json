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

    M_JSON_CV_FUN( MyData,  // Conversion function, must be in public scope
        M_JSON_CV_MEM( id );    // Note: `,` is required after MyData
        M_JSON_CV_MAP( name, m_name )   // For the remaining fields, no comma `,` is allowed, but semicolon `;` is optional
        M_JSON_CV_MEM( active )
        M_JSON_CV_MAP( value, m_value )
    )
    M_JSON_CS_FUN( MyData,  // Constructor function, must be in public scope
        M_JSON_CS_MEM( id )
        M_JSON_CS_MAP( name, m_name )
        M_JSON_CS_MEM_OR( active, true, nullptr ) // default value is `true`, nullptr means no range value
        M_JSON_CS_MAP_OR( value, m_value, 64.0, nullptr ) // default value is `true`, nullptr means no range value
    )
};

struct MyData2 {
    std::string name;   // std::string is equivalent to json::String, so it can be used directly
    MyData my_data;     // MyData already has conversion and constructor functions, so it can be used directly
    std::vector<MyData> data_list;  // Lists composed of directly usable types can be used directly (but nested lists are not supported)
    MyData2() = default;
    M_JSON_CV_FUN( MyData2,
        M_JSON_CV_MEM( name )
        M_JSON_CV_MAP( data, my_data )
        M_JSON_CV_MEM( data_list )
    )
    M_JSON_CS_FUN( MyData2,
        M_JSON_CS_MEM( name )
        M_JSON_CS_MAP( data, my_data )
        M_JSON_CS_MEM_OR( data_list, std::vector<MyData>{}, MyData{} ) // variable name, default value, default value for inner elements
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
    smp_val.type(); // Returns Type::eObject
    smp_val.is(json::Type::eArray);
    smp_val.type_name();// Returns "Object", no prefix `e`

    json::Value vi_42 = smp_val.get<json::Object>()["key1"]; // Note: mapped value of Object is still Value type
    double i_42 = vi_42.get<json::Number>(); // Get inner value, json::Number is equivalent to double

    auto str_view = smp_val["key2"].to<std::string_view>(); // Returns a view of the internal string
    auto str = smp_val["key2"].move<std::string>(); // Moves out the internal string, now the internal string is empty, previous view is invalid
    int int_42 = smp_val["key1"].to<int>(); // Returns a copy of the internal integer

    auto opt_str = smp_val["key1"].to_if<std::string>(); // opt_str is std::optional<std::string>, conversion fails to empty, but does not throw exception
    if(opt_str) std::cout << *opt_str << std::endl; // If conversion succeeds, output the string

    std::string json_str1 = R"( [ 1, false, null, { "Hello": "World" } ] )";
    std::string json_str2 = R"( false )"; // Top-level type can be any JSON type
    json::Value val1 = json::deserialize(json_str1).value_or( nullptr ); // Parse JSON string
    std::cout << val1[1].to<bool>();; // Output 0 (boolalpha not specified)

    std::string str_ser = val1.serialize(); // Efficient serialization without invalid whitespace, returns std::string
    std::string str_back;
    val1.serialize_to( str_back ); // Write serialization result to the end of std::string
    val1.serialize_to( std::cout ); // Directly output serialization result to `ostream`

    auto pretty_str = val1.serialize_pretty().value_or( "fail" );
    bool res = val1.serialize_pretty_to( std::cout ); // Output to `ostream`, returns whether successful
    if (!res){ std::cerr << "fail"; }

    json::Value val_arr_1 = json::Array{{ 1, 2, 3 }};
    json::Value val_arr_2 = json::Array{{ 1, 2, 3 }};
    json::Value val_arr_3 = json::Array{{ 1, true, 3 }};
    val_arr_1 == val_arr_2; // true
    val_arr_1 == val_arr_3; // false



    {
        json::Value v_null;
        MyData d_null{ v_null }; // No content, so all fields are default values
        d_null.active; // true, because constructor specifies default value as true

        json::Value v_object{ json::Type::eObject };
        v_object["id"] = 42;
        v_object["name"] = "Test User";
        v_object["active"] = false;
        v_object["value"] = 128.0;
        MyData d_object{ v_object };
        d_object.m_name == "Test User"; // true

        json::Value v_data{ d_object }; // Convert MyData to JSON object
        v_data["id"] = 42; // true
    }

    {
        MyData2 data2;
        json::Value v_data2{ data2 };
        v_data2.serialize_pretty_to( std::cout );
    }

    std::println("==============================================");
}
