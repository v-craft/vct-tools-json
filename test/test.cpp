// //
// // Created by mys_vac on 25-4-19.
// import std;
// import Json;
int main(){}
// #define TEST(module_name, test_name) \
//     void test_##module_name_##test_name()
//
// TEST(JsonTest, Basic) {
//     ASSERT_NO_THROW(Json::Value());
// }

// TEST(GoogleTest, start) {
//     std::cout << "Google Test Start" << std::endl;
// }
//
// // 基础类型测试
// TEST(JsonBase, type_test) {
//     ASSERT_NO_THROW(Json::Value());
//     // ASSERT_NO_THROW(Json::Value(Json::Type::object));
//     // ASSERT_NO_THROW(Json::Value(Json::Type::array));
//     // ASSERT_NO_THROW(Json::Value(Json::Type::string));
//     // ASSERT_NO_THROW(Json::Value(Json::Type::number));
//     // ASSERT_NO_THROW(Json::Value(Json::Type::boolean));
//     // ASSERT_NO_THROW(Json::Value(Json::Type::null));
//     //
//     // EXPECT_EQ(Json::Value().type(), Json::Type::null);
//     // EXPECT_EQ(Json::Value(Json::Type::object).type(), Json::Type::object);
//     // EXPECT_EQ(Json::Value(Json::Type::array).type(), Json::Type::array);
//     // EXPECT_EQ(Json::Value(Json::Type::string).type(), Json::Type::string);
//     // EXPECT_EQ(Json::Value(Json::Type::number).type(), Json::Type::number);
//     // EXPECT_EQ(Json::Value(Json::Type::boolean).type(), Json::Type::boolean);
//     // EXPECT_EQ(Json::Value(Json::Type::null).type(), Json::Type::null);
//     // EXPECT_EQ(Json::Value{}.type(), Json::Type::null);
//     // EXPECT_EQ(Json::Value{Json::Type::object}.type(), Json::Type::object);
//     // EXPECT_EQ(Json::Value{Json::Type::array}.type(), Json::Type::array);
//     // EXPECT_EQ(Json::Value{Json::Type::string}.type(), Json::Type::string);
//     // EXPECT_EQ(Json::Value{Json::Type::number}.type(), Json::Type::number);
//     // EXPECT_EQ(Json::Value{Json::Type::boolean}.type(), Json::Type::boolean);
//     // EXPECT_EQ(Json::Value{Json::Type::null}.type(), Json::Type::null);
//     //
//     // EXPECT_EQ(Json::Value().is_object(), false);
//     // EXPECT_EQ(Json::Value().is_array(), false);
//     // EXPECT_EQ(Json::Value().is_string(), false);
//     // EXPECT_EQ(Json::Value().is_number(), false);
//     // EXPECT_EQ(Json::Value().is_bool(), false);
//     // EXPECT_EQ(Json::Value().is_int64(), false);
//     // EXPECT_EQ(Json::Value().is_double(), false);
//     //
//     // EXPECT_EQ(Json::Value().is_null(), true);
//     // EXPECT_EQ(Json::Value(Json::Type::object).is_object(), true);
//     // EXPECT_EQ(Json::Value(Json::Type::array).is_array(), true);
//     // EXPECT_EQ(Json::Value(Json::Type::string).is_string(), true);
//     // EXPECT_EQ(Json::Value(Json::Type::number).is_number(), true);
//     // EXPECT_EQ(Json::Value(Json::Type::boolean).is_bool(), true);
//     // EXPECT_EQ(Json::Value(Json::Type::null).is_null(), true);
//     //
//     // EXPECT_EQ(Json::Value(Json::Type::object).is_array(), false);
//     // EXPECT_EQ(Json::Value(Json::Type::object).is_string(), false);
//     // EXPECT_EQ(Json::Value(Json::Type::object).is_number(), false);
//     // EXPECT_EQ(Json::Value(Json::Type::object).is_bool(), false);
//     // EXPECT_EQ(Json::Value(Json::Type::object).is_null(), false);
//     // EXPECT_EQ(Json::Value(Json::Type::object).is_int64(), true);
//     // EXPECT_EQ(Json::Value(Json::Type::object).is_double(), false);
//     //
//     // EXPECT_EQ(Json::Value(Json::Type::array).is_object(), false);
//     // EXPECT_EQ(Json::Value(Json::Type::array).is_string(), false);
//     // EXPECT_EQ(Json::Value(Json::Type::array).is_number(), false);
//     // EXPECT_EQ(Json::Value(Json::Type::array).is_bool(), false);
//     // EXPECT_EQ(Json::Value(Json::Type::array).is_null(), false);
//     // EXPECT_EQ(Json::Value(Json::Type::array).is_int64(), false);
//     // EXPECT_EQ(Json::Value(Json::Type::array).is_double(), false);
//     //
//     // EXPECT_EQ(Json::Value(Json::Type::string).is_object(), false);
//     // EXPECT_EQ(Json::Value(Json::Type::string).is_array(), false);
//     // EXPECT_EQ(Json::Value(Json::Type::string).is_number(), false);
//     // EXPECT_EQ(Json::Value(Json::Type::string).is_bool(), false);
//     // EXPECT_EQ(Json::Value(Json::Type::string).is_null(), false);
//     // EXPECT_EQ(Json::Value(Json::Type::string).is_int64(), false);
//     // EXPECT_EQ(Json::Value(Json::Type::string).is_double(), false);
//     //
//     // EXPECT_EQ(Json::Value(Json::Type::number).is_object(), false);
//     // EXPECT_EQ(Json::Value(Json::Type::number).is_array(), false);
//     // EXPECT_EQ(Json::Value(Json::Type::number).is_string(), false);
//     // EXPECT_EQ(Json::Value(Json::Type::number).is_bool(), false);
//     // EXPECT_EQ(Json::Value(Json::Type::number).is_null(), false);
//     // EXPECT_EQ(Json::Value(Json::Type::number).is_int64(), true);
//     // EXPECT_EQ(Json::Value(Json::Type::number).is_double(), false);
//     //
//     // EXPECT_EQ(Json::Value(Json::Type::boolean).is_object(), false);
//     // EXPECT_EQ(Json::Value(Json::Type::boolean).is_array(), false);
//     // EXPECT_EQ(Json::Value(Json::Type::boolean).is_string(), false);
//     // EXPECT_EQ(Json::Value(Json::Type::boolean).is_number(), false);
//     // EXPECT_EQ(Json::Value(Json::Type::boolean).is_null(), false);
//     // EXPECT_EQ(Json::Value(Json::Type::boolean).is_int64(), false);
//     // EXPECT_EQ(Json::Value(Json::Type::boolean).is_double(), false);
//     //
//     // EXPECT_EQ(Json::Value(Json::Type::null).is_object(), false);
//     // EXPECT_EQ(Json::Value(Json::Type::null).is_array(), false);
//     // EXPECT_EQ(Json::Value(Json::Type::null).is_string(), false);
//     // EXPECT_EQ(Json::Value(Json::Type::null).is_number(), false);
//     // EXPECT_EQ(Json::Value(Json::Type::null).is_bool(), false);
//     // EXPECT_EQ(Json::Value(Json::Type::null).is_int64(), false);
//     // EXPECT_EQ(Json::Value(Json::Type::null).is_double(), false);
// }

