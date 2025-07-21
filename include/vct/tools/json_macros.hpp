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

/**
 * @defgroup JSON_CONVERSION_MACROS JSON Conversion Macros
 * @brief Macros for converting C++ objects to JSON Values
 * @details These macros provide automatic serialization of C++ class members to JSON format.
 *          They generate operator::vct::tools::json::Value() with perfect forwarding support.
 * @{
 */

/**
 * @def M_VCT_TOOLS_JSON_CONVERSION_FIELD(member_name)
 * @brief Convert a class member to JSON field with the same name
 * @param member_name The name of the class member to serialize
 *
 * @details This macro generates code to serialize a class member to a JSON object field.
 *          The JSON field name will be the same as the member name.
 *          Includes compile-time type checking to ensure the member type is JSON-convertible.
 *
 * @note Requirements:
 *       - The member type must be constructible to ::vct::tools::json::Value
 *       - Must be used within M_VCT_TOOLS_JSON_CONVERSION_FUNCTION
 *
 * @example
 * @code
 * class Person {
 *     std::string name;
 *     int age;
 *
 *     M_VCT_TOOLS_JSON_CONVERSION_FUNCTION(Person,
 *         M_VCT_TOOLS_JSON_CONVERSION_FIELD(name);
 *         M_VCT_TOOLS_JSON_CONVERSION_FIELD(age);
 *     )
 * };
 * @endcode
 */
#define M_VCT_TOOLS_JSON_CONVERSION_FIELD( member_name ) \
    do {    \
        static_assert( std::is_constructible_v< ::vct::tools::json::Value, decltype(this->member_name) >, "VCT_TOOLS_JSON: " #member_name " use macros CONVERSION_FILED, json::Value must be constructible from it. " );  \
        macro_result[ #member_name ] = ::vct::tools::json::Value{ _move_if_rvalue(this->member_name) };  \
    }while(false);
    
/**
 * @def M_VCT_TOOLS_JSON_CONVERSION_MAP_FIELD(field_name, member_name)
 * @brief Convert a class member to JSON field with custom field name
 * @param field_name The name to use for the JSON field (string literal)
 * @param member_name The name of the class member to serialize
 *
 * @details This macro generates code to serialize a class member to a JSON object field
 *          with a custom field name. Useful when JSON field names differ from C++ member names
 *          or when following specific JSON naming conventions.
 *
 * @note Requirements:
 *       - The member type must be constructible to ::vct::tools::json::Value
 *       - Must be used within M_VCT_TOOLS_JSON_CONVERSION_FUNCTION
 *
 * @example
 * @code
 * class Person {
 *     std::string firstName;
 *     std::string lastName;
 *
 *     M_VCT_TOOLS_JSON_CONVERSION_FUNCTION(Person,
 *         M_VCT_TOOLS_JSON_CONVERSION_MAP_FIELD("first_name", firstName);
 *         M_VCT_TOOLS_JSON_CONVERSION_MAP_FIELD("last_name", lastName);
 *     )
 * };
 * @endcode
 */
#define M_VCT_TOOLS_JSON_CONVERSION_MAP_FIELD( field_name, member_name ) \
    do {    \
        static_assert( std::is_constructible_v< ::vct::tools::json::Value, decltype(this->member_name) >, "VCT_TOOLS_JSON: " #member_name " use macros CONVERSION_FILED, json::Value must be constructible from it. " );  \
        macro_result[ #field_name ] = ::vct::tools::json::Value{ _move_if_rvalue(this->member_name) };  \
    }while(false);

/**
 * @def M_VCT_TOOLS_JSON_CONVERSION_FUNCTION(class_name, ...)
 * @brief Generate JSON conversion operator for a class
 * @param class_name The name of the class being converted
 * @param ... Variable arguments containing conversion field macros
 *
 * @details This macro generates three overloads of operator::vct::tools::json::Value():
 *          - const& version: for const lvalue references (copies values)
 *          - && version: for rvalue references (moves values for optimization)
 *          - & version: for non-const lvalue references (returns references)
 *
 * @note The macro creates a JSON object and populates it using the provided field macros.
 *       Perfect forwarding ensures optimal performance based on value category.
 *
 * @example
 * @code
 * class Config {
 *     std::string host;
 *     int port;
 *     bool ssl_enabled;
 *
 *     M_VCT_TOOLS_JSON_CONVERSION_FUNCTION(Config,
 *         M_VCT_TOOLS_JSON_CONVERSION_FIELD(host);
 *         M_VCT_TOOLS_JSON_CONVERSION_FIELD(port);
 *         M_VCT_TOOLS_JSON_CONVERSION_MAP_FIELD("ssl", ssl_enabled);
 *     )
 * };
 * @endcode
 */
#define M_VCT_TOOLS_JSON_CONVERSION_FUNCTION( class_name, ... )   \
    explicit operator ::vct::tools::json::Value() const & { \
        ::vct::tools::json::Value macro_result{ ::vct::tools::json::Object{} }; \
        auto _move_if_rvalue = [](const auto& val) -> const auto& { return val; }; \
        __VA_ARGS__ \
        return macro_result; \
    } \
    explicit operator ::vct::tools::json::Value() && { \
    ::vct::tools::json::Value macro_result{ ::vct::tools::json::Object{} }; \
    auto _move_if_rvalue = [](auto& val) -> auto&& { return std::move(val); }; \
    __VA_ARGS__ \
    return macro_result; \
    } \
    explicit operator ::vct::tools::json::Value() & { \
        ::vct::tools::json::Value macro_result{ ::vct::tools::json::Object{} }; \
        auto _move_if_rvalue = [](auto& val) -> auto& { return val; }; \
        __VA_ARGS__ \
        return macro_result; \
    } \

/** @} */ // end of JSON_CONVERSION_MACROS

/**
 * @defgroup JSON_CONSTRUCTOR_MACROS JSON Constructor Macros
 * @brief Macros for constructing C++ objects from JSON Values
 * @details These macros provide automatic deserialization from JSON to C++ class instances.
 *          They generate constructors that accept ::vct::tools::json::Value parameters.
 * @{
 */

/**
 * @def M_VCT_TOOLS_JSON_CONSTRUCTOR_FIELD_OR(member_name, default_result, default_range_value)
 * @brief Deserialize JSON field to class member with fallback default value
 * @param member_name The class member to initialize
 * @param default_result The default value to use if JSON field is missing or conversion fails
 * @param default_range_value Default value for range-type conversions (arrays/objects)
 *
 * @details This macro attempts to deserialize a JSON field to a class member.
 *          If the field doesn't exist or conversion fails, it uses the provided default value.
 *          Supports complex types with range conversion fallbacks.
 *
 * @note Requirements:
 *       - default_result must be convertible to the member type
 *       - Must be used within M_VCT_TOOLS_JSON_CONSTRUCTOR_FUNCTION
 *
 * @example
 * @code
 * class ServerConfig {
 *     std::string host;
 *     int port;
 *     std::vector<std::string> allowed_ips;
 *
 *     M_VCT_TOOLS_JSON_CONSTRUCTOR_FUNCTION(ServerConfig,
 *         M_VCT_TOOLS_JSON_CONSTRUCTOR_FIELD_OR(host, "localhost", "");
 *         M_VCT_TOOLS_JSON_CONSTRUCTOR_FIELD_OR(port, 8080, 0);
 *         M_VCT_TOOLS_JSON_CONSTRUCTOR_FIELD_OR(allowed_ips, std::vector<std::string>{}, std::string{});
 *     )
 * };
 * @endcode
 */
#define M_VCT_TOOLS_JSON_CONSTRUCTOR_FIELD_OR(member_name, default_result, default_range_value) \
    do{ \
        static_assert( std::is_convertible_v<decltype(default_result), std::remove_cvref_t<decltype(member_name)>>, "VCT_TOOLS_JSON: " #member_name " use macros FIELD_OR, default_result must be convertible to member_type. "  );  \
        this->member_name = ( \
            _json_value.contains( #member_name ) ? \
            _json_value[ #member_name ].template move_or<std::remove_cvref_t<decltype(member_name)>,std::remove_cvref_t<decltype(default_range_value)>>( default_result, default_range_value ) : \
            default_result \
        );  \
    }while(false);

/**
 * @def M_VCT_TOOLS_JSON_CONSTRUCTOR_FIELD_DEFAULT(member_name)
 * @brief Deserialize JSON field to class member with default-constructed fallback
 * @param member_name The class member to initialize
 *
 * @details This macro attempts to deserialize a JSON field to a class member.
 *          If the field doesn't exist or conversion fails, it default-constructs the member.
 *          More convenient than FIELD_OR when default construction is sufficient.
 *
 * @note Requirements:
 *       - The member type must be default constructible
 *       - Must be used within M_VCT_TOOLS_JSON_CONSTRUCTOR_FUNCTION
 *
 * @example
 * @code
 * class User {
 *     std::string username;
 *     std::string email;
 *     int login_count;
 *
 *     M_VCT_TOOLS_JSON_CONSTRUCTOR_FUNCTION(User,
 *         M_VCT_TOOLS_JSON_CONSTRUCTOR_FIELD_DEFAULT(username);
 *         M_VCT_TOOLS_JSON_CONSTRUCTOR_FIELD_DEFAULT(email);
 *         M_VCT_TOOLS_JSON_CONSTRUCTOR_FIELD_DEFAULT(login_count);
 *     )
 * };
 * @endcode
 */
#define M_VCT_TOOLS_JSON_CONSTRUCTOR_FIELD_DEFAULT(member_name) \
    do {    \
        static_assert( std::is_default_constructible_v<std::remove_cvref_t<decltype(member_name)>>, "VCT_TOOLS_JSON: " #member_name " use macros FIELD_DEFAULT, must is_default_constructible. "    );  \
        this->member_name = ( \
            _json_value.contains( #member_name ) ? \
            _json_value[ #member_name ].template move_or<std::remove_cvref_t<decltype(member_name)>>( std::remove_cvref_t<decltype(member_name)>{} ) : \
            std::remove_cvref_t<decltype(member_name)>{} \
        );  \
    }while(false);

/**
 * @def M_VCT_TOOLS_JSON_CONSTRUCTOR_MAP_FIELD_OR(field_name, member_name, default_result, default_range_value)
 * @brief Deserialize custom-named JSON field to class member with fallback default value
 * @param field_name The JSON field name to look for (string literal)
 * @param member_name The class member to initialize
 * @param default_result The default value to use if JSON field is missing or conversion fails
 * @param default_range_value Default value for range-type conversions (arrays/objects)
 *
 * @details Similar to CONSTRUCTOR_FIELD_OR but allows mapping between different JSON field names
 *          and C++ member names. Useful for adapting to external API naming conventions.
 *
 * @note Requirements:
 *       - default_result must be convertible to the member type
 *       - Must be used within M_VCT_TOOLS_JSON_CONSTRUCTOR_FUNCTION
 *
 * @example
 * @code
 * class Person {
 *     std::string firstName;
 *     std::string lastName;
 *     int age;
 *
 *     M_VCT_TOOLS_JSON_CONSTRUCTOR_FUNCTION(Person,
 *         M_VCT_TOOLS_JSON_CONSTRUCTOR_MAP_FIELD_OR("first_name", firstName, "", "");
 *         M_VCT_TOOLS_JSON_CONSTRUCTOR_MAP_FIELD_OR("last_name", lastName, "", "");
 *         M_VCT_TOOLS_JSON_CONSTRUCTOR_MAP_FIELD_OR("age", age, 0, 0);
 *     )
 * };
 * @endcode
 */
#define M_VCT_TOOLS_JSON_CONSTRUCTOR_MAP_FIELD_OR(field_name, member_name, default_result, default_range_value) \
    do{ \
        static_assert( std::is_convertible_v<decltype(default_result), std::remove_cvref_t<decltype(member_name)>>, "VCT_TOOLS_JSON: " #member_name " use macros FIELD_OR, default_result must be convertible to member_type. " );  \
        this->member_name = ( \
            _json_value.contains( #field_name ) ? \
            _json_value[ #field_name ].template move_or<std::remove_cvref_t<decltype(member_name)>,std::remove_cvref_t<decltype(default_range_value)>>( default_result, default_range_value ) : \
            default_result \
        );  \
    }while(false);

/**
 * @def M_VCT_TOOLS_JSON_CONSTRUCTOR_MAP_FIELD_DEFAULT(field_name, member_name)
 * @brief Deserialize custom-named JSON field to class member with default-constructed fallback
 * @param field_name The JSON field name to look for (string literal)
 * @param member_name The class member to initialize
 *
 * @details Similar to CONSTRUCTOR_FIELD_DEFAULT but allows mapping between different JSON field names
 *          and C++ member names. Uses default construction as fallback when field is missing.
 *
 * @note Requirements:
 *       - The member type must be default constructible
 *       - Must be used within M_VCT_TOOLS_JSON_CONSTRUCTOR_FUNCTION
 *
 * @example
 * @code
 * class APIResponse {
 *     int statusCode;
 *     std::string message;
 *     std::string errorDetails;
 *
 *     M_VCT_TOOLS_JSON_CONSTRUCTOR_FUNCTION(APIResponse,
 *         M_VCT_TOOLS_JSON_CONSTRUCTOR_MAP_FIELD_DEFAULT("status_code", statusCode);
 *         M_VCT_TOOLS_JSON_CONSTRUCTOR_MAP_FIELD_DEFAULT("msg", message);
 *         M_VCT_TOOLS_JSON_CONSTRUCTOR_MAP_FIELD_DEFAULT("error", errorDetails);
 *     )
 * };
 * @endcode
 */
#define M_VCT_TOOLS_JSON_CONSTRUCTOR_MAP_FIELD_DEFAULT(field_name, member_name) \
    do {    \
        static_assert( std::is_default_constructible_v<std::remove_cvref_t<decltype(member_name)>>, "VCT_TOOLS_JSON: " #member_name " use macros FIELD_DEFAULT, must is_default_constructible. "    );  \
        this->member_name = ( \
            _json_value.contains( #field_name ) ? \
            _json_value[ #field_name ].template move_or<std::remove_cvref_t<decltype(member_name)>>( std::remove_cvref_t<decltype(member_name)>{} ) : \
            std::remove_cvref_t<decltype(member_name)>{} \
        );  \
    }while(false);

/**
 * @def M_VCT_TOOLS_JSON_CONSTRUCTOR_FUNCTION(class_name, ...)
 * @brief Generate JSON constructor for a class
 * @param class_name The name of the class being constructed
 * @param ... Variable arguments containing constructor field macros
 *
 * @details This macro generates an explicit constructor that takes a ::vct::tools::json::Value
 *          and initializes class members based on the JSON content. The constructor is noexcept
 *          and uses move semantics for optimal performance.
 *
 * @note The constructor parameter name is _json_value and is available within the macro body.
 *       All field initialization macros should be used within this macro's body.
 *
 * @example
 * @code
 * class DatabaseConfig {
 *     std::string connection_string;
 *     int max_connections;
 *     bool use_ssl;
 *     std::vector<std::string> trusted_hosts;
 *
 *     M_VCT_TOOLS_JSON_CONSTRUCTOR_FUNCTION(DatabaseConfig,
 *         M_VCT_TOOLS_JSON_CONSTRUCTOR_FIELD_OR(connection_string, "localhost:5432", "");
 *         M_VCT_TOOLS_JSON_CONSTRUCTOR_FIELD_OR(max_connections, 10, 0);
 *         M_VCT_TOOLS_JSON_CONSTRUCTOR_FIELD_DEFAULT(use_ssl);
 *         M_VCT_TOOLS_JSON_CONSTRUCTOR_FIELD_DEFAULT(trusted_hosts);
 *     )
 * };
 * @endcode
 */
#define M_VCT_TOOLS_JSON_CONSTRUCTOR_FUNCTION( class_name, ... ) \
    explicit class_name ( ::vct::tools::json::Value _json_value ) noexcept {  \
        __VA_ARGS__     \
    }

/** @} */ // end of JSON_CONSTRUCTOR_MACROS

/**
 * @defgroup JSON_SIMPLIFIED_MACROS Simplified JSON Macros
 * @brief Shortened aliases for JSON macros when M_VCT_JSON_SIMPLIFY_MACROS is defined
 * @details These macros provide shorter, more convenient names for the full macro set.
 *          Enable by defining M_VCT_JSON_SIMPLIFY_MACROS before including this header.
 * @{
 */

#ifdef M_VCT_TOOLS_JSON_SIMPLIFY_MACROS
    /**
     * @def M_JSON_CV_MEM
     * @brief Simplified alias for M_VCT_TOOLS_JSON_CONVERSION_FIELD
     * @see M_VCT_TOOLS_JSON_CONVERSION_FIELD
     */
    #define M_JSON_CV_MEM       M_VCT_TOOLS_JSON_CONVERSION_FIELD

    /**
     * @def M_JSON_CV_MAP
     * @brief Simplified alias for M_VCT_TOOLS_JSON_CONVERSION_MAP_FIELD
     * @see M_VCT_TOOLS_JSON_CONVERSION_MAP_FIELD
     */
    #define M_JSON_CV_MAP       M_VCT_TOOLS_JSON_CONVERSION_MAP_FIELD

    /**
     * @def M_JSON_CV_FUN
     * @brief Simplified alias for M_VCT_TOOLS_JSON_CONVERSION_FUNCTION
     * @see M_VCT_TOOLS_JSON_CONVERSION_FUNCTION
     */
    #define M_JSON_CV_FUN       M_VCT_TOOLS_JSON_CONVERSION_FUNCTION

    /**
     * @def M_JSON_CS_MEM
     * @brief Simplified alias for M_VCT_TOOLS_JSON_CONSTRUCTOR_FIELD_DEFAULT
     * @see M_VCT_TOOLS_JSON_CONSTRUCTOR_FIELD_DEFAULT
     */
    #define M_JSON_CS_MEM       M_VCT_TOOLS_JSON_CONSTRUCTOR_FIELD_DEFAULT

    /**
     * @def M_JSON_CS_MAP
     * @brief Simplified alias for M_VCT_TOOLS_JSON_CONSTRUCTOR_MAP_FIELD_DEFAULT
     * @see M_VCT_TOOLS_JSON_CONSTRUCTOR_MAP_FIELD_DEFAULT
     */
    #define M_JSON_CS_MAP       M_VCT_TOOLS_JSON_CONSTRUCTOR_MAP_FIELD_DEFAULT

    /**
     * @def M_JSON_CS_MEM_OR
     * @brief Simplified alias for M_VCT_TOOLS_JSON_CONSTRUCTOR_FIELD_OR
     * @see M_VCT_TOOLS_JSON_CONSTRUCTOR_FIELD_OR
     */
    #define M_JSON_CS_MEM_OR    M_VCT_TOOLS_JSON_CONSTRUCTOR_FIELD_OR

    /**
     * @def M_JSON_CS_MAP_OR
     * @brief Simplified alias for M_VCT_TOOLS_JSON_CONSTRUCTOR_MAP_FIELD_OR
     * @see M_VCT_TOOLS_JSON_CONSTRUCTOR_MAP_FIELD_OR
     */
    #define M_JSON_CS_MAP_OR    M_VCT_TOOLS_JSON_CONSTRUCTOR_MAP_FIELD_OR

    /**
     * @def M_JSON_CS_FUN
     * @brief Simplified alias for M_VCT_TOOLS_JSON_CONSTRUCTOR_FUNCTION
     * @see M_VCT_TOOLS_JSON_CONSTRUCTOR_FUNCTION
     */
    #define M_JSON_CS_FUN       M_VCT_TOOLS_JSON_CONSTRUCTOR_FUNCTION
#endif // M_VCT_TOOLS_JSON_SIMPLIFY_MACROS

/** @} */ // end of JSON_SIMPLIFIED_MACROS

#endif // _M_VCT_TOOLS_JSON_MACROS_HPP
