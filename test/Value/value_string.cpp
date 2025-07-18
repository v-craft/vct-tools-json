#include <vct/test_unit_macros.hpp>

import std;
import vct.test.unit;
import vct.tools.json;

using namespace vct::tools;

M_TEST(Value, String) {
    // Test default string construction
    M_ASSERT_NO_THROW( json::Value str_default{json::Type::eString} );
    M_ASSERT_EQ( json::Value{json::Type::eString}.get<json::String>(), "" );
    M_ASSERT_EQ( json::Value{json::Type::eString}.type(), json::Type::eString );
    
    // Test construction with explicit String wrapper
    M_ASSERT_NO_THROW( json::Value str_hello{json::String("hello")} );
    M_ASSERT_NO_THROW( json::Value str_empty{json::String("")} );
    M_ASSERT_NO_THROW( json::Value str_special{json::String("Hello, 世界!")} );
    
    // Test direct string construction (without json::String wrapper)
    M_ASSERT_NO_THROW( json::Value direct_str{"direct string"} );
    M_ASSERT_NO_THROW( json::Value direct_empty{""} );
    
    // Test implicit construction (copy initialization)
    json::Value implicit_str = "implicit string";
    json::Value implicit_empty = "";
    M_ASSERT_EQ( implicit_str.type(), json::Type::eString );
    M_ASSERT_EQ( implicit_empty.type(), json::Type::eString );
    M_ASSERT_EQ( implicit_str.get<json::String>(), "implicit string" );
    M_ASSERT_EQ( implicit_empty.get<json::String>(), "" );
    
    // Test copy initialization with different forms
    json::Value copy_init_str = "copy init string";
    json::Value copy_init_wrapper = json::String("wrapper string");
    std::string std_str = "std::string test";
    json::Value copy_init_std = std_str;
    
    M_ASSERT_EQ( copy_init_str.type(), json::Type::eString );
    M_ASSERT_EQ( copy_init_wrapper.type(), json::Type::eString );
    M_ASSERT_EQ( copy_init_std.type(), json::Type::eString );
    
    M_ASSERT_EQ( copy_init_str.get<json::String>(), "copy init string" );
    M_ASSERT_EQ( copy_init_wrapper.get<json::String>(), "wrapper string" );
    M_ASSERT_EQ( copy_init_std.get<json::String>(), "std::string test" );
    
    // Test type checking
    json::Value str_val{"test string"};
    json::Value empty_val{""};
    
    M_ASSERT_EQ( str_val.type(), json::Type::eString );
    M_ASSERT_EQ( empty_val.type(), json::Type::eString );
    
    M_ASSERT_TRUE( str_val.is(json::Type::eString) );
    M_ASSERT_TRUE( empty_val.is(json::Type::eString) );
    
    M_ASSERT_FALSE( str_val.is(json::Type::eNumber) );
    M_ASSERT_FALSE( str_val.is(json::Type::eBool) );
    M_ASSERT_FALSE( str_val.is(json::Type::eNull) );
    M_ASSERT_FALSE( str_val.is(json::Type::eArray) );
    M_ASSERT_FALSE( str_val.is(json::Type::eObject) );
    
    M_EXPECT_STREQ( str_val.type_name(), "String" );
    M_EXPECT_STREQ( empty_val.type_name(), "String" );
    
    // Test get<T>() method
    M_ASSERT_EQ( str_val.get<json::String>(), "test string" );
    M_ASSERT_EQ( empty_val.get<json::String>(), "" );
    
    // Test get_ref<T>() method
    M_ASSERT_NO_THROW( auto& ref = str_val.get_ref<json::String>() );
    M_ASSERT_EQ( str_val.get_ref<json::String>(), "test string" );
    M_ASSERT_EQ( empty_val.get_ref<json::String>(), "" );
    
    // Test get_ref with different string values
    json::Value ref_test_str{"reference test"};
    json::Value ref_test_empty{""};
    json::Value ref_test_unicode{"测试 Unicode"};
    
    M_ASSERT_NO_THROW( auto& ref_str = ref_test_str.get_ref<json::String>() );
    M_ASSERT_NO_THROW( auto& ref_empty = ref_test_empty.get_ref<json::String>() );
    M_ASSERT_NO_THROW( auto& ref_unicode = ref_test_unicode.get_ref<json::String>() );
    
    M_ASSERT_EQ( ref_test_str.get_ref<json::String>(), "reference test" );
    M_ASSERT_EQ( ref_test_empty.get_ref<json::String>(), "" );
    M_ASSERT_EQ( ref_test_unicode.get_ref<json::String>(), "测试 Unicode" );
    
    // Test const get_ref
    const json::Value const_str{"const string"};
    M_ASSERT_NO_THROW( auto& const_ref = const_str.get_ref<json::String>() );
    M_ASSERT_EQ( const_str.get_ref<json::String>(), "const string" );
    
    // Test get_ref type safety - wrong type should throw
    json::Value wrong_type_val{42};
    M_ASSERT_THROW( std::ignore = wrong_type_val.get_ref<json::String>(), std::runtime_error );
    
    json::Value bool_val_for_ref{true};
    M_ASSERT_THROW( std::ignore = bool_val_for_ref.get_ref<json::String>(), std::runtime_error );
    
    json::Value null_val_for_ref{nullptr};
    M_ASSERT_THROW( std::ignore = null_val_for_ref.get_ref<json::String>(), std::runtime_error );
    
    // Test assignment operations
    json::Value assign_val{json::Type::eString};
    M_ASSERT_NO_THROW( assign_val = json::String("assigned") );
    M_ASSERT_EQ( assign_val.get<json::String>(), "assigned" );
    M_ASSERT_EQ( assign_val.type(), json::Type::eString );
    
    M_ASSERT_NO_THROW( assign_val = json::String("reassigned") );
    M_ASSERT_EQ( assign_val.get<json::String>(), "reassigned" );
    
    // Test direct assignment (without json::String wrapper)
    json::Value direct_assign{};
    M_ASSERT_NO_THROW( direct_assign = "direct assignment" );
    M_ASSERT_EQ( direct_assign.type(), json::Type::eString );
    M_ASSERT_EQ( direct_assign.get<json::String>(), "direct assignment" );
    
    M_ASSERT_NO_THROW( direct_assign = "" );
    M_ASSERT_EQ( direct_assign.get<json::String>(), "" );
    
    // Test implicit assignment (copy assignment)
    json::Value implicit_assign{};
    implicit_assign = "implicit assignment";
    M_ASSERT_EQ( implicit_assign.type(), json::Type::eString );
    M_ASSERT_EQ( implicit_assign.get<json::String>(), "implicit assignment" );
    
    std::string std_assign = "std::string assignment";
    implicit_assign = std_assign;
    M_ASSERT_EQ( implicit_assign.get<json::String>(), "std::string assignment" );
    
    // Test reference modification
    json::Value mod_val{"original"};
    auto& str_ref = mod_val.get_ref<json::String>();
    str_ref = "modified";
    M_ASSERT_EQ( mod_val.get<json::String>(), "modified" );
    
    str_ref += " appended";
    M_ASSERT_EQ( mod_val.get<json::String>(), "modified appended" );
    
    // Test Value-to-Value comparison
    json::Value val_str1{"same string"};
    json::Value val_str2{"same string"};
    json::Value val_str3{"different string"};
    json::Value val_num{42};
    
    M_ASSERT_TRUE( val_str1 == val_str2 );
    M_ASSERT_FALSE( val_str1 == val_str3 );
    M_ASSERT_FALSE( val_str1 == val_num ); // Different types
    
    // Test Value-to-string comparison
    json::Value str_compare{"comparison test"};
    M_ASSERT_TRUE( str_compare == "comparison test" );
    M_ASSERT_TRUE( str_compare == json::String("comparison test") );
    M_ASSERT_FALSE( str_compare == "different" );
    M_ASSERT_FALSE( str_compare == "" );
    
    // Test mixed type comparison
    json::Value num_val{42.0};
    json::Value bool_val{true};
    json::Value null_val{nullptr};
    
    M_ASSERT_FALSE( str_compare == num_val );
    M_ASSERT_FALSE( str_compare == bool_val );
    M_ASSERT_FALSE( str_compare == null_val );
    
    // Test basic serialization
    M_ASSERT_NO_THROW( str_val.serialize() );
    M_ASSERT_NO_THROW( empty_val.serialize() );
    
    // Test specific serialization results
    M_ASSERT_EQ( json::Value{"hello"}.serialize(), "\"hello\"" );
    M_ASSERT_EQ( json::Value{""}.serialize(), "\"\"" );
    M_ASSERT_EQ( json::Value{"simple"}.serialize(), "\"simple\"" );
    
    // Test pretty serialization
    M_ASSERT_NO_THROW( str_val.serialize_pretty() );
    M_ASSERT_EQ( json::Value{"pretty"}.serialize_pretty(), "\"pretty\"" );
    M_ASSERT_EQ( json::Value{""}.serialize_pretty(), "\"\"" );
    
    // Test escape sequences serialization
    json::Value escape_test{"line1\nline2"};
    M_ASSERT_EQ( escape_test.serialize(), "\"line1\\nline2\"" );
    
    json::Value tab_test{"col1\tcol2"};
    M_ASSERT_EQ( tab_test.serialize(), "\"col1\\tcol2\"" );
    
    json::Value quote_test{"say \"hello\""};
    M_ASSERT_EQ( quote_test.serialize(), "\"say \\\"hello\\\"\"" );
    
    json::Value backslash_test{"path\\to\\file"};
    M_ASSERT_EQ( backslash_test.serialize(), "\"path\\\\to\\\\file\"" );
    
    json::Value carriage_test{"line1\rline2"};
    M_ASSERT_EQ( carriage_test.serialize(), "\"line1\\rline2\"" );
    
    json::Value formfeed_test{"page1\fpage2"};
    M_ASSERT_EQ( formfeed_test.serialize(), "\"page1\\fpage2\"" );
    
    json::Value backspace_test{"text\bmore"};
    M_ASSERT_EQ( backspace_test.serialize(), "\"text\\bmore\"" );
    
    // Test basic parsing from string
    M_ASSERT_NO_THROW( json::parse("\"hello\"") );
    M_ASSERT_NO_THROW( json::parse("\"\"") );
    M_ASSERT_NO_THROW( json::parse("\"simple string\"") );
    
    // Test parsing results
    auto parsed_hello = json::parse("\"hello world\"");
    if (parsed_hello.has_value()) {
        M_ASSERT_EQ( parsed_hello->type(), json::Type::eString );
        M_ASSERT_EQ( parsed_hello->get<json::String>(), "hello world" );
    } else {
        M_ASSERT_FAIL("Failed to parse simple string");
    }
    
    auto parsed_empty = json::parse("\"\"");
    if (parsed_empty.has_value()) {
        M_ASSERT_EQ( parsed_empty->type(), json::Type::eString );
        M_ASSERT_EQ( parsed_empty->get<json::String>(), "" );
    } else {
        M_ASSERT_FAIL("Failed to parse empty string");
    }
    
    // Test escape sequence parsing (unescaping)
    auto parsed_newline = json::parse("\"line1\\nline2\"");
    if (parsed_newline.has_value()) {
        M_ASSERT_EQ( parsed_newline->type(), json::Type::eString );
        M_ASSERT_EQ( parsed_newline->get<json::String>(), "line1\nline2" );
    } else {
        M_ASSERT_FAIL("Failed to parse newline escape sequence");
    }
    
    auto parsed_tab = json::parse("\"col1\\tcol2\"");
    if (parsed_tab.has_value()) {
        M_ASSERT_EQ( parsed_tab->type(), json::Type::eString );
        M_ASSERT_EQ( parsed_tab->get<json::String>(), "col1\tcol2" );
    } else {
        M_ASSERT_FAIL("Failed to parse tab escape sequence");
    }
    
    auto parsed_quote = json::parse("\"say \\\"hello\\\"\"");
    if (parsed_quote.has_value()) {
        M_ASSERT_EQ( parsed_quote->type(), json::Type::eString );
        M_ASSERT_EQ( parsed_quote->get<json::String>(), "say \"hello\"" );
    } else {
        M_ASSERT_FAIL("Failed to parse quote escape sequence");
    }
    
    auto parsed_backslash = json::parse("\"path\\\\to\\\\file\"");
    if (parsed_backslash.has_value()) {
        M_ASSERT_EQ( parsed_backslash->type(), json::Type::eString );
        M_ASSERT_EQ( parsed_backslash->get<json::String>(), "path\\to\\file" );
    } else {
        M_ASSERT_FAIL("Failed to parse backslash escape sequence");
    }
    
    auto parsed_carriage = json::parse("\"line1\\rline2\"");
    if (parsed_carriage.has_value()) {
        M_ASSERT_EQ( parsed_carriage->type(), json::Type::eString );
        M_ASSERT_EQ( parsed_carriage->get<json::String>(), "line1\rline2" );
    } else {
        M_ASSERT_FAIL("Failed to parse carriage return escape sequence");
    }
    
    auto parsed_formfeed = json::parse("\"page1\\fpage2\"");
    if (parsed_formfeed.has_value()) {
        M_ASSERT_EQ( parsed_formfeed->type(), json::Type::eString );
        M_ASSERT_EQ( parsed_formfeed->get<json::String>(), "page1\fpage2" );
    } else {
        M_ASSERT_FAIL("Failed to parse form feed escape sequence");
    }
    
    auto parsed_backspace = json::parse("\"text\\bmore\"");
    if (parsed_backspace.has_value()) {
        M_ASSERT_EQ( parsed_backspace->type(), json::Type::eString );
        M_ASSERT_EQ( parsed_backspace->get<json::String>(), "text\bmore" );
    } else {
        M_ASSERT_FAIL("Failed to parse backspace escape sequence");
    }
    
    // Test Unicode escape sequences (\uXXXX)
    auto parsed_unicode_a = json::parse("\"\\u0041\""); // 'A'
    if (parsed_unicode_a.has_value()) {
        M_ASSERT_EQ( parsed_unicode_a->type(), json::Type::eString );
        M_ASSERT_EQ( parsed_unicode_a->get<json::String>(), "A" );
    } else {
        M_ASSERT_FAIL("Failed to parse Unicode escape \\u0041");
    }
    
    auto parsed_unicode_heart = json::parse("\"\\u2665\""); // ♥
    if (parsed_unicode_heart.has_value()) {
        M_ASSERT_EQ( parsed_unicode_heart->type(), json::Type::eString );
        M_ASSERT_EQ( parsed_unicode_heart->get<json::String>(), "♥" );
    } else {
        M_ASSERT_FAIL("Failed to parse Unicode escape \\u2665");
    }
    
    auto parsed_unicode_chinese = json::parse("\"\\u4e2d\\u6587\""); // 中文
    if (parsed_unicode_chinese.has_value()) {
        M_ASSERT_EQ( parsed_unicode_chinese->type(), json::Type::eString );
        M_ASSERT_EQ( parsed_unicode_chinese->get<json::String>(), "中文" );
    } else {
        M_ASSERT_FAIL("Failed to parse Unicode escape \\u4e2d\\u6587");
    }
    
    auto parsed_unicode_mixed = json::parse("\"Hello \\u4e16\\u754c!\""); // Hello 世界!
    if (parsed_unicode_mixed.has_value()) {
        M_ASSERT_EQ( parsed_unicode_mixed->type(), json::Type::eString );
        M_ASSERT_EQ( parsed_unicode_mixed->get<json::String>(), "Hello 世界!" );
    } else {
        M_ASSERT_FAIL("Failed to parse mixed Unicode escape");
    }
    
    // Test that Unicode characters are NOT re-encoded when serializing
    json::Value unicode_original{"Hello 世界! ♥"};
    auto unicode_serialized = unicode_original.serialize();
    M_ASSERT_EQ( unicode_serialized, "\"Hello 世界! ♥\"" ); // Should NOT be \\u encoded
    
    // Test round-trip: parse Unicode escapes, then serialize (should keep Unicode chars)
    auto unicode_roundtrip = json::parse("\"\\u4e16\\u754c\""); // 世界
    if (unicode_roundtrip.has_value()) {
        auto roundtrip_serialized = unicode_roundtrip->serialize();
        M_ASSERT_EQ( roundtrip_serialized, "\"世界\"" ); // Should be Unicode chars, not \\u escapes
    } else {
        M_ASSERT_FAIL("Failed Unicode round-trip test");
    }
    
    // Test special control characters that need escaping
    json::Value control_chars{"\x01\x02\x03\x1f"}; // Control characters
    auto control_serialized = control_chars.serialize();
    // Should be escaped as \u0001\u0002\u0003\u001f or similar
    M_ASSERT_TRUE( control_serialized.find("\\u") != std::string::npos );
    
    // Test type safety - wrong type access should throw
    M_ASSERT_THROW( std::ignore = str_val.get<json::Number>(), std::runtime_error );
    M_ASSERT_THROW( std::ignore = str_val.get<json::Bool>(), std::runtime_error );
    M_ASSERT_THROW( std::ignore = str_val.get<json::Array>(), std::runtime_error );
    M_ASSERT_THROW( std::ignore = str_val.get<json::Object>(), std::runtime_error );
    M_ASSERT_THROW( std::ignore = str_val.get<std::nullptr_t>(), std::runtime_error );
    
    // Test String get_ref type safety
    M_ASSERT_THROW( std::ignore = str_val.get_ref<json::Number>(), std::runtime_error );
    M_ASSERT_THROW( std::ignore = str_val.get_ref<json::Bool>(), std::runtime_error );
    M_ASSERT_THROW( std::ignore = str_val.get_ref<json::Array>(), std::runtime_error );
    M_ASSERT_THROW( std::ignore = str_val.get_ref<json::Object>(), std::runtime_error );
    M_ASSERT_THROW( std::ignore = str_val.get_ref<std::nullptr_t>(), std::runtime_error );
    
    // Test String to std::string conversion (should work)
    json::Value str_conv_test{"conversion test"};
    M_ASSERT_NO_THROW( auto std_str_conv = str_conv_test.get<std::string>() );
    M_ASSERT_EQ( str_conv_test.get<std::string>(), "conversion test" );
    
    // Test String to std::string_view conversion (should work)
    json::Value str_view_test{"string_view test"};
    M_ASSERT_NO_THROW( auto str_view_conv = str_view_test.get<std::string_view>() );
    M_ASSERT_EQ( str_view_test.get<std::string_view>(), "string_view test" );
    
    // Test std::string_view properties
    json::Value view_prop_test{"view properties"};
    auto view = view_prop_test.get<std::string_view>();
    M_ASSERT_EQ( view.size(), 15 );
    M_ASSERT_EQ( view.length(), 15 );
    M_ASSERT_FALSE( view.empty() );
    M_ASSERT_TRUE( view.starts_with("view") );
    M_ASSERT_TRUE( view.ends_with("properties") );
    M_ASSERT_TRUE( view.contains("prop") );
    
    // Test std::string_view with empty string
    json::Value empty_view_test{""};
    auto empty_view = empty_view_test.get<std::string_view>();
    M_ASSERT_EQ( empty_view.size(), 0 );
    M_ASSERT_TRUE( empty_view.empty() );
    
    // Test std::string_view with Unicode content
    json::Value unicode_view_test{"Unicode 测试 ♥"};
    auto unicode_view = unicode_view_test.get<std::string_view>();
    M_ASSERT_EQ( unicode_view, "Unicode 测试 ♥" );
    M_ASSERT_TRUE( unicode_view.contains("测试") );
    M_ASSERT_TRUE( unicode_view.contains("♥") );
    
    // Test std::string_view comparison with different types
    json::Value view_compare_test{"view comparison"};
    auto view_comp = view_compare_test.get<std::string_view>();
    M_ASSERT_EQ( view_comp, "view comparison" );
    M_ASSERT_EQ( view_comp, std::string("view comparison") );
    M_ASSERT_EQ( view_comp, view_compare_test.get<json::String>() );
    M_ASSERT_EQ( view_comp, view_compare_test.get<std::string>() );
    
    // Test that string_view points to the same data as the original string
    json::Value data_test{"data pointer test"};
    auto& original_string_ref = data_test.get_ref<json::String>();
    auto view_data = data_test.get<std::string_view>();
    M_ASSERT_EQ( view_data.data(), original_string_ref.data() ); // Should point to same memory
    M_ASSERT_EQ( view_data.size(), original_string_ref.size() );
    
    // Test string_view with special characters
    json::Value special_view_test{"Special\nChars\t\"Test\""};
    auto special_view = special_view_test.get<std::string_view>();
    M_ASSERT_EQ( special_view, "Special\nChars\t\"Test\"" );
    M_ASSERT_TRUE( special_view.contains("\n") );
    M_ASSERT_TRUE( special_view.contains("\t") );
    M_ASSERT_TRUE( special_view.contains("\"") );
    
    // Test round-trip serialization/parsing
    json::Value original_str_roundtrip{"Complex string with \"quotes\" and \n newlines \t tabs"};
    auto serialized_str = original_str_roundtrip.serialize();
    auto parsed_back_str = json::parse(serialized_str);
    if (parsed_back_str.has_value()) {
        M_ASSERT_EQ( parsed_back_str->type(), json::Type::eString );
        M_ASSERT_EQ( parsed_back_str->get<json::String>(), original_str_roundtrip.get<json::String>() );
    } else {
        M_ASSERT_FAIL("Failed to parse back serialized string");
    }
    
    // Test string length and content
    json::Value length_test{"12345"};
    M_ASSERT_EQ( length_test.get<json::String>().length(), 5 );
    M_ASSERT_EQ( length_test.get<json::String>().size(), 5 );
    M_ASSERT_FALSE( length_test.get<json::String>().empty() );
    
    json::Value empty_length_test{""};
    M_ASSERT_EQ( empty_length_test.get<json::String>().length(), 0 );
    M_ASSERT_TRUE( empty_length_test.get<json::String>().empty() );
    
    // Test string consistency
    json::Value consistency_str1{"same content"};
    json::Value consistency_str2{"same content"};
    json::Value consistency_str3 = json::String("same content");
    
    M_ASSERT_TRUE( consistency_str1 == consistency_str2 );
    M_ASSERT_TRUE( consistency_str2 == consistency_str3 );
    M_ASSERT_TRUE( consistency_str1 == consistency_str3 );
    
    M_ASSERT_TRUE( consistency_str1 == "same content" );
    M_ASSERT_TRUE( consistency_str2 == "same content" );
    M_ASSERT_TRUE( consistency_str3 == "same content" );
    
    // Test that all string values serialize to the same result
    M_ASSERT_EQ( consistency_str1.serialize(), consistency_str2.serialize() );
    M_ASSERT_EQ( consistency_str2.serialize(), consistency_str3.serialize() );
    M_ASSERT_EQ( consistency_str1.serialize(), "\"same content\"" );
}
