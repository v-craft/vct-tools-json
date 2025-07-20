#include <vct/test_unit_macros.hpp>

import std;
import vct.test.unit;
import vct.tools.json;

using namespace vct::tools;

M_TEST(ARR_MAP, Simple) {
    const std::vector arr_int = { 1, 2, 3, 4, 5 };
    const json::Value v_arr_int{ arr_int };
    const json::Value v_arr_int_res = json::Array{ {1, 2, 3, 4, 5} };
    M_EXPECT_EQ( v_arr_int, v_arr_int_res );

}

M_TEST(ARR_MAP, ArrayTypes) {
    // std::array<int, N>
    const std::array<int, 4> arr = {10, 20, 30, 40};
    json::Value v_arr{ arr };
    M_EXPECT_EQ( v_arr, (json::Array{{10, 20, 30, 40}}) );

    // std::list<double>
    const std::list<double> lst = {1.1, 2.2, 3.3};
    json::Value v_lst{lst};
    M_EXPECT_EQ(v_lst, (json::Array{{1.1, 2.2, 3.3}}) );

    // std::deque<bool>
    const std::deque<bool> deq = {true, false, true};
    json::Value v_deq{deq};
    M_EXPECT_EQ(v_deq, (json::Array{{true, false, true}}));

    // std::vector<std::string>
    const std::vector<std::string> vec_str = {"a", "b", "c"};
    json::Value v_vec_str{vec_str};
    M_EXPECT_EQ(v_vec_str, (json::Array{{"a", "b", "c"}}) );

    // std::vector<json::Value>
    const std::vector<json::Value> vec_val = {json::Number(1), json::Bool(false), json::String("x")};
    json::Value v_vec_val{vec_val};
    M_EXPECT_EQ(v_vec_val, (json::Array{{1, false, "x"}}) );
}

M_TEST(ARR_MAP, MapTypes) {
    // std::map<std::string, int>
    const std::map<std::string, int> map_int = {{"a", 1}, {"b", 2}};
    json::Value v_map_int{map_int};
    M_EXPECT_EQ(v_map_int["a"], 1);
    M_EXPECT_EQ(v_map_int["b"], 2);

    // std::map<std::string, bool>
    const std::map<std::string, bool> map_bool = {{"x", true}, {"y", false}};
    json::Value v_map_bool{map_bool};
    M_EXPECT_EQ(v_map_bool["x"], true);
    M_EXPECT_EQ(v_map_bool["y"], false);

    // std::map<std::string, std::string>
    const std::map<std::string, std::string> map_str = {{"foo", "bar"}, {"baz", "qux"}};
    json::Value v_map_str{map_str};
    M_EXPECT_EQ(v_map_str["foo"], "bar");
    M_EXPECT_EQ(v_map_str["baz"], "qux");

    // std::map<std::string, json::Value>
    const std::map<std::string, json::Value> map_val = {{"n", json::Number(42)}, {"b", json::Bool(true)}, {"s", json::String("str")}};
    json::Value v_map_val{map_val};
    M_EXPECT_EQ(v_map_val["n"], 42);
    M_EXPECT_EQ(v_map_val["b"], true);
    M_EXPECT_EQ(v_map_val["s"], "str");
}

M_TEST(ARR_MAP, UnorderedMapTypes) {
    // std::unordered_map<std::string, double>
    const std::unordered_map<std::string, double> umap_num = {{"pi", 3.14}, {"e", 2.718}};
    json::Value v_umap_num{umap_num};
    M_EXPECT_EQ(v_umap_num["pi"], 3.14);
    M_EXPECT_EQ(v_umap_num["e"], 2.718);

    // std::unordered_map<std::string, bool>
    const std::unordered_map<std::string, bool> umap_bool = {{"t", true}, {"f", false}};
    json::Value v_umap_bool{umap_bool};
    M_EXPECT_EQ(v_umap_bool["t"], true);
    M_EXPECT_EQ(v_umap_bool["f"], false);

    // std::unordered_map<std::string, std::string>
    const std::unordered_map<std::string, std::string> umap_str = {{"hello", "world"}, {"key", "value"}};
    json::Value v_umap_str{umap_str};
    M_EXPECT_EQ(v_umap_str["hello"], "world");
    M_EXPECT_EQ(v_umap_str["key"], "value");

    // std::unordered_map<std::string, json::Value>
    const std::unordered_map<std::string, json::Value> umap_val = {{"num", json::Number(7)}, {"flag", json::Bool(false)}, {"txt", json::String("abc")}};
    json::Value v_umap_val{umap_val};
    M_EXPECT_EQ(v_umap_val["num"], 7);
    M_EXPECT_EQ(v_umap_val["flag"], false);
    M_EXPECT_EQ(v_umap_val["txt"], "abc");
}

M_TEST(ARR_MAP, NestedContainers) {
    // Nested array
    const std::vector<std::vector<int>> nested_vec = {{1,2}, {3,4}};
    json::Value v_nested_vec{nested_vec};
    M_EXPECT_EQ(v_nested_vec[0], (json::Array{{1,2}}));
    M_EXPECT_EQ(v_nested_vec[1], (json::Array{{3,4}}));

    // Nested map
    const std::map<std::string, std::map<std::string, int>> nested_map = {{"outer", {{"inner", 99}}}};
    json::Value v_nested_map{nested_map};
    M_EXPECT_EQ(v_nested_map["outer"]["inner"], 99);

    // Array of maps
    const std::vector<std::map<std::string, bool>> arr_of_map = { {{"a", true}}, {{"b", false}} };
    json::Value v_arr_of_map{arr_of_map};
    M_EXPECT_EQ(v_arr_of_map[0]["a"], true);
    M_EXPECT_EQ(v_arr_of_map[1]["b"], false);

    // Map of arrays
    const std::map<std::string, std::vector<std::string>> map_of_arr = {{"letters", {"a", "b"}}, {"digits", {"1", "2"}}};
    json::Value v_map_of_arr{map_of_arr};
    M_EXPECT_EQ(v_map_of_arr["letters"], (json::Array{{"a", "b"}}));
    M_EXPECT_EQ(v_map_of_arr["digits"], (json::Array{{"1", "2"}}));
}
