#include <vct/test_unit_macros.hpp>

import std;
import vct.test.unit;
import vct.tools.json;


using namespace vct::tools;

M_TEST(Value, Array) {
    // Test default array construction
    M_ASSERT_NO_THROW( json::Value arr_default{json::Type::eArray} );
    M_ASSERT_EQ( json::Value{json::Type::eArray}.type(), json::Type::eArray );
    M_ASSERT_EQ( json::Value{json::Type::eArray}.size(), 0 );
    M_ASSERT_TRUE( json::Value{json::Type::eArray}.to<json::Array>().empty() );
    
    // Test construction with explicit Array wrapper
    M_ASSERT_NO_THROW( json::Value arr_explicit{json::Array{}} );
    M_ASSERT_NO_THROW( json::Value arr_init{json::Array{1, 2, 3}} );
    
    // Test direct array construction
    json::Value arr_direct{json::Array{10, 20, 30}};
    M_ASSERT_EQ( arr_direct.type(), json::Type::eArray );
    M_ASSERT_EQ( arr_direct.size(), 3 );
    M_ASSERT_FALSE( arr_direct.to<json::Array>().empty() );
    
    // Test array with mixed types
    json::Value mixed_arr{json::Array{
        json::Number(42),  // explicit conversion example
        "hello",           // implicit string conversion
        true,              // implicit bool conversion
        nullptr            // implicit null conversion
    }};
    M_ASSERT_EQ( mixed_arr.type(), json::Type::eArray );
    M_ASSERT_EQ( mixed_arr.size(), 4 );
    
    // Test type checking
    json::Value arr_val{json::Array{1, 2, 3}};
    json::Value empty_arr{json::Array{}};
    
    M_ASSERT_EQ( arr_val.type(), json::Type::eArray );
    M_ASSERT_EQ( empty_arr.type(), json::Type::eArray );
    
    M_ASSERT_TRUE( arr_val.is(json::Type::eArray) );
    M_ASSERT_TRUE( empty_arr.is(json::Type::eArray) );
    
    M_ASSERT_FALSE( arr_val.is(json::Type::eString) );
    M_ASSERT_FALSE( arr_val.is(json::Type::eNumber) );
    M_ASSERT_FALSE( arr_val.is(json::Type::eBool) );
    M_ASSERT_FALSE( arr_val.is(json::Type::eNull) );
    M_ASSERT_FALSE( arr_val.is(json::Type::eObject) );
    
    M_EXPECT_STREQ( arr_val.type_name(), "Array" );
    M_EXPECT_STREQ( empty_arr.type_name(), "Array" );
    
    // Test size() function
    json::Value size_test{json::Array{1, 2, 3, 4, 5}};
    M_ASSERT_EQ( size_test.size(), 5 );
    M_ASSERT_EQ( size_test.to<json::Array>().size(), 5 );
    
    json::Value empty_size_test{json::Array{}};
    M_ASSERT_EQ( empty_size_test.size(), 0 );
    M_ASSERT_EQ( empty_size_test.to<json::Array>().size(), 0 );
    M_ASSERT_TRUE( empty_size_test.to<json::Array>().empty() );
    
    // Test [] operator (subscript access)
    json::Value subscript_test{json::Array{10, 20, 30, 40, 50}};
    M_ASSERT_EQ( subscript_test[0].to<json::Number>(), 10 );
    M_ASSERT_EQ( subscript_test[1].to<json::Number>(), 20 );
    M_ASSERT_EQ( subscript_test[2].to<json::Number>(), 30 );
    M_ASSERT_EQ( subscript_test[3].to<json::Number>(), 40 );
    M_ASSERT_EQ( subscript_test[4].to<json::Number>(), 50 );
    
    // Test [] operator with mixed types
    json::Value mixed_subscript{json::Array{
        42,     // implicit number conversion
        "test", // implicit string conversion
        true,   // implicit bool conversion
        nullptr // implicit null conversion
    }};
    M_ASSERT_EQ( mixed_subscript[0].to<json::Number>(), 42 );
    M_ASSERT_EQ( mixed_subscript[1].to<json::String>(), "test" );
    M_ASSERT_EQ( mixed_subscript[2].to<json::Bool>(), true );
    M_ASSERT_EQ( mixed_subscript[3].type(), json::Type::eNull );
    
    // Test [] operator modification
    json::Value modify_test{json::Array{1, 2, 3}};
    modify_test[0] = 100;         // implicit number conversion
    modify_test[1] = "modified";  // implicit string conversion
    modify_test[2] = false;       // implicit bool conversion
    
    M_ASSERT_EQ( modify_test[0].to<json::Number>(), 100 );
    M_ASSERT_EQ( modify_test[1].to<json::String>(), "modified" );
    M_ASSERT_EQ( modify_test[2].to<json::Bool>(), false );
    
    // Test const [] operator
    const json::Value const_arr{json::Array{1, 2, 3}};
    M_ASSERT_EQ( const_arr[0].to<json::Number>(), 1 );
    M_ASSERT_EQ( const_arr[1].to<json::Number>(), 2 );
    M_ASSERT_EQ( const_arr[2].to<json::Number>(), 3 );
    
    // Test at() function
    json::Value at_test{json::Array{100, 200, 300}};
    M_ASSERT_NO_THROW( std::ignore = at_test.at(0) );
    M_ASSERT_NO_THROW( std::ignore = at_test.at(1) );
    M_ASSERT_NO_THROW( std::ignore = at_test.at(2) );
    
    M_ASSERT_EQ( at_test.at(0).to<json::Number>(), 100 );
    M_ASSERT_EQ( at_test.at(1).to<json::Number>(), 200 );
    M_ASSERT_EQ( at_test.at(2).to<json::Number>(), 300 );
    
    // Test at() with out-of-bounds access (should throw)
    M_ASSERT_THROW( std::ignore = at_test.at(3), std::out_of_range );
    M_ASSERT_THROW( std::ignore = at_test.at(10), std::out_of_range );
    M_ASSERT_THROW( std::ignore = at_test.at(100), std::out_of_range );
    
    // Test at() with negative index (should throw)
    M_ASSERT_THROW( std::ignore = at_test.at(-1), std::out_of_range );
    M_ASSERT_THROW( std::ignore = at_test.at(-10), std::out_of_range );
    
    // Test at() with empty array (should throw)
    json::Value empty_at_test{json::Array{}};
    M_ASSERT_THROW( std::ignore = empty_at_test.at(0), std::out_of_range );
    M_ASSERT_THROW( std::ignore = empty_at_test.at(1), std::out_of_range );
    
    // Test const at() function
    const json::Value const_at_test{json::Array{1, 2, 3}};
    M_ASSERT_NO_THROW( std::ignore = const_at_test.at(0) );
    M_ASSERT_EQ( const_at_test.at(0).to<json::Number>(), 1 );
    M_ASSERT_EQ( const_at_test.at(1).to<json::Number>(), 2 );
    M_ASSERT_EQ( const_at_test.at(2).to<json::Number>(), 3 );
    M_ASSERT_THROW( std::ignore = const_at_test.at(3), std::out_of_range );
    
    // Test get<T>() method
    json::Value get_test{json::Array{1, 2, 3}};
    M_ASSERT_NO_THROW( std::ignore = get_test.to<json::Array>() );
    auto arr = get_test.to<json::Array>();
    M_ASSERT_EQ( arr.size(), 3 );
    M_ASSERT_EQ( arr[0].to<json::Number>(), 1 );
    M_ASSERT_EQ( arr[1].to<json::Number>(), 2 );
    M_ASSERT_EQ( arr[2].to<json::Number>(), 3 );
    
    // Test get<T>() method
    json::Value ref_test{json::Array{10, 20, 30}};
    M_ASSERT_NO_THROW( auto& ref = ref_test.get<json::Array>() );
    auto& arr_ref = ref_test.get<json::Array>();
    M_ASSERT_EQ( arr_ref.size(), 3 );
    
    // Test reference modification
    arr_ref[0] = 100;  // implicit number conversion
    arr_ref.push_back(40);
    M_ASSERT_EQ( ref_test.size(), 4 );
    M_ASSERT_EQ( ref_test[0].to<json::Number>(), 100 );
    M_ASSERT_EQ( ref_test[3].to<json::Number>(), 40 );
    
    // Test const get
    const json::Value const_ref_test{json::Array{1, 2, 3}};
    M_ASSERT_NO_THROW( auto& const_ref = const_ref_test.get<json::Array>() );
    const auto& const_arr_ref = const_ref_test.get<json::Array>();
    M_ASSERT_EQ( const_arr_ref.size(), 3 );
    M_ASSERT_EQ( const_arr_ref[0].to<json::Number>(), 1 );
    
    // Test get type safety - wrong type should throw
    json::Value wrong_type_val{42};
    M_ASSERT_THROW( std::ignore = wrong_type_val.get<json::Array>(), std::runtime_error );
    
    json::Value string_val{"not array"};
    M_ASSERT_THROW( std::ignore = string_val.get<json::Array>(), std::runtime_error );
    
    // Test assignment operations
    json::Value assign_val{json::Type::eArray};
    M_ASSERT_NO_THROW( assign_val = json::Array{1, 2, 3} );
    M_ASSERT_EQ( assign_val.size(), 3 );
    M_ASSERT_EQ( assign_val[0].to<json::Number>(), 1 );
    
    M_ASSERT_NO_THROW( assign_val = json::Array{10, 20} );
    M_ASSERT_EQ( assign_val.size(), 2 );
    M_ASSERT_EQ( assign_val[0].to<json::Number>(), 10 );
    M_ASSERT_EQ( assign_val[1].to<json::Number>(), 20 );
    
    // Test nested arrays (2D array)
    json::Value nested_2d{json::Array{
        json::Array{1, 2, 3},
        json::Array{4, 5, 6},
        json::Array{7, 8, 9}
    }};
    M_ASSERT_EQ( nested_2d.type(), json::Type::eArray );
    M_ASSERT_EQ( nested_2d.size(), 3 );
    
    // Test accessing nested array elements
    M_ASSERT_EQ( nested_2d[0].type(), json::Type::eArray );
    M_ASSERT_EQ( nested_2d[0].size(), 3 );
    M_ASSERT_EQ( nested_2d[0][0].to<json::Number>(), 1 );
    M_ASSERT_EQ( nested_2d[0][1].to<json::Number>(), 2 );
    M_ASSERT_EQ( nested_2d[0][2].to<json::Number>(), 3 );
    
    M_ASSERT_EQ( nested_2d[1][0].to<json::Number>(), 4 );
    M_ASSERT_EQ( nested_2d[1][1].to<json::Number>(), 5 );
    M_ASSERT_EQ( nested_2d[1][2].to<json::Number>(), 6 );
    
    M_ASSERT_EQ( nested_2d[2][0].to<json::Number>(), 7 );
    M_ASSERT_EQ( nested_2d[2][1].to<json::Number>(), 8 );
    M_ASSERT_EQ( nested_2d[2][2].to<json::Number>(), 9 );
    
    // Test nested arrays with at()
    M_ASSERT_NO_THROW( std::ignore = nested_2d.at(0).at(0) );
    M_ASSERT_EQ( nested_2d.at(0).at(0).to<json::Number>(), 1 );
    M_ASSERT_EQ( nested_2d.at(1).at(1).to<json::Number>(), 5 );
    M_ASSERT_EQ( nested_2d.at(2).at(2).to<json::Number>(), 9 );
    
    // Test nested arrays bounds checking
    M_ASSERT_THROW( std::ignore = nested_2d.at(3), std::out_of_range );
    M_ASSERT_THROW( std::ignore = nested_2d.at(0).at(3), std::out_of_range );
    M_ASSERT_THROW( std::ignore = nested_2d.at(1).at(-1), std::out_of_range );
    
    // Test 3D nested arrays
    json::Value nested_3d{json::Array{
        json::Array{
            json::Array{1, 2},
            json::Array{3, 4}
        },
        json::Array{
            json::Array{5, 6},
            json::Array{7, 8}
        }
    }};
    M_ASSERT_EQ( nested_3d.size(), 2 );
    M_ASSERT_EQ( nested_3d[0].size(), 2 );
    M_ASSERT_EQ( nested_3d[0][0].size(), 2 );
    
    M_ASSERT_EQ( nested_3d[0][0][0].to<json::Number>(), 1 );
    M_ASSERT_EQ( nested_3d[0][0][1].to<json::Number>(), 2 );
    M_ASSERT_EQ( nested_3d[0][1][0].to<json::Number>(), 3 );
    M_ASSERT_EQ( nested_3d[0][1][1].to<json::Number>(), 4 );
    M_ASSERT_EQ( nested_3d[1][0][0].to<json::Number>(), 5 );
    M_ASSERT_EQ( nested_3d[1][0][1].to<json::Number>(), 6 );
    M_ASSERT_EQ( nested_3d[1][1][0].to<json::Number>(), 7 );
    M_ASSERT_EQ( nested_3d[1][1][1].to<json::Number>(), 8 );
    
    // Test 3D arrays with at()
    M_ASSERT_EQ( nested_3d.at(0).at(0).at(0).to<json::Number>(), 1 );
    M_ASSERT_EQ( nested_3d.at(1).at(1).at(1).to<json::Number>(), 8 );
    M_ASSERT_THROW( std::ignore = nested_3d.at(0).at(0).at(2), std::out_of_range );
    M_ASSERT_THROW( std::ignore = nested_3d.at(2), std::out_of_range );
    
    // Test mixed nested arrays
    json::Value mixed_nested{json::Array{
        json::Array{1, 2, 3},
        json::String("nested string"),
        json::Array{
            json::Bool(true),
            json::Array{4, 5},
            nullptr         // implicit null conversion
        }
    }};
    M_ASSERT_EQ( mixed_nested.size(), 3 );
    M_ASSERT_EQ( mixed_nested[0].size(), 3 );
    M_ASSERT_EQ( mixed_nested[1].type(), json::Type::eString );
    M_ASSERT_EQ( mixed_nested[2].size(), 3 );
    
    M_ASSERT_EQ( mixed_nested[0][0].to<json::Number>(), 1 );
    M_ASSERT_EQ( mixed_nested[1].to<json::String>(), "nested string" );
    M_ASSERT_EQ( mixed_nested[2][0].to<json::Bool>(), true );
    M_ASSERT_EQ( mixed_nested[2][1].size(), 2 );
    M_ASSERT_EQ( mixed_nested[2][1][0].to<json::Number>(), 4 );
    M_ASSERT_EQ( mixed_nested[2][1][1].to<json::Number>(), 5 );
    M_ASSERT_EQ( mixed_nested[2][2].type(), json::Type::eNull );
    
    // Test irregular nested arrays (different sizes)
    json::Value irregular{json::Array{
        json::Array{1},
        json::Array{2, 3},
        json::Array{4, 5, 6},
        json::Array{7, 8, 9, 10}
    }};
    M_ASSERT_EQ( irregular.size(), 4 );
    M_ASSERT_EQ( irregular[0].size(), 1 );
    M_ASSERT_EQ( irregular[1].size(), 2 );
    M_ASSERT_EQ( irregular[2].size(), 3 );
    M_ASSERT_EQ( irregular[3].size(), 4 );
    
    M_ASSERT_EQ( irregular[0][0].to<json::Number>(), 1 );
    M_ASSERT_EQ( irregular[1][1].to<json::Number>(), 3 );
    M_ASSERT_EQ( irregular[2][2].to<json::Number>(), 6 );
    M_ASSERT_EQ( irregular[3][3].to<json::Number>(), 10 );
    
    // Test Value-to-Value comparison
    json::Value arr_cmp1{json::Array{1, 2, 3}};
    json::Value arr_cmp2{json::Array{1, 2, 3}};
    json::Value arr_cmp3{json::Array{1, 2, 4}};
    json::Value arr_cmp4{json::Array{1, 2}};
    
    M_ASSERT_TRUE( arr_cmp1 == arr_cmp2 );
    M_ASSERT_FALSE( arr_cmp1 == arr_cmp3 );
    M_ASSERT_FALSE( arr_cmp1 == arr_cmp4 );
    
    // Test array comparison with different types
    json::Value str_val{"not array"};
    json::Value num_val{42};
    M_ASSERT_FALSE( arr_cmp1 == str_val );
    M_ASSERT_FALSE( arr_cmp1 == num_val );
    
    // Test empty array comparison
    json::Value empty1{json::Array{}};
    json::Value empty2{json::Array{}};
    M_ASSERT_TRUE( empty1 == empty2 );
    M_ASSERT_FALSE( empty1 == arr_cmp1 );
    
    // Test nested array comparison
    json::Value nested_cmp1{json::Array{
        json::Array{1, 2},
        json::Array{3, 4}
    }};
    json::Value nested_cmp2{json::Array{
        json::Array{1, 2},
        json::Array{3, 4}
    }};
    json::Value nested_cmp3{json::Array{
        json::Array{1, 2},
        json::Array{3, 5}
    }};
    
    M_ASSERT_TRUE( nested_cmp1 == nested_cmp2 );
    M_ASSERT_FALSE( nested_cmp1 == nested_cmp3 );
    
    // Test basic serialization
    json::Value serial_test{json::Array{1, 2, 3}};
    M_ASSERT_NO_THROW( serial_test.serialize() );
    M_ASSERT_EQ( serial_test.serialize(), "[1,2,3]" );
    
    json::Value empty_serial{json::Array{}};
    M_ASSERT_EQ( empty_serial.serialize(), "[]" );
    
    // Test mixed type serialization
    json::Value mixed_serial{json::Array{
        42,       // implicit conversion
        "test",   // implicit conversion
        true,     // implicit conversion
        nullptr   // implicit null conversion
    }};
    M_ASSERT_EQ( mixed_serial.serialize(), "[42,\"test\",true,null]" );
    
    // Test nested array serialization
    json::Value nested_serial{json::Array{
        json::Array{1, 2},
        json::Array{3, 4}
    }};
    M_ASSERT_EQ( nested_serial.serialize(), "[[1,2],[3,4]]" );
    
    // Test pretty serialization
    M_ASSERT_NO_THROW( serial_test.serialize_pretty() );
    M_ASSERT_NO_THROW( nested_serial.serialize_pretty() );
    
    // Test basic parsing
    auto parsed_simple = json::parse("[1,2,3]");
    if (parsed_simple.has_value()) {
        M_ASSERT_EQ( parsed_simple->type(), json::Type::eArray );
        M_ASSERT_EQ( parsed_simple->size(), 3 );
        M_ASSERT_EQ( (*parsed_simple)[0].to<json::Number>(), 1 );
        M_ASSERT_EQ( (*parsed_simple)[1].to<json::Number>(), 2 );
        M_ASSERT_EQ( (*parsed_simple)[2].to<json::Number>(), 3 );
    } else {
        M_ASSERT_FAIL("Failed to parse simple array");
    }
    
    auto parsed_empty = json::parse("[]");
    if (parsed_empty.has_value()) {
        M_ASSERT_EQ( parsed_empty->type(), json::Type::eArray );
        M_ASSERT_EQ( parsed_empty->size(), 0 );
    } else {
        M_ASSERT_FAIL("Failed to parse empty array");
    }
    
    // Test nested array parsing
    auto parsed_nested = json::parse("[[1,2],[3,4]]");
    if (parsed_nested.has_value()) {
        M_ASSERT_EQ( parsed_nested->type(), json::Type::eArray );
        M_ASSERT_EQ( parsed_nested->size(), 2 );
        M_ASSERT_EQ( (*parsed_nested)[0].size(), 2 );
        M_ASSERT_EQ( (*parsed_nested)[1].size(), 2 );
        M_ASSERT_EQ( (*parsed_nested)[0][0].to<json::Number>(), 1 );
        M_ASSERT_EQ( (*parsed_nested)[0][1].to<json::Number>(), 2 );
        M_ASSERT_EQ( (*parsed_nested)[1][0].to<json::Number>(), 3 );
        M_ASSERT_EQ( (*parsed_nested)[1][1].to<json::Number>(), 4 );
    } else {
        M_ASSERT_FAIL("Failed to parse nested array");
    }
    
    // Test mixed type parsing
    auto parsed_mixed = json::parse("[42,\"test\",true,null]");
    if (parsed_mixed.has_value()) {
        M_ASSERT_EQ( parsed_mixed->type(), json::Type::eArray );
        M_ASSERT_EQ( parsed_mixed->size(), 4 );
        M_ASSERT_EQ( (*parsed_mixed)[0].to<json::Number>(), 42 );
        M_ASSERT_EQ( (*parsed_mixed)[1].to<json::String>(), "test" );
        M_ASSERT_EQ( (*parsed_mixed)[2].to<json::Bool>(), true );
        M_ASSERT_EQ( (*parsed_mixed)[3].type(), json::Type::eNull );
    } else {
        M_ASSERT_FAIL("Failed to parse mixed type array");
    }
    
    // Test round-trip serialization/parsing
    json::Value original_arr{json::Array{
        json::Array{1, 2},
        json::String("nested"),
        json::Array{3, 4, 5}
    }};
    auto serialized_arr = original_arr.serialize();
    auto parsed_back_arr = json::parse(serialized_arr);
    if (parsed_back_arr.has_value()) {
        M_ASSERT_EQ( parsed_back_arr->type(), json::Type::eArray );
        M_ASSERT_EQ( parsed_back_arr->size(), original_arr.size() );
        M_ASSERT_TRUE( *parsed_back_arr == original_arr );
    } else {
        M_ASSERT_FAIL("Failed to parse back serialized array");
    }
    
    // Test type safety - wrong type access should throw
    M_ASSERT_THROW( std::ignore = arr_val.to<json::Number>(), std::runtime_error );
    M_ASSERT_THROW( std::ignore = arr_val.to<json::String>(), std::runtime_error );
    M_ASSERT_THROW( std::ignore = arr_val.to<json::Bool>(), std::runtime_error );
    M_ASSERT_THROW( std::ignore = arr_val.to<json::Object>(), std::runtime_error );
    M_ASSERT_THROW( std::ignore = arr_val.to<std::nullptr_t>(), std::runtime_error );
    
    // Test Array get type safety
    M_ASSERT_THROW( std::ignore = arr_val.get<json::Number>(), std::runtime_error );
    M_ASSERT_THROW( std::ignore = arr_val.get<json::String>(), std::runtime_error );
    M_ASSERT_THROW( std::ignore = arr_val.get<json::Bool>(), std::runtime_error );
    M_ASSERT_THROW( std::ignore = arr_val.get<json::Object>(), std::runtime_error );
    M_ASSERT_THROW( std::ignore = arr_val.get<std::nullptr_t>(), std::runtime_error );
    
    // Test array consistency across different construction methods
    json::Value consistency_arr1{json::Array{1, 2, 3}};
    json::Value consistency_arr2{json::Array{1, 2, 3}};
    
    M_ASSERT_TRUE( consistency_arr1 == consistency_arr2 );
    M_ASSERT_EQ( consistency_arr1.serialize(), consistency_arr2.serialize() );
    M_ASSERT_EQ( consistency_arr1.size(), consistency_arr2.size() );
    
    // Test deep nesting limits (reasonable depth)
    json::Value deep_nested{json::Array{
        json::Array{
            json::Array{
                json::Array{1, 2}
            }
        }
    }};
    M_ASSERT_EQ( deep_nested[0][0][0][0].to<json::Number>(), 1 );
    M_ASSERT_EQ( deep_nested[0][0][0][1].to<json::Number>(), 2 );
    
    // Test complex high-dimensional array with mixed types
    json::Value complex_multi_dim{json::Array{
        // First dimension: Array of different data structures
        json::Array{
            // 2D matrix of numbers
            json::Array{
                json::Array{1, 2, 3},
                json::Array{4, 5, 6},
                json::Array{7, 8, 9}
            },
            // 2D matrix of strings
            json::Array{
                json::Array{"a", "b", "c"},
                json::Array{"d", "e", "f"},
                json::Array{"g", "h", "i"}
            },
            // 2D matrix of booleans
            json::Array{
                json::Array{true, false, true},
                json::Array{false, true, false},
                json::Array{true, true, false}
            }
        },
        // Second dimension: Array of mixed nested structures
        json::Array{
            // 3D cube of numbers
            json::Array{
                json::Array{
                    json::Array{10, 11, 12},
                    json::Array{13, 14, 15}
                },
                json::Array{
                    json::Array{16, 17, 18},
                    json::Array{19, 20, 21}
                }
            },
            // 3D structure with mixed types
            json::Array{
                json::Array{
                    json::Array{"x", "y", "z"},
                    json::Array{true, false, nullptr}  // implicit null conversion
                },
                json::Array{
                    json::Array{100, 200, 300},
                    json::Array{"alpha", "beta", "gamma"}
                }
            }
        },
        // Third dimension: Array of irregular structures
        json::Array{
            // Irregular 4D structure
            json::Array{
                json::Array{
                    json::Array{
                        json::Array{1000, 2000},
                        json::Array{3000, 4000, 5000}
                    },
                    json::Array{
                        json::Array{"deep", "nested"},
                        json::Array{true, false, nullptr, "mixed"}  // implicit null conversion
                    }
                },
                json::Array{
                    json::Array{
                        json::Array{nullptr, "null_test"},  // implicit null conversion
                        json::Array{42, 43, 44}
                    }
                }
            },
            // Another irregular structure
            json::Array{
                json::Array{
                    json::Array{
                        json::Array{9.5, 8.5, 7.5},
                        json::Array{"final", "level"}
                    }
                },
                json::String("single_string_element"),
                json::Number(999),
                json::Bool(true)
            }
        }
    }};
    
    // Test the structure dimensions
    M_ASSERT_EQ( complex_multi_dim.size(), 3 );
    M_ASSERT_EQ( complex_multi_dim[0].size(), 3 );
    M_ASSERT_EQ( complex_multi_dim[1].size(), 2 );
    M_ASSERT_EQ( complex_multi_dim[2].size(), 2 );
    
    // Test first dimension - 2D matrices
    // Numbers matrix
    M_ASSERT_EQ( complex_multi_dim[0][0].size(), 3 );
    M_ASSERT_EQ( complex_multi_dim[0][0][0].size(), 3 );
    M_ASSERT_EQ( complex_multi_dim[0][0][0][0].to<json::Number>(), 1 );
    M_ASSERT_EQ( complex_multi_dim[0][0][1][1].to<json::Number>(), 5 );
    M_ASSERT_EQ( complex_multi_dim[0][0][2][2].to<json::Number>(), 9 );
    
    // Strings matrix
    M_ASSERT_EQ( complex_multi_dim[0][1][0][0].to<json::String>(), "a" );
    M_ASSERT_EQ( complex_multi_dim[0][1][1][1].to<json::String>(), "e" );
    M_ASSERT_EQ( complex_multi_dim[0][1][2][2].to<json::String>(), "i" );
    
    // Booleans matrix
    M_ASSERT_EQ( complex_multi_dim[0][2][0][0].to<json::Bool>(), true );
    M_ASSERT_EQ( complex_multi_dim[0][2][1][1].to<json::Bool>(), true );
    M_ASSERT_EQ( complex_multi_dim[0][2][2][2].to<json::Bool>(), false );
    
    // Test second dimension - 3D structures
    // 3D numbers cube
    M_ASSERT_EQ( complex_multi_dim[1][0][0][0][0].to<json::Number>(), 10 );
    M_ASSERT_EQ( complex_multi_dim[1][0][0][0][2].to<json::Number>(), 12 );
    M_ASSERT_EQ( complex_multi_dim[1][0][1][1][2].to<json::Number>(), 21 );
    
    // 3D mixed structure
    M_ASSERT_EQ( complex_multi_dim[1][1][0][0][0].to<json::String>(), "x" );
    M_ASSERT_EQ( complex_multi_dim[1][1][0][1][0].to<json::Bool>(), true );
    M_ASSERT_EQ( complex_multi_dim[1][1][0][1][2].type(), json::Type::eNull );
    M_ASSERT_EQ( complex_multi_dim[1][1][1][0][1].to<json::Number>(), 200 );
    M_ASSERT_EQ( complex_multi_dim[1][1][1][1][2].to<json::String>(), "gamma" );
    
    // Test third dimension - irregular 4D structures
    // First irregular 4D
    M_ASSERT_EQ( complex_multi_dim[2][0][0][0][0][0].to<json::Number>(), 1000 );
    M_ASSERT_EQ( complex_multi_dim[2][0][0][0][1][2].to<json::Number>(), 5000 );
    M_ASSERT_EQ( complex_multi_dim[2][0][0][1][0][0].to<json::String>(), "deep" );
    M_ASSERT_EQ( complex_multi_dim[2][0][0][1][1][3].to<json::String>(), "mixed" );
    M_ASSERT_EQ( complex_multi_dim[2][0][1][0][0][0].type(), json::Type::eNull );
    M_ASSERT_EQ( complex_multi_dim[2][0][1][0][1][1].to<json::Number>(), 43 );
    
    // Second irregular structure
    M_ASSERT_EQ( complex_multi_dim[2][1][0][0][0][0].to<json::Number>(), 9.5 );
    M_ASSERT_EQ( complex_multi_dim[2][1][0][0][1][0].to<json::String>(), "final" );
    M_ASSERT_EQ( complex_multi_dim[2][1][1].to<json::String>(), "single_string_element" );
    M_ASSERT_EQ( complex_multi_dim[2][1][2].to<json::Number>(), 999 );
    M_ASSERT_EQ( complex_multi_dim[2][1][3].to<json::Bool>(), true );
    
    // Test at() function on complex structure
    M_ASSERT_NO_THROW( std::ignore = complex_multi_dim.at(0).at(0).at(0).at(0) );
    M_ASSERT_EQ( complex_multi_dim.at(0).at(0).at(0).at(0).to<json::Number>(), 1 );
    M_ASSERT_EQ( complex_multi_dim.at(1).at(1).at(0).at(0).at(1).to<json::String>(), "y" );
    M_ASSERT_EQ( complex_multi_dim.at(2).at(0).at(0).at(0).at(0).at(1).to<json::Number>(), 2000 );
    
    // Test bounds checking on complex structure
    M_ASSERT_THROW( std::ignore = complex_multi_dim.at(3), std::out_of_range );
    M_ASSERT_THROW( std::ignore = complex_multi_dim.at(0).at(3), std::out_of_range );
    M_ASSERT_THROW( std::ignore = complex_multi_dim.at(1).at(0).at(2), std::out_of_range );
    M_ASSERT_THROW( std::ignore = complex_multi_dim.at(2).at(0).at(0).at(0).at(0).at(2), std::out_of_range );
    
    // Test serialization of complex structure
    M_ASSERT_NO_THROW( complex_multi_dim.serialize() );
    auto complex_serialized = complex_multi_dim.serialize();
    M_ASSERT_TRUE( complex_serialized.size() > 100 ); // Should be quite long
    M_ASSERT_TRUE( complex_serialized.contains("[") );
    M_ASSERT_TRUE( complex_serialized.contains("]") );
    M_ASSERT_TRUE( complex_serialized.contains("\"a\"") );
    M_ASSERT_TRUE( complex_serialized.contains("true") );
    M_ASSERT_TRUE( complex_serialized.contains("false") );
    M_ASSERT_TRUE( complex_serialized.contains("null") );
    M_ASSERT_TRUE( complex_serialized.contains("1000") );
    M_ASSERT_TRUE( complex_serialized.contains("\"deep\"") );
    
    // Test pretty serialization
    M_ASSERT_NO_THROW( complex_multi_dim.serialize_pretty() );
    auto complex_pretty = complex_multi_dim.serialize_pretty();
    if (complex_pretty.has_value()) {
        M_ASSERT_TRUE( complex_pretty->size() > complex_serialized.size() ); // Pretty should be longer
    } else {
        M_ASSERT_FAIL("Failed to serialize complex structure pretty");
    }
    
    // Test round-trip serialization/parsing for complex structure
    auto parsed_complex = json::parse(complex_serialized);
    if (parsed_complex.has_value()) {
        M_ASSERT_EQ( parsed_complex->type(), json::Type::eArray );
        M_ASSERT_EQ( parsed_complex->size(), 3 );
        // Test a few key elements to ensure structure is preserved
        M_ASSERT_EQ( (*parsed_complex)[0][0][0][0].to<json::Number>(), 1 );
        M_ASSERT_EQ( (*parsed_complex)[0][1][1][1].to<json::String>(), "e" );
        M_ASSERT_EQ( (*parsed_complex)[1][1][0][1][2].type(), json::Type::eNull );
        M_ASSERT_EQ( (*parsed_complex)[2][0][0][1][0][0].to<json::String>(), "deep" );
        M_ASSERT_EQ( (*parsed_complex)[2][1][2].to<json::Number>(), 999 );
        // Test full equality
        M_ASSERT_TRUE( *parsed_complex == complex_multi_dim );
    } else {
        M_ASSERT_FAIL("Failed to parse complex multi-dimensional array");
    }
    
    // Test modification of complex structure
    json::Value complex_copy = complex_multi_dim;
    complex_copy[0][0][0][0] = json::Number(999);
    complex_copy[1][1][0][0][0] = json::String("modified");
    complex_copy[2][0][0][0][0][0] = json::Bool(false);
    
    M_ASSERT_EQ( complex_copy[0][0][0][0].to<json::Number>(), 999 );
    M_ASSERT_EQ( complex_copy[1][1][0][0][0].to<json::String>(), "modified" );
    M_ASSERT_EQ( complex_copy[2][0][0][0][0][0].to<json::Bool>(), false );
    
    // Original should remain unchanged
    M_ASSERT_EQ( complex_multi_dim[0][0][0][0].to<json::Number>(), 1 );
    M_ASSERT_EQ( complex_multi_dim[1][1][0][0][0].to<json::String>(), "x" );
    M_ASSERT_EQ( complex_multi_dim[2][0][0][0][0][0].to<json::Number>(), 1000 );
    
    // Test that modified and original are different
    M_ASSERT_FALSE( complex_copy == complex_multi_dim );
    
    // Test modification through [] operator reference
    json::Value ref_modify_test{json::Array{1, 2, 3, 4, 5}};
    
    // Get references and modify through them
    auto& ref0 = ref_modify_test[0];
    auto& ref1 = ref_modify_test[1];
    auto& ref4 = ref_modify_test[4];
    
    ref0 = 100;        // implicit conversion
    ref1 = "modified"; // implicit conversion
    ref4 = true;       // implicit conversion
    
    M_ASSERT_EQ( ref_modify_test[0].to<json::Number>(), 100 );
    M_ASSERT_EQ( ref_modify_test[1].to<json::String>(), "modified" );
    M_ASSERT_EQ( ref_modify_test[2].to<json::Number>(), 3 ); // unchanged
    M_ASSERT_EQ( ref_modify_test[3].to<json::Number>(), 4 ); // unchanged
    M_ASSERT_EQ( ref_modify_test[4].to<json::Bool>(), true );
    
    // Test modification through at() reference
    json::Value at_modify_test{json::Array{10, 20, 30, 40, 50}};
    
    // Get references through at() and modify
    auto& at_ref0 = at_modify_test.at(0);
    auto& at_ref2 = at_modify_test.at(2);
    auto& at_ref4 = at_modify_test.at(4);
    
    at_ref0 = "first";     // implicit conversion
    at_ref2 = false;       // implicit conversion
    at_ref4 = nullptr;     // implicit null conversion
    
    M_ASSERT_EQ( at_modify_test.at(0).to<json::String>(), "first" );
    M_ASSERT_EQ( at_modify_test.at(1).to<json::Number>(), 20 ); // unchanged
    M_ASSERT_EQ( at_modify_test.at(2).to<json::Bool>(), false );
    M_ASSERT_EQ( at_modify_test.at(3).to<json::Number>(), 40 ); // unchanged
    M_ASSERT_EQ( at_modify_test.at(4).type(), json::Type::eNull );
    
    // Test nested array modification through [] references
    json::Value nested_ref_modify{json::Array{
        json::Array{1, 2, 3},
        json::Array{4, 5, 6},
        json::Array{7, 8, 9}
    }};
    
    // Get nested references and modify
    auto& nested_ref00 = nested_ref_modify[0][0];
    auto& nested_ref11 = nested_ref_modify[1][1];
    auto& nested_ref22 = nested_ref_modify[2][2];
    
    nested_ref00 = "corner1";   // implicit conversion
    nested_ref11 = true;        // implicit conversion
    nested_ref22 = nullptr;     // implicit null conversion
    
    M_ASSERT_EQ( nested_ref_modify[0][0].to<json::String>(), "corner1" );
    M_ASSERT_EQ( nested_ref_modify[0][1].to<json::Number>(), 2 ); // unchanged
    M_ASSERT_EQ( nested_ref_modify[1][1].to<json::Bool>(), true );
    M_ASSERT_EQ( nested_ref_modify[1][0].to<json::Number>(), 4 ); // unchanged
    M_ASSERT_EQ( nested_ref_modify[2][2].type(), json::Type::eNull );
    M_ASSERT_EQ( nested_ref_modify[2][1].to<json::Number>(), 8 ); // unchanged
    
    // Test nested array modification through at() references
    json::Value nested_at_modify{json::Array{
        json::Array{100, 200, 300},
        json::Array{400, 500, 600},
        json::Array{700, 800, 900}
    }};
    
    // Get nested references through at() and modify
    auto& nested_at_ref01 = nested_at_modify.at(0).at(1);
    auto& nested_at_ref12 = nested_at_modify.at(1).at(2);
    auto& nested_at_ref20 = nested_at_modify.at(2).at(0);
    
    nested_at_ref01 = "at_modified";  // implicit conversion
    nested_at_ref12 = false;          // implicit conversion
    nested_at_ref20 = nullptr;        // implicit null conversion
    
    M_ASSERT_EQ( nested_at_modify.at(0).at(0).to<json::Number>(), 100 ); // unchanged
    M_ASSERT_EQ( nested_at_modify.at(0).at(1).to<json::String>(), "at_modified" );
    M_ASSERT_EQ( nested_at_modify.at(0).at(2).to<json::Number>(), 300 ); // unchanged
    M_ASSERT_EQ( nested_at_modify.at(1).at(0).to<json::Number>(), 400 ); // unchanged
    M_ASSERT_EQ( nested_at_modify.at(1).at(1).to<json::Number>(), 500 ); // unchanged
    M_ASSERT_EQ( nested_at_modify.at(1).at(2).to<json::Bool>(), false );
    M_ASSERT_EQ( nested_at_modify.at(2).at(0).type(), json::Type::eNull );
    M_ASSERT_EQ( nested_at_modify.at(2).at(1).to<json::Number>(), 800 ); // unchanged
    M_ASSERT_EQ( nested_at_modify.at(2).at(2).to<json::Number>(), 900 ); // unchanged
    
    // Test 3D array modification through mixed [] and at() references
    json::Value mixed_3d_modify{json::Array{
        json::Array{
            json::Array{1, 2, 3},
            json::Array{4, 5, 6}
        },
        json::Array{
            json::Array{7, 8, 9},
            json::Array{10, 11, 12}
        }
    }};
    
    // Mix [] and at() access for modification
    auto& mixed_ref000 = mixed_3d_modify[0][0][0];
    auto& mixed_ref101 = mixed_3d_modify[1].at(0)[1];
    auto& mixed_ref112 = mixed_3d_modify.at(1)[1].at(2);
    
    mixed_ref000 = json::String("mixed_000");
    mixed_ref101 = json::Bool(true);
    mixed_ref112 = nullptr;     // implicit null conversion
    
    M_ASSERT_EQ( mixed_3d_modify[0][0][0].to<json::String>(), "mixed_000" );
    M_ASSERT_EQ( mixed_3d_modify[0][0][1].to<json::Number>(), 2 ); // unchanged
    M_ASSERT_EQ( mixed_3d_modify[1][0][1].to<json::Bool>(), true );
    M_ASSERT_EQ( mixed_3d_modify[1][0][0].to<json::Number>(), 7 ); // unchanged
    M_ASSERT_EQ( mixed_3d_modify[1][1][2].type(), json::Type::eNull );
    M_ASSERT_EQ( mixed_3d_modify[1][1][1].to<json::Number>(), 11 ); // unchanged
    
    // Test modification of complex structure through references
    json::Value complex_ref_modify{json::Array{
        json::Array{1, 2, 3},
        json::String("original"),
        json::Array{
            json::Bool(true),
            json::Array{4, 5},
            nullptr         // implicit null conversion
        }
    }};
    
    // Get various nested references and modify
    auto& complex_ref0 = complex_ref_modify[0];
    auto& complex_ref1 = complex_ref_modify[1];
    auto& complex_ref20 = complex_ref_modify[2][0];
    auto& complex_ref211 = complex_ref_modify.at(2).at(1).at(1);
    
    complex_ref0 = json::String("replaced_array");
    complex_ref1 = json::Number(999);
    complex_ref20 = json::String("was_bool");
    complex_ref211 = json::Bool(false);
    
    M_ASSERT_EQ( complex_ref_modify[0].to<json::String>(), "replaced_array" );
    M_ASSERT_EQ( complex_ref_modify[1].to<json::Number>(), 999 );
    M_ASSERT_EQ( complex_ref_modify[2][0].to<json::String>(), "was_bool" );
    M_ASSERT_EQ( complex_ref_modify[2][1][0].to<json::Number>(), 4 ); // unchanged
    M_ASSERT_EQ( complex_ref_modify[2][1][1].to<json::Bool>(), false );
    M_ASSERT_EQ( complex_ref_modify[2][2].type(), json::Type::eNull ); // unchanged
    
    // Test reference persistence and multiple modifications
    json::Value persist_test{json::Array{1, 2, 3}};
    auto& persist_ref = persist_test[1];
    
    // First modification
    persist_ref = json::String("first_change");
    M_ASSERT_EQ( persist_test[1].to<json::String>(), "first_change" );
    
    // Second modification through same reference
    persist_ref = json::Bool(true);
    M_ASSERT_EQ( persist_test[1].to<json::Bool>(), true );
    
    // Third modification through same reference
    persist_ref = nullptr;      // implicit null conversion
    M_ASSERT_EQ( persist_test[1].type(), json::Type::eNull );
    
    // Verify other elements unchanged
    M_ASSERT_EQ( persist_test[0].to<json::Number>(), 1 );
    M_ASSERT_EQ( persist_test[2].to<json::Number>(), 3 );
    
    // Test modification through multiple references to same element
    json::Value multi_ref_test{json::Array{100, 200, 300}};
    auto& multi_ref1 = multi_ref_test[1];
    auto& multi_ref1_again = multi_ref_test.at(1);
    
    multi_ref1 = json::String("ref1_change");
    M_ASSERT_EQ( multi_ref_test[1].to<json::String>(), "ref1_change" );
    M_ASSERT_EQ( multi_ref1_again.to<json::String>(), "ref1_change" );
    
    multi_ref1_again = json::Bool(false);
    M_ASSERT_EQ( multi_ref_test[1].to<json::Bool>(), false );
    M_ASSERT_EQ( multi_ref1.to<json::Bool>(), false );
    
    // Test serialization after reference modifications
    json::Value serial_after_ref{json::Array{1, 2, 3}};
    serial_after_ref[1] = "modified";  // implicit conversion
    
    auto serialized_after = serial_after_ref.serialize();
    M_ASSERT_EQ( serialized_after, "[1,\"modified\",3]" );
    
    // Test round-trip after reference modifications
    auto parsed_after = json::parse(serialized_after);
    if (parsed_after.has_value()) {
        M_ASSERT_TRUE( *parsed_after == serial_after_ref );
        M_ASSERT_EQ( (*parsed_after)[1].to<json::String>(), "modified" );
    } else {
        M_ASSERT_FAIL("Failed to parse after reference modification");
    }
    
    // Test contains() method - should always return false for arrays
    // (contains is only for testing map/object keys, not array elements)
    json::Value contains_test{json::Array{1, 2, 3, "hello", true}};
    
    // Test with values that exist in the array - should still return false
    M_ASSERT_FALSE( contains_test.contains("hello") );     // string exists but as value, not key
    M_ASSERT_FALSE( contains_test.contains("1") );         // number exists but as value, not key
    M_ASSERT_FALSE( contains_test.contains("0") );         // valid index but not a key
    
    // Test with non-existent keys
    M_ASSERT_FALSE( contains_test.contains("nonexistent") );
    M_ASSERT_FALSE( contains_test.contains("") );          // empty string
    
    // Test with empty array
    json::Value empty_contains_test{json::Array{}};
    M_ASSERT_FALSE( empty_contains_test.contains("anything") );
    
    // Test arrays with mixed value types and comparisons
    json::Value mixed_types_array{json::Array{
        2,          // implicit number conversion
        "12",       // implicit string conversion
        false,      // implicit bool conversion
        nullptr,    // implicit null conversion
        124.231     // implicit number conversion
    }};
    
    // Test individual element types and values
    M_ASSERT_EQ( mixed_types_array.size(), 5 );
    M_ASSERT_EQ( mixed_types_array[0].type(), json::Type::eNumber );
    M_ASSERT_EQ( mixed_types_array[1].type(), json::Type::eString );
    M_ASSERT_EQ( mixed_types_array[2].type(), json::Type::eBool );
    M_ASSERT_EQ( mixed_types_array[3].type(), json::Type::eNull );
    M_ASSERT_EQ( mixed_types_array[4].type(), json::Type::eNumber );
    
    // Test individual element values
    M_ASSERT_EQ( mixed_types_array[0].to<json::Number>(), 2 );
    M_ASSERT_EQ( mixed_types_array[1].to<json::String>(), "12" );
    M_ASSERT_EQ( mixed_types_array[2].to<json::Bool>(), false );
    M_ASSERT_EQ( mixed_types_array[3].type(), json::Type::eNull );
    M_ASSERT_EQ( mixed_types_array[4].to<json::Number>(), 124.231 );
    
    // Test batch comparison - identical arrays should be equal
    json::Value mixed_types_array_copy{json::Array{
        2,
        "12",
        false,
        nullptr,    // implicit null conversion
        124.231
    }};
    M_ASSERT_TRUE( mixed_types_array == mixed_types_array_copy );
    
    // Test batch comparison - different values should not be equal
    json::Value mixed_types_array_diff1{json::Array{
        3,          // different number
        "12",
        false,
        nullptr,    // implicit null conversion
        124.231
    }};
    M_ASSERT_FALSE( mixed_types_array == mixed_types_array_diff1 );
    
    json::Value mixed_types_array_diff2{json::Array{
        2,
        "13",       // different string
        false,
        nullptr,    // implicit null conversion
        124.231
    }};
    M_ASSERT_FALSE( mixed_types_array == mixed_types_array_diff2 );
    
    json::Value mixed_types_array_diff3{json::Array{
        2,
        "12",
        true,         // different bool
        nullptr,      // implicit null conversion
        124.231
    }};
    M_ASSERT_FALSE( mixed_types_array == mixed_types_array_diff3 );
    
    json::Value mixed_types_array_diff4{json::Array{
        2,
        "12",
        false,
        "not_null",   // different type (not null)
        124.231
    }};
    M_ASSERT_FALSE( mixed_types_array == mixed_types_array_diff4 );
    
    json::Value mixed_types_array_diff5{json::Array{
        2,
        "12",
        false,
        nullptr,      // implicit null conversion
        124.232       // different decimal
    }};
    M_ASSERT_FALSE( mixed_types_array == mixed_types_array_diff5 );
    
    // Test batch comparison - different sizes should not be equal
    json::Value mixed_types_array_shorter{json::Array{
        2,
        "12",
        false,
        nullptr     // implicit null conversion
        // missing last element
    }};
    M_ASSERT_FALSE( mixed_types_array == mixed_types_array_shorter );
    
    json::Value mixed_types_array_longer{json::Array{
        2,
        "12",
        false,
        nullptr,    // implicit null conversion
        124.231,
        "extra"     // extra element
    }};
    M_ASSERT_FALSE( mixed_types_array == mixed_types_array_longer );
    
    // Test individual element comparisons within same array
    M_ASSERT_FALSE( mixed_types_array[0] == mixed_types_array[1] ); // 2 != "12"
    M_ASSERT_FALSE( mixed_types_array[0] == mixed_types_array[2] ); // 2 != false
    M_ASSERT_FALSE( mixed_types_array[0] == mixed_types_array[3] ); // 2 != null
    M_ASSERT_FALSE( mixed_types_array[0] == mixed_types_array[4] ); // 2 != 124.231
    M_ASSERT_FALSE( mixed_types_array[1] == mixed_types_array[2] ); // "12" != false
    M_ASSERT_FALSE( mixed_types_array[1] == mixed_types_array[3] ); // "12" != null
    M_ASSERT_FALSE( mixed_types_array[1] == mixed_types_array[4] ); // "12" != 124.231
    M_ASSERT_FALSE( mixed_types_array[2] == mixed_types_array[3] ); // false != null
    M_ASSERT_FALSE( mixed_types_array[2] == mixed_types_array[4] ); // false != 124.231
    M_ASSERT_FALSE( mixed_types_array[3] == mixed_types_array[4] ); // null != 124.231
    
    // Test individual element comparisons across arrays
    M_ASSERT_TRUE( mixed_types_array[0] == mixed_types_array_copy[0] ); // 2 == 2
    M_ASSERT_TRUE( mixed_types_array[1] == mixed_types_array_copy[1] ); // "12" == "12"
    M_ASSERT_TRUE( mixed_types_array[2] == mixed_types_array_copy[2] ); // false == false
    M_ASSERT_TRUE( mixed_types_array[3] == mixed_types_array_copy[3] ); // null == null
    M_ASSERT_TRUE( mixed_types_array[4] == mixed_types_array_copy[4] ); // 124.231 == 124.231
    
    M_ASSERT_FALSE( mixed_types_array[0] == mixed_types_array_diff1[0] ); // 2 != 3
    M_ASSERT_FALSE( mixed_types_array[1] == mixed_types_array_diff2[1] ); // "12" != "13"
    M_ASSERT_FALSE( mixed_types_array[2] == mixed_types_array_diff3[2] ); // false != true
    M_ASSERT_FALSE( mixed_types_array[3] == mixed_types_array_diff4[3] ); // null != "not_null"
    M_ASSERT_FALSE( mixed_types_array[4] == mixed_types_array_diff5[4] ); // 124.231 != 124.232
    
    // Test more complex mixed type arrays
    json::Value complex_mixed{json::Array{
        -42,            // implicit conversion
        "",             // empty string - implicit conversion
        json::Bool(true), // explicit conversion example
        nullptr,        // implicit null conversion
        0,              // implicit conversion
        "null",         // string "null" - implicit conversion
        false,          // implicit conversion
        3.14159,        // implicit conversion
        "false",        // string "false" - implicit conversion
        1e10            // scientific notation - implicit conversion
    }};
    
    M_ASSERT_EQ( complex_mixed.size(), 10 );
    M_ASSERT_EQ( complex_mixed[0].to<json::Number>(), -42 );
    M_ASSERT_EQ( complex_mixed[1].to<json::String>(), "" );
    M_ASSERT_EQ( complex_mixed[2].to<json::Bool>(), true );
    M_ASSERT_EQ( complex_mixed[3].type(), json::Type::eNull );
    M_ASSERT_EQ( complex_mixed[4].to<json::Number>(), 0 );
    M_ASSERT_EQ( complex_mixed[5].to<json::String>(), "null" );
    M_ASSERT_EQ( complex_mixed[6].to<json::Bool>(), false );
    M_ASSERT_EQ( complex_mixed[7].to<json::Number>(), 3.14159 );
    M_ASSERT_EQ( complex_mixed[8].to<json::String>(), "false" );
    M_ASSERT_EQ( complex_mixed[9].to<json::Number>(), 1e10 );
    
    // Test that string representations don't equal actual values
    M_ASSERT_FALSE( complex_mixed[3] == complex_mixed[5] ); // null != "null"
    M_ASSERT_FALSE( complex_mixed[6] == complex_mixed[8] ); // false != "false"
    M_ASSERT_FALSE( complex_mixed[4] == complex_mixed[1] ); // 0 != ""
    
    // Test batch comparison with complex mixed array
    json::Value complex_mixed_copy{json::Array{
        -42,
        "",
        true,
        nullptr,    // implicit null conversion
        0,
        "null",
        false,
        3.14159,
        "false",
        1e10
    }};
    M_ASSERT_TRUE( complex_mixed == complex_mixed_copy );
    
    // Test nested arrays with mixed types
    json::Value nested_mixed{json::Array{
        json::Array{1, "one", true},  // mix of implicit conversions
        json::Array{2.5, nullptr, false},  // mix with implicit null conversion
        json::Array{json::String("three"), json::Number(3), json::Array{3.1, 3.2, 3.3}}  // mix of explicit and implicit
    }};
    
    M_ASSERT_EQ( nested_mixed.size(), 3 );
    M_ASSERT_EQ( nested_mixed[0].size(), 3 );
    M_ASSERT_EQ( nested_mixed[1].size(), 3 );
    M_ASSERT_EQ( nested_mixed[2].size(), 3 );
    
    // Test nested individual element comparisons
    M_ASSERT_EQ( nested_mixed[0][0].to<json::Number>(), 1 );
    M_ASSERT_EQ( nested_mixed[0][1].to<json::String>(), "one" );
    M_ASSERT_EQ( nested_mixed[0][2].to<json::Bool>(), true );
    M_ASSERT_EQ( nested_mixed[1][0].to<json::Number>(), 2.5 );
    M_ASSERT_EQ( nested_mixed[1][1].type(), json::Type::eNull );
    M_ASSERT_EQ( nested_mixed[1][2].to<json::Bool>(), false );
    M_ASSERT_EQ( nested_mixed[2][0].to<json::String>(), "three" );
    M_ASSERT_EQ( nested_mixed[2][1].to<json::Number>(), 3 );
    M_ASSERT_EQ( nested_mixed[2][2].type(), json::Type::eArray );
    M_ASSERT_EQ( nested_mixed[2][2][0].to<json::Number>(), 3.1 );
    M_ASSERT_EQ( nested_mixed[2][2][1].to<json::Number>(), 3.2 );
    M_ASSERT_EQ( nested_mixed[2][2][2].to<json::Number>(), 3.3 );
    
    // Test batch comparison of nested mixed arrays
    json::Value nested_mixed_copy{json::Array{
        json::Array{1, "one", true},
        json::Array{2.5, nullptr, false},  // simplified with implicit null conversion
        json::Array{"three", 3, json::Array{3.1, 3.2, 3.3}}  // simplified version
    }};
    M_ASSERT_TRUE( nested_mixed == nested_mixed_copy );
    
    // Test that sub-arrays can be compared
    M_ASSERT_TRUE( nested_mixed[0] == nested_mixed_copy[0] );
    M_ASSERT_TRUE( nested_mixed[1] == nested_mixed_copy[1] );
    M_ASSERT_TRUE( nested_mixed[2] == nested_mixed_copy[2] );
    M_ASSERT_TRUE( nested_mixed[2][2] == nested_mixed_copy[2][2] );
    
    // Test different sub-arrays are not equal
    M_ASSERT_FALSE( nested_mixed[0] == nested_mixed[1] );
    M_ASSERT_FALSE( nested_mixed[0] == nested_mixed[2] );
    M_ASSERT_FALSE( nested_mixed[1] == nested_mixed[2] );
    
    // Test serialization and parsing of mixed type arrays
    auto mixed_serialized = mixed_types_array.serialize();
    
    auto parsed_mixed_types = json::parse(mixed_serialized);
    if (parsed_mixed_types.has_value()) {
        M_ASSERT_TRUE( *parsed_mixed_types == mixed_types_array );
        M_ASSERT_EQ( (*parsed_mixed_types)[0].to<json::Number>(), 2 );
        M_ASSERT_EQ( (*parsed_mixed_types)[1].to<json::String>(), "12" );
        M_ASSERT_EQ( (*parsed_mixed_types)[2].to<json::Bool>(), false );
        M_ASSERT_EQ( (*parsed_mixed_types)[3].type(), json::Type::eNull );
        M_ASSERT_EQ( (*parsed_mixed_types)[4].to<json::Number>(), 124.231 );
    } else {
        M_ASSERT_FAIL("Failed to parse mixed type array");
    }
    
    // Test std::vector functionality on json::Array
    // Since json::Array is std::vector<json::Value>, test vector-specific operations
    
    // Test iterators
    json::Value iter_test{json::Array{1, 2, 3, 4, 5}};
    auto& iter_arr = iter_test.get<json::Array>();
    
    // Test range-based for loop
    int expected_value = 1;
    for (const auto& element : iter_arr) {
        M_ASSERT_EQ( element.to<json::Number>(), expected_value++ );
    }
    
    // Test iterator access
    auto iter_begin = iter_arr.begin();
    auto iter_end = iter_arr.end();
    M_ASSERT_EQ( std::distance(iter_begin, iter_end), 5 );
    
    // Test iterator dereferencing
    M_ASSERT_EQ( (*iter_begin).to<json::Number>(), 1 );
    M_ASSERT_EQ( (*(iter_begin + 1)).to<json::Number>(), 2 );
    M_ASSERT_EQ( (*(iter_end - 1)).to<json::Number>(), 5 );
    
    // Test iterator modification
    *iter_begin = 10;           // implicit conversion
    *(iter_begin + 1) = "two";  // implicit conversion
    M_ASSERT_EQ( iter_arr[0].to<json::Number>(), 10 );
    M_ASSERT_EQ( iter_arr[1].to<json::String>(), "two" );
    
    // Test const iterators
    const auto& const_iter_arr = iter_test.get<json::Array>();
    auto const_iter_begin = const_iter_arr.begin();
    auto const_iter_end = const_iter_arr.end();
    M_ASSERT_EQ( std::distance(const_iter_begin, const_iter_end), 5 );
    M_ASSERT_EQ( (*const_iter_begin).to<json::Number>(), 10 );
    
    // Test cbegin() and cend()
    auto cbegin_iter = iter_arr.cbegin();
    auto cend_iter = iter_arr.cend();
    M_ASSERT_EQ( std::distance(cbegin_iter, cend_iter), 5 );
    
    // Test reverse iterators
    auto rbegin_iter = iter_arr.rbegin();
    auto rend_iter = iter_arr.rend();
    M_ASSERT_EQ( std::distance(rbegin_iter, rend_iter), 5 );
    M_ASSERT_EQ( (*rbegin_iter).to<json::Number>(), 5 );  // last element
    M_ASSERT_EQ( (*(rbegin_iter + 1)).to<json::Number>(), 4 );  // second to last
    
    // Test push_back and pop_back
    json::Value push_pop_test{json::Array{1, 2, 3}};
    auto& push_pop_arr = push_pop_test.get<json::Array>();
    
    // Test push_back
    push_pop_arr.push_back(4);          // implicit conversion
    push_pop_arr.push_back("five");     // implicit conversion
    push_pop_arr.push_back(true);       // implicit conversion
    push_pop_arr.push_back(nullptr);    // implicit conversion
    
    M_ASSERT_EQ( push_pop_arr.size(), 7 );
    M_ASSERT_EQ( push_pop_arr[3].to<json::Number>(), 4 );
    M_ASSERT_EQ( push_pop_arr[4].to<json::String>(), "five" );
    M_ASSERT_EQ( push_pop_arr[5].to<json::Bool>(), true );
    M_ASSERT_EQ( push_pop_arr[6].type(), json::Type::eNull );
    
    // Test pop_back
    push_pop_arr.pop_back();  // remove null
    push_pop_arr.pop_back();  // remove true
    M_ASSERT_EQ( push_pop_arr.size(), 5 );
    M_ASSERT_EQ( push_pop_arr[4].to<json::String>(), "five" );
    
    // Test emplace_back
    push_pop_arr.emplace_back(json::Number(100));
    push_pop_arr.emplace_back(json::String("emplace"));
    M_ASSERT_EQ( push_pop_arr.size(), 7 );
    M_ASSERT_EQ( push_pop_arr[5].to<json::Number>(), 100 );
    M_ASSERT_EQ( push_pop_arr[6].to<json::String>(), "emplace" );
    
    // Test insert operations
    json::Value insert_test{json::Array{1, 2, 3}};
    auto& insert_arr = insert_test.get<json::Array>();
    
    // Test insert at beginning
    auto insert_iter = insert_arr.insert(insert_arr.begin(), 0);  // implicit conversion
    M_ASSERT_EQ( insert_arr.size(), 4 );
    M_ASSERT_EQ( insert_arr[0].to<json::Number>(), 0 );
    M_ASSERT_EQ( insert_arr[1].to<json::Number>(), 1 );
    M_ASSERT_EQ( (*insert_iter).to<json::Number>(), 0 );
    
    // Test insert in middle
    insert_iter = insert_arr.insert(insert_arr.begin() + 2, "middle");  // implicit conversion
    M_ASSERT_EQ( insert_arr.size(), 5 );
    M_ASSERT_EQ( insert_arr[0].to<json::Number>(), 0 );
    M_ASSERT_EQ( insert_arr[1].to<json::Number>(), 1 );
    M_ASSERT_EQ( insert_arr[2].to<json::String>(), "middle" );
    M_ASSERT_EQ( insert_arr[3].to<json::Number>(), 2 );
    M_ASSERT_EQ( insert_arr[4].to<json::Number>(), 3 );
    
    // Test insert at end
    insert_iter = insert_arr.insert(insert_arr.end(), true);  // implicit conversion
    M_ASSERT_EQ( insert_arr.size(), 6 );
    M_ASSERT_EQ( insert_arr[5].to<json::Bool>(), true );
    
    // Test insert multiple elements
    insert_iter = insert_arr.insert(insert_arr.begin() + 1, 2, json::String("repeated"));
    M_ASSERT_EQ( insert_arr.size(), 8 );
    M_ASSERT_EQ( insert_arr[1].to<json::String>(), "repeated" );
    M_ASSERT_EQ( insert_arr[2].to<json::String>(), "repeated" );
    M_ASSERT_EQ( insert_arr[3].to<json::Number>(), 1 );
    
    // Test insert with range
    json::Array source_arr{json::Number(100), json::Number(200), json::Number(300)};
    insert_iter = insert_arr.insert(insert_arr.end(), source_arr.begin(), source_arr.end());
    M_ASSERT_EQ( insert_arr.size(), 11 );
    M_ASSERT_EQ( insert_arr[8].to<json::Number>(), 100 );
    M_ASSERT_EQ( insert_arr[9].to<json::Number>(), 200 );
    M_ASSERT_EQ( insert_arr[10].to<json::Number>(), 300 );
    
    // Test insert with initializer list
    insert_iter = insert_arr.insert(insert_arr.begin(), {json::String("init1"), json::String("init2")});
    M_ASSERT_EQ( insert_arr.size(), 13 );
    M_ASSERT_EQ( insert_arr[0].to<json::String>(), "init1" );
    M_ASSERT_EQ( insert_arr[1].to<json::String>(), "init2" );
    
    // Test emplace
    json::Value emplace_test{json::Array{1, 2, 3}};
    auto& emplace_arr = emplace_test.get<json::Array>();
    
    auto emplace_iter = emplace_arr.emplace(emplace_arr.begin() + 1, json::String("emplaced"));
    M_ASSERT_EQ( emplace_arr.size(), 4 );
    M_ASSERT_EQ( emplace_arr[1].to<json::String>(), "emplaced" );
    M_ASSERT_EQ( (*emplace_iter).to<json::String>(), "emplaced" );
    
    // Test erase operations
    json::Value erase_test{json::Array{1, 2, 3, 4, 5, 6, 7, 8, 9, 10}};
    auto& erase_arr = erase_test.get<json::Array>();
    
    // Test erase single element
    auto erase_iter = erase_arr.erase(erase_arr.begin() + 2);  // erase 3
    M_ASSERT_EQ( erase_arr.size(), 9 );
    M_ASSERT_EQ( erase_arr[2].to<json::Number>(), 4 );  // 4 moved to position 2
    M_ASSERT_EQ( (*erase_iter).to<json::Number>(), 4 );
    
    // Test erase range
    erase_iter = erase_arr.erase(erase_arr.begin() + 1, erase_arr.begin() + 4);  // erase 2, 4, 5
    M_ASSERT_EQ( erase_arr.size(), 6 );
    M_ASSERT_EQ( erase_arr[0].to<json::Number>(), 1 );
    M_ASSERT_EQ( erase_arr[1].to<json::Number>(), 6 );  // 6 moved to position 1
    M_ASSERT_EQ( erase_arr[2].to<json::Number>(), 7 );
    
    // Test clear
    erase_arr.clear();
    M_ASSERT_EQ( erase_arr.size(), 0 );
    M_ASSERT_TRUE( erase_arr.empty() );
    
    // Test resize operations
    json::Value resize_test{json::Array{1, 2, 3}};
    auto& resize_arr = resize_test.get<json::Array>();
    
    // Test resize to larger size with default value
    resize_arr.resize(5);
    M_ASSERT_EQ( resize_arr.size(), 5 );
    M_ASSERT_EQ( resize_arr[0].to<json::Number>(), 1 );
    M_ASSERT_EQ( resize_arr[1].to<json::Number>(), 2 );
    M_ASSERT_EQ( resize_arr[2].to<json::Number>(), 3 );
    M_ASSERT_EQ( resize_arr[3].type(), json::Type::eNull );  // default constructed
    M_ASSERT_EQ( resize_arr[4].type(), json::Type::eNull );  // default constructed
    
    // Test resize to larger size with specific value
    resize_arr.resize(7, json::String("resized"));
    M_ASSERT_EQ( resize_arr.size(), 7 );
    M_ASSERT_EQ( resize_arr[5].to<json::String>(), "resized" );
    M_ASSERT_EQ( resize_arr[6].to<json::String>(), "resized" );
    
    // Test resize to smaller size
    resize_arr.resize(3);
    M_ASSERT_EQ( resize_arr.size(), 3 );
    M_ASSERT_EQ( resize_arr[0].to<json::Number>(), 1 );
    M_ASSERT_EQ( resize_arr[1].to<json::Number>(), 2 );
    M_ASSERT_EQ( resize_arr[2].to<json::Number>(), 3 );
    
    // Test reserve and capacity
    json::Value capacity_test{json::Array{}};
    auto& capacity_arr = capacity_test.get<json::Array>();
    
    capacity_arr.reserve(100);
    M_ASSERT_TRUE( capacity_arr.capacity() >= 100 );
    M_ASSERT_EQ( capacity_arr.size(), 0 );
    
    // Test shrink_to_fit
    capacity_arr.push_back(1);
    capacity_arr.push_back(2);
    capacity_arr.shrink_to_fit();
    M_ASSERT_EQ( capacity_arr.size(), 2 );
    M_ASSERT_TRUE( capacity_arr.capacity() >= 2 );
    
    // Test front and back
    json::Value front_back_test{json::Array{1, 2, 3, 4, 5}};
    auto& front_back_arr = front_back_test.get<json::Array>();
    
    M_ASSERT_EQ( front_back_arr.front().to<json::Number>(), 1 );
    M_ASSERT_EQ( front_back_arr.back().to<json::Number>(), 5 );
    
    // Test modifying through front and back
    front_back_arr.front() = "first";  // implicit conversion
    front_back_arr.back() = "last";    // implicit conversion
    M_ASSERT_EQ( front_back_arr.front().to<json::String>(), "first" );
    M_ASSERT_EQ( front_back_arr.back().to<json::String>(), "last" );
    
    // Test const front and back
    const auto& const_front_back_arr = front_back_test.get<json::Array>();
    M_ASSERT_EQ( const_front_back_arr.front().to<json::String>(), "first" );
    M_ASSERT_EQ( const_front_back_arr.back().to<json::String>(), "last" );
    
    // Test data() method
    json::Value data_test{json::Array{1, 2, 3}};
    auto& data_arr = data_test.get<json::Array>();
    
    auto* data_ptr = data_arr.data();
    M_ASSERT_EQ( data_ptr[0].to<json::Number>(), 1 );
    M_ASSERT_EQ( data_ptr[1].to<json::Number>(), 2 );
    M_ASSERT_EQ( data_ptr[2].to<json::Number>(), 3 );
    
    // Test modifying through data pointer
    data_ptr[0] = "modified";  // implicit conversion
    M_ASSERT_EQ( data_arr[0].to<json::String>(), "modified" );
    
    // Test assignment operations
    json::Value assign_test{json::Array{}};
    auto& assign_arr = assign_test.get<json::Array>();
    
    // Test assign with count and value
    assign_arr.assign(3, json::String("assigned"));
    M_ASSERT_EQ( assign_arr.size(), 3 );
    M_ASSERT_EQ( assign_arr[0].to<json::String>(), "assigned" );
    M_ASSERT_EQ( assign_arr[1].to<json::String>(), "assigned" );
    M_ASSERT_EQ( assign_arr[2].to<json::String>(), "assigned" );
    
    // Test assign with range
    json::Array source_assign{json::Number(100), json::Number(200), json::Number(300)};
    assign_arr.assign(source_assign.begin(), source_assign.end());
    M_ASSERT_EQ( assign_arr.size(), 3 );
    M_ASSERT_EQ( assign_arr[0].to<json::Number>(), 100 );
    M_ASSERT_EQ( assign_arr[1].to<json::Number>(), 200 );
    M_ASSERT_EQ( assign_arr[2].to<json::Number>(), 300 );
    
    // Test assign with initializer list
    assign_arr.assign({json::String("init1"), json::String("init2"), json::String("init3")});
    M_ASSERT_EQ( assign_arr.size(), 3 );
    M_ASSERT_EQ( assign_arr[0].to<json::String>(), "init1" );
    M_ASSERT_EQ( assign_arr[1].to<json::String>(), "init2" );
    M_ASSERT_EQ( assign_arr[2].to<json::String>(), "init3" );
    
    // Test swap
    json::Value swap_test1{json::Array{1, 2, 3}};
    json::Value swap_test2{json::Array{"a", "b", "c"}};
    auto& swap_arr1 = swap_test1.get<json::Array>();
    auto& swap_arr2 = swap_test2.get<json::Array>();
    
    swap_arr1.swap(swap_arr2);
    M_ASSERT_EQ( swap_arr1.size(), 3 );
    M_ASSERT_EQ( swap_arr1[0].to<json::String>(), "a" );
    M_ASSERT_EQ( swap_arr1[1].to<json::String>(), "b" );
    M_ASSERT_EQ( swap_arr1[2].to<json::String>(), "c" );
    
    M_ASSERT_EQ( swap_arr2.size(), 3 );
    M_ASSERT_EQ( swap_arr2[0].to<json::Number>(), 1 );
    M_ASSERT_EQ( swap_arr2[1].to<json::Number>(), 2 );
    M_ASSERT_EQ( swap_arr2[2].to<json::Number>(), 3 );
    
    // Test std::algorithms with iterators
    json::Value algo_test{json::Array{5, 2, 8, 1, 9, 3}};
    auto& algo_arr = algo_test.get<json::Array>();
    
    // Test std::find
    auto find_iter = std::find_if(algo_arr.begin(), algo_arr.end(), 
        [](const json::Value& val) { return val.to<json::Number>() == 8; });
    M_ASSERT_TRUE( find_iter != algo_arr.end() );
    M_ASSERT_EQ( (*find_iter).to<json::Number>(), 8 );
    
    // Test std::count
    algo_arr.push_back(2);  // add another 2
    auto count_2 = std::count_if(algo_arr.begin(), algo_arr.end(),
        [](const json::Value& val) { return val.to<json::Number>() == 2; });
    M_ASSERT_EQ( count_2, 2 );
    
    // Test std::sort (custom comparator needed for json::Value)
    std::sort(algo_arr.begin(), algo_arr.end(), 
        [](const json::Value& a, const json::Value& b) {
            return a.to<json::Number>() < b.to<json::Number>();
        });
    M_ASSERT_EQ( algo_arr[0].to<json::Number>(), 1 );
    M_ASSERT_EQ( algo_arr[1].to<json::Number>(), 2 );
    M_ASSERT_EQ( algo_arr[2].to<json::Number>(), 2 );
    M_ASSERT_EQ( algo_arr[3].to<json::Number>(), 3 );
    M_ASSERT_EQ( algo_arr[4].to<json::Number>(), 5 );
    M_ASSERT_EQ( algo_arr[5].to<json::Number>(), 8 );
    M_ASSERT_EQ( algo_arr[6].to<json::Number>(), 9 );
    
    // Test std::transform
    json::Value transform_test{json::Array{1, 2, 3, 4, 5}};
    auto& transform_arr = transform_test.get<json::Array>();
    
    std::transform(transform_arr.begin(), transform_arr.end(), transform_arr.begin(),
        [](const json::Value& val) -> json::Value {
            return json::Number(val.to<json::Number>() * 2);
        });
    M_ASSERT_EQ( transform_arr[0].to<json::Number>(), 2 );
    M_ASSERT_EQ( transform_arr[1].to<json::Number>(), 4 );
    M_ASSERT_EQ( transform_arr[2].to<json::Number>(), 6 );
    M_ASSERT_EQ( transform_arr[3].to<json::Number>(), 8 );
    M_ASSERT_EQ( transform_arr[4].to<json::Number>(), 10 );
    
    // Test serialization after vector operations
    json::Value vector_ops_test{json::Array{1, 2, 3}};
    auto& vector_ops_arr = vector_ops_test.get<json::Array>();
    
    vector_ops_arr.push_back("four");
    vector_ops_arr.insert(vector_ops_arr.begin() + 1, true);
    vector_ops_arr.emplace_back(nullptr);
    
    auto serialized_vector_ops = vector_ops_test.serialize();
    M_ASSERT_EQ( serialized_vector_ops, "[1,true,2,3,\"four\",null]" );
    
    // Test parsing and vector operations
    auto parsed_for_vector = json::parse("[10,20,30]");
    if (parsed_for_vector.has_value()) {
        auto& parsed_arr = parsed_for_vector->get<json::Array>();
        parsed_arr.push_back(40);
        parsed_arr.insert(parsed_arr.begin(), 0);
        M_ASSERT_EQ( parsed_arr.size(), 5 );
        M_ASSERT_EQ( parsed_arr[0].to<json::Number>(), 0 );
        M_ASSERT_EQ( parsed_arr[1].to<json::Number>(), 10 );
        M_ASSERT_EQ( parsed_arr[2].to<json::Number>(), 20 );
        M_ASSERT_EQ( parsed_arr[3].to<json::Number>(), 30 );
        M_ASSERT_EQ( parsed_arr[4].to<json::Number>(), 40 );
    } else {
        M_ASSERT_FAIL("Failed to parse array for vector operations");
    }
}
