#include <vct/test_unit_macros.hpp>

import std;
import vct.test.unit;
import vct.tools.json;

using namespace vct::tools;

// --- Construction tests ---
M_TEST(Value, String) {
    // Default construction
    M_ASSERT_NO_THROW(json::Value str_default{ json::Type::eString });
    M_ASSERT_EQ(json::Value{json::Type::eString}.is_num(), true);
    M_ASSERT_EQ(json::Value{json::Type::eString}.is_nul(), false);
    M_ASSERT_EQ(json::Value{json::Type::eString}.is_arr(), false);
    M_ASSERT_EQ(json::Value{json::Type::eString}.is_obj(), false);
    M_ASSERT_EQ(json::Value{json::Type::eString}.is_bol(), false);
    M_ASSERT_EQ(json::Value{json::Type::eString}.is_str(), false);
    M_ASSERT_EQ(json::Value{json::Type::eString}.type(), json::Type::eNumber);

    M_ASSERT_NO_THROW(json::Value str_default{json::String{}});
    M_ASSERT_EQ(json::Value{json::String{}}.to<json::String>(), "");
    M_ASSERT_EQ(json::Value{json::String{}}.type(), json::Type::eString);

    M_ASSERT_EQ(json::Value{json::String{}}.is_num(), false);
    M_ASSERT_EQ(json::Value{json::String{}}.is_nul(), false);
    M_ASSERT_EQ(json::Value{json::String{}}.is_arr(), false);
    M_ASSERT_EQ(json::Value{json::String{}}.is_obj(), false);
    M_ASSERT_EQ(json::Value{json::String{}}.is_bol(), false);
    M_ASSERT_EQ(json::Value{json::String{}}.is_str(), true);

    // Construction with explicit String wrapper
    M_ASSERT_NO_THROW(json::Value str_hello{json::String("hello")});
    M_ASSERT_NO_THROW(json::Value str_empty{json::String("")});
    M_ASSERT_NO_THROW(json::Value str_special{json::String("Hello, 世界!")});

    // Direct string construction
    M_ASSERT_NO_THROW(json::Value direct_str{"direct string"});
    M_ASSERT_NO_THROW(json::Value direct_empty{""});

    // Implicit construction
    json::Value implicit_str = "implicit string";
    json::Value implicit_empty = "";
    M_ASSERT_EQ(implicit_str.type(), json::Type::eString);
    M_ASSERT_EQ(implicit_empty.type(), json::Type::eString);
    M_ASSERT_EQ(implicit_str.to<json::String>(), "implicit string");
    M_ASSERT_EQ(implicit_empty.to<json::String>(), "");

    // Construction from std::string
    std::string std_str = "std::string test";
    json::Value copy_init_std = std_str;
    M_ASSERT_EQ(copy_init_std.type(), json::Type::eString);
    M_ASSERT_EQ(copy_init_std.to<json::String>(), "std::string test");

    // --- Type checking ---
    json::Value str_val{"test string"};
    json::Value empty_val{""};
    M_ASSERT_TRUE(str_val.is_str());
    M_ASSERT_FALSE(str_val.is_num());
    M_EXPECT_STREQ(str_val.type_name(), "String");

    // --- Reference access ---
    M_ASSERT_EQ(str_val.str(), "test string");
    M_ASSERT_EQ(empty_val.str(), "");
    const json::Value const_str{"const string"};
    M_ASSERT_EQ(const_str.str(), "const string");
    M_ASSERT_THROW(std::ignore = json::Value{42}.str(), std::bad_variant_access);

    // --- Assignment tests ---
    json::Value assign_val{json::String{}};
    assign_val = json::String("assigned");
    M_ASSERT_EQ(assign_val.to<json::String>(), "assigned");
    assign_val = "reassigned";
    M_ASSERT_EQ(assign_val.to<json::String>(), "reassigned");

    // Reference modification
    json::Value mod_val{"original"};
    auto& str_ref = mod_val.str();
    str_ref = "modified";
    M_ASSERT_EQ(mod_val.to<json::String>(), "modified");
    str_ref += " appended";
    M_ASSERT_EQ(mod_val.to<json::String>(), "modified appended");

    // --- Comparison tests ---
    json::Value val_str1{"same string"};
    json::Value val_str2{"same string"};
    json::Value val_str3{"different string"};
    M_ASSERT_TRUE(val_str1 == val_str2);
    M_ASSERT_FALSE(val_str1 == val_str3);
    M_ASSERT_TRUE(val_str1 == "same string");
    M_ASSERT_FALSE(val_str1 == "");

    // Mixed type comparison
    json::Value num_val{42.0};
    json::Value bool_val{true};
    json::Value null_val{nullptr};
    M_ASSERT_FALSE(val_str1 == num_val);
    M_ASSERT_FALSE(val_str1 == bool_val);
    M_ASSERT_FALSE(val_str1 == null_val);

    // --- Serialization tests ---
    M_ASSERT_EQ(json::Value{"hello"}.dump(), "\"hello\"");
    M_ASSERT_EQ(json::Value{""}.dump(), "\"\"");
    M_ASSERT_EQ(json::Value{"simple"}.dump(), "\"simple\"");

    // Pretty serialization
    M_ASSERT_EQ(json::Value{"pretty"}.dumpf(), "\"pretty\"");
    M_ASSERT_EQ(json::Value{""}.dumpf(), "\"\"");

    // Escape sequences serialization
    M_ASSERT_EQ(json::Value{"line1\nline2"}.dump(), "\"line1\\nline2\"");
    M_ASSERT_EQ(json::Value{"col1\tcol2"}.dump(), "\"col1\\tcol2\"");
    M_ASSERT_EQ(json::Value{"say \"hello\""}.dump(), "\"say \\\"hello\\\"\"");
    M_ASSERT_EQ(json::Value{"path\\to\\file"}.dump(), "\"path\\\\to\\\\file\"");

    // --- Parsing tests ---
    auto parsed_hello = json::read("\"hello world\"");
    if (parsed_hello.has_value()) {
        M_ASSERT_EQ(parsed_hello->type(), json::Type::eString);
        M_ASSERT_EQ(parsed_hello->str(), "hello world");
    }

    auto parsed_empty = json::read("\"\"");
    if (parsed_empty.has_value()) {
        M_ASSERT_EQ(parsed_empty->type(), json::Type::eString);
        M_ASSERT_EQ(parsed_empty->str(), "");
    }

    // Escape sequence parsing
    auto parsed_newline = json::read(R"("line1\nline2")");
    if (parsed_newline.has_value()) {
        M_ASSERT_EQ(parsed_newline->str(), "line1\nline2");
    }

    auto parsed_tab = json::read(R"("col1\tcol2")");
    if (parsed_tab.has_value()) {
        M_ASSERT_EQ(parsed_tab->str(), "col1\tcol2");
    }

    auto parsed_quote = json::read(R"("say \"hello\"")");
    if (parsed_quote.has_value()) {
        M_ASSERT_EQ(parsed_quote->str(), "say \"hello\"");
    }

    auto parsed_backslash = json::read(R"("path\\to\\file")");
    if (parsed_backslash.has_value()) {
        M_ASSERT_EQ(parsed_backslash->str(), "path\\to\\file");
    }

    // --- Unicode escape sequence parsing ---
    auto parsed_unicode_a = json::read(R"("\u0041")");
    if (parsed_unicode_a.has_value()) {
        M_ASSERT_EQ(parsed_unicode_a->str(), "A");
    }

    auto parsed_unicode_chinese = json::read(R"("\u4e2d\u6587")");
    if (parsed_unicode_chinese.has_value()) {
        M_ASSERT_EQ(parsed_unicode_chinese->str(), "中文");
    }

    auto parsed_unicode_mixed = json::read(R"("Hello \u4e16\u754c!")");
    if (parsed_unicode_mixed.has_value()) {
        M_ASSERT_EQ(parsed_unicode_mixed->str(), "Hello 世界!");
    }

    // Unicode round-trip: parse then serialize
    auto unicode_roundtrip = json::read(R"("\u4e16\u754c")");
    if (unicode_roundtrip.has_value()) {
        auto roundtrip_serialized = unicode_roundtrip->dump();
        M_ASSERT_EQ(roundtrip_serialized, "\"世界\"");
    }

    // Control characters serialization
    json::Value control_chars{"\x01\x02\x03\x1f"};
    auto control_serialized = control_chars.dump();
    M_ASSERT_TRUE(control_serialized.find("\\u") != std::string::npos);

    // --- Type safety ---
    M_ASSERT_THROW(std::ignore = str_val.to<json::Number>(), std::runtime_error);
    M_ASSERT_THROW(std::ignore = str_val.to<json::Bool>(), std::runtime_error);
    M_ASSERT_THROW(std::ignore = str_val.to<json::Array>(), std::runtime_error);
    M_ASSERT_THROW(std::ignore = str_val.to<json::Object>(), std::runtime_error);
    M_ASSERT_THROW(std::ignore = str_val.to<std::nullptr_t>(), std::runtime_error);

    // --- String conversion ---
    json::Value str_conv_test{"conversion test"};
    M_ASSERT_EQ(str_conv_test.to<std::string>(), "conversion test");
    json::Value str_view_test{"string_view test"};
    auto view = str_view_test.to<std::string_view>();
    M_ASSERT_EQ(view, "string_view test");

    // --- String properties ---
    json::Value length_test{"12345"};
    M_ASSERT_EQ(length_test.to<json::String>().length(), 5);
    M_ASSERT_FALSE(length_test.to<json::String>().empty());

    json::Value empty_length_test{""};
    M_ASSERT_EQ(empty_length_test.to<json::String>().length(), 0);
    M_ASSERT_TRUE(empty_length_test.to<json::String>().empty());

    // --- Consistency checks ---
    json::Value consistency_str1{"same content"};
    json::Value consistency_str2{"same content"};
    M_ASSERT_TRUE(consistency_str1 == consistency_str2);
    M_ASSERT_EQ(consistency_str1.dump(), "\"same content\"");
}
