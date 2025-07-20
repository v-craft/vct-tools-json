/**
 * @file json_macros.hpp
 * @brief Comprehensive JSON serialization and deserialization macro system for C++23
 * @details This header provides modern C++23 macros for automatic JSON conversion with:
 *          - Perfect forwarding and move semantics optimization
 *          - Compile-time type safety with static_assert
 *          - Support for custom field mapping
 *          - Automatic constructor generation from JSON
 *          - Exception-safe error handling
 * @author Mysvac
 * @version 0.2.0
 * @since C++23
 */


#ifndef _M_VCT_TOOLS_JSON_MACROS_HPP
#define _M_VCT_TOOLS_JSON_MACROS_HPP


#define M_VCT_TOOLS_JSON_CONVERSION_FIELD( member_name ) \
    do {    \
        static_assert( std::is_constructible_v< ::vct::tools::json::Value, decltype(this->member_name) >, "VCT_TOOLS_JSON: " #member_name " use macros CONVERSION_FILED, json::Value must be constructible from it. " );  \
        macro_result[ #member_name ] = ::vct::tools::json::Value{ _move_if_rvalue(this->member_name) };  \
    }while(false);
    



#define M_VCT_TOOLS_JSON_CONVERSION_MAP_FIELD( field_name, member_name ) \
    do {    \
        static_assert( std::is_constructible_v< ::vct::tools::json::Value, decltype(this->member_name) >, "VCT_TOOLS_JSON: " #member_name " use macros CONVERSION_FILED, json::Value must be constructible from it. " );  \
        macro_result[ #field_name ] = ::vct::tools::json::Value{ _move_if_rvalue(this->member_name) };  \
    }while(false);


#define M_VCT_TOOLS_JSON_CONVERSION_FUNCTION( class_name, ... )   \
    explicit operator ::vct::tools::json::Value() const & { \
        ::vct::tools::json::Value macro_result{::vct::tools::json::Type::eObject}; \
        auto _move_if_rvalue = [](const auto& val) -> const auto& { return val; }; \
        __VA_ARGS__ \
        return macro_result; \
    } \
    explicit operator ::vct::tools::json::Value() && { \
    ::vct::tools::json::Value macro_result{::vct::tools::json::Type::eObject}; \
    auto _move_if_rvalue = [](auto& val) -> auto&& { return std::move(val); }; \
    __VA_ARGS__ \
    return macro_result; \
    } \
    explicit operator ::vct::tools::json::Value() & { \
        ::vct::tools::json::Value macro_result{::vct::tools::json::Type::eObject}; \
        auto _move_if_rvalue = [](auto& val) -> auto& { return val; }; \
        __VA_ARGS__ \
        return macro_result; \
    } \





#define M_VCT_TOOLS_JSON_CONSTRUCTOR_FIELD_OR(member_name, default_result, default_range_value) \
    do{ \
        static_assert( std::is_convertible_v<decltype(default_result), std::remove_cvref_t<decltype(member_name)>>, "VCT_TOOLS_JSON: " #member_name " use macros FIELD_OR, default_result must be convertible to member_type. "  );  \
        this->member_name = ( \
            _json_value.contains( #member_name ) ? \
            _json_value[ #member_name ].template move_or<std::remove_cvref_t<decltype(member_name)>,std::remove_cvref_t<decltype(default_range_value)>>( default_result, default_range_value ) : \
            default_result \
        );  \
    }while(false);


#define M_VCT_TOOLS_JSON_CONSTRUCTOR_FIELD_DEFAULT(member_name) \
    do {    \
        static_assert( std::is_default_constructible_v<std::remove_cvref_t<decltype(member_name)>>, "VCT_TOOLS_JSON: " #member_name " use macros FIELD_DEFAULT, must is_default_constructible. "    );  \
        this->member_name = ( \
            _json_value.contains( #member_name ) ? \
            _json_value[ #member_name ].template move_or<std::remove_cvref_t<decltype(member_name)>>( std::remove_cvref_t<decltype(member_name)>{} ) : \
            std::remove_cvref_t<decltype(member_name)>{} \
        );  \
    }while(false);


#define M_VCT_TOOLS_JSON_CONSTRUCTOR_MAP_FIELD_OR(field_name, member_name, default_result, default_range_value) \
    do{ \
        static_assert( std::is_convertible_v<decltype(default_result), std::remove_cvref_t<decltype(member_name)>>, "VCT_TOOLS_JSON: " #member_name " use macros FIELD_OR, default_result must be convertible to member_type. " );  \
        this->member_name = ( \
            _json_value.contains( #field_name ) ? \
            _json_value[ #field_name ].template move_or<std::remove_cvref_t<decltype(member_name)>,std::remove_cvref_t<decltype(default_range_value)>>( default_result, default_range_value ) : \
            default_result \
        );  \
    }while(false);


#define M_VCT_TOOLS_JSON_CONSTRUCTOR_MAP_FIELD_DEFAULT(field_name, member_name) \
    do {    \
        static_assert( std::is_default_constructible_v<std::remove_cvref_t<decltype(member_name)>>, "VCT_TOOLS_JSON: " #member_name " use macros FIELD_DEFAULT, must is_default_constructible. "    );  \
        this->member_name = ( \
            _json_value.contains( #field_name ) ? \
            _json_value[ #field_name ].template move_or<std::remove_cvref_t<decltype(member_name)>>( std::remove_cvref_t<decltype(member_name)>{} ) : \
            std::remove_cvref_t<decltype(member_name)>{} \
        );  \
    }while(false);



#define M_VCT_TOOLS_JSON_CONSTRUCTOR_FUNCTION( class_name, ... ) \
    explicit class_name ( ::vct::tools::json::Value _json_value ) noexcept {  \
        __VA_ARGS__     \
    }


#ifdef M_VCT_JSON_SIMPLIFY_MACROS
    #define M_JSON_CV_MEM       M_VCT_TOOLS_JSON_CONVERSION_FIELD
    #define M_JSON_CV_MAP       M_VCT_TOOLS_JSON_CONVERSION_MAP_FIELD
    #define M_JSON_CV_FUN       M_VCT_TOOLS_JSON_CONVERSION_FUNCTION
    #define M_JSON_CS_MEM       M_VCT_TOOLS_JSON_CONSTRUCTOR_FIELD_DEFAULT
    #define M_JSON_CS_MAP       M_VCT_TOOLS_JSON_CONSTRUCTOR_MAP_FIELD_DEFAULT
    #define M_JSON_CS_MEM_OR    M_VCT_TOOLS_JSON_CONSTRUCTOR_FIELD_OR
    #define M_JSON_CS_MAP_OR    M_VCT_TOOLS_JSON_CONSTRUCTOR_MAP_FIELD_OR
    #define M_JSON_CS_FUN       M_VCT_TOOLS_JSON_CONSTRUCTOR_FUNCTION
#endif // M_VCT_JSON_SIMPLIFY_MACROS

#endif // _M_VCT_TOOLS_JSON_MACROS_HPP

