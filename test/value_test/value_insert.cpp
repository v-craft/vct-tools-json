#include <vct/test_unit_macros.hpp>

import std;
import vct.test.unit;
import vct.tools.json;


using namespace vct::tools;

M_TEST(Value, Insert) {
    using json::Value;
    using json::String;
    // 1. size() & empty() for array
    Value arr = json::Array{};
    M_ASSERT_EQ(arr.size(), 0);
    M_ASSERT_TRUE(arr.empty());
    arr.push_back(1);
    arr.push_back(2);
    arr.push_back(3);
    M_ASSERT_EQ(arr.size(), 3);
    M_ASSERT_FALSE(arr.empty());
    // 2. size() & empty() for object
    Value obj = json::Object{};
    M_ASSERT_EQ(obj.size(), 0);
    M_ASSERT_TRUE(obj.empty());
    obj.insert("a", 1);
    obj.insert("b", 2);
    M_ASSERT_EQ(obj.size(), 2);
    M_ASSERT_FALSE(obj.empty());
    // 3. size() & empty() for non-container
    Value str = String{"abc"};
    M_ASSERT_EQ(str.size(), 0);
    M_ASSERT_FALSE(str.empty());
    // 4. push_back(const Value&) & push_back(Value&&)
    Value arr2 = json::Array{};
    Value v1 = 10;
    M_ASSERT_TRUE(arr2.push_back(v1));
    M_ASSERT_EQ(arr2.size(), 1);
    M_ASSERT_TRUE(arr2.push_back(Value(20)));
    M_ASSERT_EQ(arr2.size(), 2);
    // 5. pop_back()
    M_ASSERT_TRUE(arr2.pop_back());
    M_ASSERT_EQ(arr2.size(), 1);
    M_ASSERT_TRUE(arr2.pop_back());
    M_ASSERT_EQ(arr2.size(), 0);
    M_ASSERT_FALSE(arr2.pop_back());
    // 6. insert(index, const Value&) & insert(index, Value&&)
    arr2.push_back(1);
    arr2.push_back(3);
    M_ASSERT_TRUE(arr2.insert(1, Value(2)));
    M_ASSERT_EQ(arr2.size(), 3);
    M_ASSERT_EQ(arr2[1].to<json::Number>(), 2);
    Value v4 = 4;
    M_ASSERT_TRUE(arr2.insert(3, v4));
    M_ASSERT_EQ(arr2[3].to<json::Number>(), 4);
    M_ASSERT_FALSE(arr2.insert(10, Value(99)));
    // 7. insert(index, count, const Value&)
    M_ASSERT_TRUE(arr2.insert(2, 2, Value(99)));
    M_ASSERT_EQ(arr2.size(), 6);
    M_ASSERT_EQ(arr2[2].to<json::Number>(), 99);
    M_ASSERT_EQ(arr2[3].to<json::Number>(), 99);
    // 8. erase(index)
    M_ASSERT_TRUE(arr2.erase(2));
    M_ASSERT_EQ(arr2.size(), 5);
    M_ASSERT_FALSE(arr2.erase(10));
    // 9. insert(key, const Value&) & insert(key, Value&&)
    Value obj2 = json::Object{};
    M_ASSERT_TRUE(obj2.insert("x", 100));
    Value v200 = 200;
    M_ASSERT_TRUE(obj2.insert("y", v200));
    M_ASSERT_EQ(obj2["x"].to<json::Number>(), 100);
    M_ASSERT_EQ(obj2["y"].to<json::Number>(), 200);
    // 10. erase(key)
    M_ASSERT_TRUE(obj2.erase("x"));
    M_ASSERT_FALSE(obj2.erase("not_exist"));
    // 11. contains(key)
    M_ASSERT_TRUE(obj2.contains("y"));
    M_ASSERT_FALSE(obj2.contains("x"));
    // 12. insert/erase/contains on wrong type
    Value n = nullptr;
    M_ASSERT_FALSE(n.insert("a", 1));
    M_ASSERT_FALSE(n.erase("a"));
    M_ASSERT_FALSE(n.contains("a"));
    M_ASSERT_FALSE(n.push_back(1));
    M_ASSERT_FALSE(n.pop_back());
    M_ASSERT_FALSE(n.insert(0, 1));
    M_ASSERT_FALSE(n.insert(0, 1, 1));
    M_ASSERT_FALSE(n.erase(0));
}
