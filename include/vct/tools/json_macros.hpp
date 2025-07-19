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
 * 
 * @section Usage Basic Usage
 * @code
 * #define M_VCT_JSON_SIMPLIFY_MACROS  // Optional: enables shorter macro names
 * #include <vct/tools/json_macros.hpp>
 * import vct.tools.json;
 * 
 * class Person {
 *     std::string name;
 *     int age;
 *     std::vector<std::string> hobbies;
 * 
 * public:
 *     // Bidirectional JSON conversion
 *     M_JSON_CV_FUN(Person,                    // Class → JSON
 *         M_JSON_CV_MEM(name),
 *         M_JSON_CV_MEM(age), 
 *         M_JSON_CV_MEM(hobbies)
 *     )
 *     
 *     M_JSON_CS_FUN(Person,                    // JSON → Class
 *         M_JSON_CS_MEM_OR(name, "Unknown"),
 *         M_JSON_CS_MEM_OR(age, 0),
 *         M_JSON_CS_MEM(hobbies)
 *     )
 * };
 * 
 * // Usage:
 * Person p{"Alice", 25, {"reading", "coding"}};
 * json::Value j{ p };                            // Automatic conversion to JSON
 * Person p2{ j };                                // Automatic construction from JSON
 * @endcode
 * 
 * @section Performance Performance Characteristics
 * - **Move Semantics**: Automatically uses std::move() for rvalue conversions
 * - **Perfect Forwarding**: Chooses optimal copy/move strategy based on value category
 * - **Zero Runtime Overhead**: All validation happens at compile-time
 * - **Memory Safe**: Uses RAII and exception-safe patterns
 * 
 * @section Features Key Features
 * - **Type Safety**: Compile-time validation of convertible types
 * - **Custom Mapping**: Support for different JSON field names vs class member names  
 * - **Default Values**: Graceful handling of missing JSON fields
 * - **Modern C++**: Uses C++20 concepts, templates, and language features
 * - **Simplified API**: Optional short macro names for cleaner code
 */


#ifndef _M_VCT_TOOLS_JSON_MACROS_HPP
#define _M_VCT_TOOLS_JSON_MACROS_HPP

/**
 * @brief Macro for converting a class member to JSON with automatic move semantics
 * @param member_name The name of the member variable to convert
 * @details This macro automatically converts a class member to JSON format.
 *          It uses perfect forwarding to optimize performance:
 *          - In const& context: copies the member value
 *          - In & context: copies the member value  
 *          - In && context: moves the member value for better performance
 * @note Uses compile-time type checking to ensure the member is convertible to json::Value
 * @note Can be aliased as M_JSON_CV_MEM when M_VCT_JSON_SIMPLIFY_MACROS is defined
 * @example
 * @code
 * class Person {
 *     std::string name;
 *     int age;
 *     M_VCT_TOOLS_JSON_CONVERSION_FUNCTION(Person,
 *         M_VCT_TOOLS_JSON_CONVERSION_FIELD(name),
 *         M_VCT_TOOLS_JSON_CONVERSION_FIELD(age)
 *     )
 * };
 * @endcode
 */
#define M_VCT_TOOLS_JSON_CONVERSION_FIELD( member_name ) \
    do {    \
        static_assert( std::is_convertible_v< decltype(this->member_name), ::vct::tools::json::Value >, "VCT_TOOLS_JSON: " #member_name " use macros CONVERSION_FILED, must is_convertible to json::Value. " );  \
        macro_result[ #member_name ] = ::vct::tools::json::Value{ _move_if_rvalue(this->member_name) };  \
    }while(false)
    


/**
 * @brief Macro for converting a class member to JSON with custom field naming
 * @param field_name The name to use in the JSON output
 * @param member_name The actual name of the member variable in the class
 * @details This macro allows you to map class members to different JSON field names.
 *          Like M_VCT_TOOLS_JSON_CONVERSION_FIELD, it uses perfect forwarding for optimal performance.
 * @note Uses compile-time type checking to ensure the member is convertible to json::Value
 * @note Can be aliased as M_JSON_CV_MAP when M_VCT_JSON_SIMPLIFY_MACROS is defined
 * @example
 * @code
 * class User {
 *     std::string m_user_name;  // Private naming convention
 *     int m_user_id;
 *     M_VCT_TOOLS_JSON_CONVERSION_FUNCTION(User,
 *         M_VCT_TOOLS_JSON_CONVERSION_MAP_FIELD(username, m_user_name),
 *         M_VCT_TOOLS_JSON_CONVERSION_MAP_FIELD(id, m_user_id)
 *     )
 * };
 * // Results in JSON: {"username": "...", "id": 123}
 * @endcode
 */
#define M_VCT_TOOLS_JSON_CONVERSION_MAP_FIELD( field_name, member_name ) \
    do {    \
        static_assert( std::is_convertible_v< decltype(this->member_name), ::vct::tools::json::Value>, "VCT_TOOLS_JSON: " #member_name " use macros CONVERSION_FILED, must is_convertible to json::Value. " );  \
        macro_result[ #field_name ] = ::vct::tools::json::Value{ _move_if_rvalue(this->member_name) };  \
    }while(false)

/**
 * @brief Macro to generate JSON conversion operators with perfect forwarding
 * @param class_name The name of the class to convert (currently unused but kept for consistency)
 * @param ... The body of the conversion function using field conversion macros
 * @details This macro generates three conversion operator overloads with optimal performance:
 *          - `operator json::Value() const &`: For const lvalue objects (copies members)
 *          - `operator json::Value() &`: For non-const lvalue objects (copies members)
 *          - `operator json::Value() &&`: For rvalue objects (moves members for performance)
 *          
 *          The macro uses lambda-based perfect forwarding to automatically choose the optimal
 *          semantics based on the object's value category.
 * @note Can be aliased as M_JSON_CV_FUN when M_VCT_JSON_SIMPLIFY_MACROS is defined
 * @note Typically used with M_VCT_TOOLS_JSON_CONVERSION_FIELD and M_VCT_TOOLS_JSON_CONVERSION_MAP_FIELD
 * @example
 * @code
 * #include <vct/tools/json_macros.hpp>
 * import vct.tools.json;
 * 
 * class Product {
 *     std::string name;
 *     double price;
 *     std::vector<std::string> tags;
 *     
 * public:
 *     M_VCT_TOOLS_JSON_CONVERSION_FUNCTION(Product,
 *         M_VCT_TOOLS_JSON_CONVERSION_FIELD(name),
 *         M_VCT_TOOLS_JSON_CONVERSION_FIELD(price),
 *         M_VCT_TOOLS_JSON_CONVERSION_FIELD(tags)
 *     )
 * };
 * 
 * // Usage examples:
 * Product p{"Laptop", 999.99, {"electronics", "computer"}};
 * json::Value j1 = p;                    // Copies all members
 * json::Value j2 = std::move(p);         // Moves all members (more efficient)
 * json::Value j3 = Product{"Phone", 599.99, {"mobile"}}; // Direct move from temporary
 * @endcode
 * @performance The && overload provides significant performance benefits for classes with
 *              expensive-to-copy members like std::string, std::vector, etc.
 */
#define M_VCT_TOOLS_JSON_CONVERSION_FUNCTION( class_name, ... )   \
    explicit operator ::vct::tools::json::Value() const & { \
        ::vct::tools::json::Value macro_result{::vct::tools::json::Type::eObject}; \
        auto _move_if_rvalue = [](const auto& val) -> const auto& { return val; }; \
        __VA_ARGS__; \
        return macro_result; \
    } \
    explicit operator ::vct::tools::json::Value() && { \
    ::vct::tools::json::Value macro_result{::vct::tools::json::Type::eObject}; \
    auto _move_if_rvalue = [](auto& val) -> auto&& { return std::move(val); }; \
    __VA_ARGS__; \
    return macro_result; \
    } \
    explicit operator ::vct::tools::json::Value() & { \
        ::vct::tools::json::Value macro_result{::vct::tools::json::Type::eObject}; \
        auto _move_if_rvalue = [](auto& val) -> auto& { return val; }; \
        __VA_ARGS__; \
        return macro_result; \
    } \





/**
 * @brief Macro for deserializing a JSON field to a class member with a custom default value
 * @param member_name The name of the class member to initialize
 * @param default_value The default value to use if the JSON field is missing
 * @details This macro extracts a value from JSON and assigns it to a class member.
 *          If the JSON field is missing, it uses the provided default value.
 *          Uses move semantics when possible for optimal performance.
 * @note Compile-time validates that the default value is convertible to the member type
 * @note Can be aliased as M_JSON_CS_MEM_OR when M_VCT_JSON_SIMPLIFY_MACROS is defined
 * @example
 * @code
 * class Config {
 *     int timeout;
 *     std::string host;
 *     
 *     M_VCT_TOOLS_JSON_CONSTRUCTOR_FUNCTION(Config,
 *         M_VCT_TOOLS_JSON_CONSTRUCTOR_FIELD_OR(timeout, 5000),
 *         M_VCT_TOOLS_JSON_CONSTRUCTOR_FIELD_OR(host, "localhost")
 *     )
 * };
 * @endcode
 */
#define M_VCT_TOOLS_JSON_CONSTRUCTOR_FIELD_OR(member_name, default_value) \
    do{ \
        static_assert( std::is_convertible_v<decltype(default_value), std::remove_cvref_t<decltype(member_name)>>, "VCT_TOOLS_JSON: " #member_name " use macros FIELD_DEFAULT, must is_default_constructible. "    );  \
        this->member_name = ( \
            _json_value.contains( #member_name ) ? \
            _json_value[ #member_name ].template move_or<std::remove_cvref_t<decltype(member_name)>>( default_value ) : \
            default_value \
        );  \
    }while(false)

/**
 * @brief Macro for deserializing a JSON field to a class member with default initialization
 * @param member_name The name of the class member to initialize
 * @details This macro extracts a value from JSON and assigns it to a class member.
 *          If the JSON field is missing, it uses default construction for the member type.
 *          Uses move semantics when possible for optimal performance.
 * @note Compile-time validates that the member type is default constructible
 * @note Can be aliased as M_JSON_CS_MEM when M_VCT_JSON_SIMPLIFY_MACROS is defined
 * @example
 * @code
 * class Person {
 *     std::string name;     // Will be "" if missing from JSON
 *     int age;              // Will be 0 if missing from JSON
 *     std::vector<std::string> hobbies; // Will be empty vector if missing
 *     
 *     M_VCT_TOOLS_JSON_CONSTRUCTOR_FUNCTION(Person,
 *         M_VCT_TOOLS_JSON_CONSTRUCTOR_FIELD_DEFAULT(name),
 *         M_VCT_TOOLS_JSON_CONSTRUCTOR_FIELD_DEFAULT(age),
 *         M_VCT_TOOLS_JSON_CONSTRUCTOR_FIELD_DEFAULT(hobbies)
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
    }while(false)

/**
 * @brief Macro for deserializing a JSON field with custom naming to a class member with default value
 * @param field_name The name of the field in the JSON object
 * @param member_name The name of the class member to initialize
 * @param default_value The default value to use if the JSON field is missing
 * @details This macro allows mapping JSON fields with different names to class members,
 *          similar to M_VCT_TOOLS_JSON_CONSTRUCTOR_FIELD_OR but with custom field naming.
 * @note Compile-time validates that the default value is convertible to the member type
 * @note Can be aliased as M_JSON_CS_MAP_OR when M_VCT_JSON_SIMPLIFY_MACROS is defined
 * @example
 * @code
 * class UserProfile {
 *     std::string m_full_name;
 *     int m_user_id;
 *     
 *     M_VCT_TOOLS_JSON_CONSTRUCTOR_FUNCTION(UserProfile,
 *         M_VCT_TOOLS_JSON_CONSTRUCTOR_MAP_FIELD_OR(fullName, m_full_name, "Anonymous"),
 *         M_VCT_TOOLS_JSON_CONSTRUCTOR_MAP_FIELD_OR(userId, m_user_id, -1)
 *     )
 * };
 * // Expects JSON: {"fullName": "John Doe", "userId": 123}
 * @endcode
 */
#define M_VCT_TOOLS_JSON_CONSTRUCTOR_MAP_FIELD_OR(field_name, member_name, default_value) \
    do{ \
        static_assert( std::is_convertible_v<decltype(default_value), std::remove_cvref_t<decltype(member_name)>>, "VCT_TOOLS_JSON: " #member_name " use macros FIELD_DEFAULT, must is_default_constructible. "    );  \
        this->member_name = ( \
            _json_value.contains( #field_name ) ? \
            _json_value[ #field_name ].template move_or<std::remove_cvref_t<decltype(member_name)>>( default_value ) : \
            default_value \
        );  \
    }while(false)

/**
 * @brief Macro for deserializing a JSON field with custom naming to a class member with default initialization
 * @param field_name The name of the field in the JSON object
 * @param member_name The name of the class member to initialize
 * @details This macro allows mapping JSON fields with different names to class members,
 *          using default construction if the field is missing from JSON.
 * @note Compile-time validates that the member type is default constructible
 * @note Can be aliased as M_JSON_CS_MAP when M_VCT_JSON_SIMPLIFY_MACROS is defined
 * @example
 * @code
 * class Settings {
 *     bool m_enable_feature;    // Will be false if "enableFeature" missing from JSON
 *     int m_max_connections;    // Will be 0 if "maxConnections" missing from JSON
 *     
 *     M_VCT_TOOLS_JSON_CONSTRUCTOR_FUNCTION(Settings,
 *         M_VCT_TOOLS_JSON_CONSTRUCTOR_MAP_FIELD_DEFAULT(enableFeature, m_enable_feature),
 *         M_VCT_TOOLS_JSON_CONSTRUCTOR_MAP_FIELD_DEFAULT(maxConnections, m_max_connections)
 *     )
 * };
 * // Expects JSON: {"enableFeature": true, "maxConnections": 100}
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
    }while(false)


/**
 * @brief Macro to generate JSON constructor and assignment operator
 * @param class_name The name of the class (currently unused but kept for consistency)
 * @param ... The body of the constructor using field initialization macros
 * @details This macro generates two functions for JSON deserialization:
 *          1. `explicit class_name(json::Value)`: Constructor from JSON
 *          2. `class_name& operator=(json::Value)`: Assignment operator from JSON
 *          
 *          Both functions are marked noexcept and provide safe JSON deserialization
 *          with compile-time type checking and default value handling.
 * @note Can be aliased as M_JSON_CS_FUN when M_VCT_JSON_SIMPLIFY_MACROS is defined
 * @note Typically used with field initialization macros like M_VCT_TOOLS_JSON_CONSTRUCTOR_FIELD_DEFAULT
 * @example
 * @code
 * #include <vct/tools/json_macros.hpp>
 * import vct.tools.json;
 * 
 * class DatabaseConfig {
 *     std::string host;
 *     int port;
 *     std::string database;
 *     bool ssl_enabled;
 *     
 * public:
 *     M_VCT_TOOLS_JSON_CONSTRUCTOR_FUNCTION(DatabaseConfig,
 *         M_VCT_TOOLS_JSON_CONSTRUCTOR_FIELD_OR(host, "localhost"),
 *         M_VCT_TOOLS_JSON_CONSTRUCTOR_FIELD_OR(port, 5432),
 *         M_VCT_TOOLS_JSON_CONSTRUCTOR_FIELD_DEFAULT(database),
 *         M_VCT_TOOLS_JSON_CONSTRUCTOR_FIELD_OR(ssl_enabled, true)
 *     )
 * };
 * 
 * // Usage examples:
 * json::Value config_json = json::parse(R"({"host": "db.example.com", "port": 3306, "database": "myapp"})");
 * DatabaseConfig config1(config_json);           // Constructor
 * DatabaseConfig config2;
 * config2 = config_json;                         // Assignment operator
 * @endcode
 * @safety Both generated functions are noexcept and handle missing JSON fields gracefully
 *         using default values or default construction as specified in the field macros.
 */
#define M_VCT_TOOLS_JSON_CONSTRUCTOR_FUNCTION( class_name, ... ) \
    explicit class_name ( ::vct::tools::json::Value _json_value ) noexcept {  \
        __VA_ARGS__;     \
    }   \
    class_name& operator=( ::vct::tools::json::Value _json_value ) noexcept { \
        __VA_ARGS__; \
        return *this; \
    }




/**
 * @brief Simplified macro aliases for easier usage
 * @details When M_VCT_JSON_SIMPLIFY_MACROS is defined, these shorter aliases are available:
 * 
 * **Conversion Macros (Class → JSON):**
 * - `M_JSON_CV_MEM`: Alias for M_VCT_TOOLS_JSON_CONVERSION_FIELD
 * - `M_JSON_CV_MAP`: Alias for M_VCT_TOOLS_JSON_CONVERSION_MAP_FIELD
 * - `M_JSON_CV_FUN`: Alias for M_VCT_TOOLS_JSON_CONVERSION_FUNCTION
 * 
 * **Constructor Macros (JSON → Class):**
 * - `M_JSON_CS_MEM`: Alias for M_VCT_TOOLS_JSON_CONSTRUCTOR_FIELD_DEFAULT
 * - `M_JSON_CS_MAP`: Alias for M_VCT_TOOLS_JSON_CONSTRUCTOR_MAP_FIELD_DEFAULT
 * - `M_JSON_CS_MEM_OR`: Alias for M_VCT_TOOLS_JSON_CONSTRUCTOR_FIELD_OR
 * - `M_JSON_CS_MAP_OR`: Alias for M_VCT_TOOLS_JSON_CONSTRUCTOR_MAP_FIELD_OR
 * - `M_JSON_CS_FUN`: Alias for M_VCT_TOOLS_JSON_CONSTRUCTOR_FUNCTION
 * 
 * @example
 * @code
 * #define M_VCT_JSON_SIMPLIFY_MACROS
 * #include <vct/tools/json_macros.hpp>
 * import vct.tools.json;
 * 
 * class User {
 *     std::string m_name;
 *     int m_id;
 *     bool m_active;
 * 
 * public:
 *     // Conversion to JSON (simplified syntax)
 *     M_JSON_CV_FUN(User,
 *         M_JSON_CV_MAP(name, m_name),
 *         M_JSON_CV_MAP(id, m_id),
 *         M_JSON_CV_MAP(active, m_active)
 *     )
 *     
 *     // Constructor from JSON (simplified syntax)
 *     M_JSON_CS_FUN(User,
 *         M_JSON_CS_MAP_OR(name, m_name, "Unknown"),
 *         M_JSON_CS_MAP_OR(id, m_id, -1),
 *         M_JSON_CS_MAP_OR(active, m_active, false)
 *     )
 * };
 * @endcode
 * @note To enable these simplified macros, define M_VCT_JSON_SIMPLIFY_MACROS before including this header
 */
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

