/**
 * @file json.ixx
 * @brief JSON parsing and manipulation library module
 * @author Mysvac
 * @date 2025-07-21
 * @copyright Copyright (c) 2025 Mysvac. All rights reserved.
 *
 * This module provides a comprehensive JSON parsing, serialization and manipulation library
 * that supports all standard JSON data types and provides type-safe operations with modern C++ features.
 *
 * Features:
 * - Type-safe JSON value storage using std::variant
 * - Flexible type conversion system with multiple conversion strategies
 * - Support for both compact and pretty-printed JSON serialization
 * - Template-based generic programming with concepts for type safety
 * - STL-compatible container operations
 * - Exception-safe operations with noexcept guarantees where appropriate
 */

export module vct.tools.json;

import std;


/**
 * @namespace vct::tools::json
 * @brief Core namespace for JSON parsing library
 * 
 * Provides comprehensive JSON data parsing, serialization and manipulation functionality.
 * Supports all standard JSON data types: objects, arrays, strings, numbers, booleans and null.
 * 
 * Key features:
 * - Type-safe JSON value storage using std::variant
 * - Flexible type conversion system with multiple conversion strategies
 * - Support for both compact and pretty-printed JSON serialization
 * - Template-based generic programming with concepts for type safety
 * - STL-compatible container operations
 * - Exception-safe operations with noexcept guarantees where appropriate
 * 
 * @author Mysvac
 * @date 2025-07-21
 */
export namespace vct::tools::json{

    /**
     * @enum Type
     * @brief JSON data type enumeration
     *
     * Defines the six fundamental JSON data types as specified in RFC 7159.
     * Used internally for type identification and validation.
     */
    enum class Type{
        eNull = 0,  ///< JSON null type, represents null value
        eBool,      ///< JSON boolean type, represents true or false value
        eNumber,    ///< JSON number type, represents integer or floating-point value
        eString,    ///< JSON string type, represents Unicode character sequence enclosed in double quotes
        eArray,     ///< JSON array type, represents ordered list of values
        eObject     ///< JSON object type, represents unordered collection of key-value pairs
    };

    /**
     * @enum ParseError
     * @brief Enumeration of possible JSON parsing errors
     *
     * Comprehensive error codes for JSON parsing operations.
     * Used to provide detailed error information when parsing fails.
     */
    enum class ParseError {
        eNone = 0,          ///< No error occurred, parsing was successful
        eEmptyData,         ///< Empty data provided for parsing
        eRedundantText,     ///< Redundant text found after valid JSON data
        eDepthExceeded,     ///< Maximum nesting depth exceeded during parsing
        eIllegalEscape,     ///< Illegal escape sequence encountered in string literal
        eInvalidNumber,     ///< Invalid number format encountered
        eUnclosedString,    ///< Unclosed string literal in JSON data
        eUnclosedObject,    ///< Unclosed object literal in JSON data
        eUnclosedArray,     ///< Unclosed array literal in JSON data
        eUnknownFormat      ///< Invalid JSON format encountered
    };

    /**
     * @brief get the string name of a ParseError, should only be used for debug or log
     */
    std::string error_name(ParseError error);

    // Forward declaration
    class Value;
    
    /**
     * @typedef String
     * @brief JSON string type definition, using std::string
     *
     * Standard string type for JSON string values. UTF-8 encoded by default.
     */
    using String = std::string;
    static_assert( String{} == "", R"(deafult String type must be equal to "")");
    // static_assert( std::is_same_v<String, std::string>, "String type must be std::string" );


    /**
     * @typedef Object
     * @brief JSON object type definition, using std::map for key-value pairs
     *
     * Ordered map implementation for JSON objects. Provides deterministic iteration order.
     * Keys are sorted lexicographically for consistent serialization output.
     */
    using Object = std::map<String,Value>;
    // static_assert(std::is_same_v<Object, std::map<String,Value>>, "Object type must be std::map<String,Value>");

    /**
     * @typedef Array
     * @brief JSON array type definition, using std::vector
     *
     * Dynamic array implementation for JSON arrays. Provides O(1) random access
     * and efficient push_back operations for array construction.
     */
    using Array = std::vector<Value>;
    // static_assert( std::is_same_v<Array, std::vector<Value>>, "Array type must be std::vector" );

    /**
     * @typedef Number
     * @brief JSON number type definition, using double for precision
     *
     * Uses IEEE 754 double-precision floating-point for maximum compatibility
     * with JSON number specification. Can represent integers up to 2^53 exactly.
     *
     * @note Number type could be cast to Bool by `to/move` function in this library, but not equal in `==` operator.
     */
    using Number = double;
    static_assert( Number{} == 0.0, "deafult Number type must be 0.0");
    // static_assert( std::is_same_v<Number, double>, "Number type must be double" );


    /**
     * @typedef Bool
     * @brief JSON boolean type definition, using bool
     *
     * Standard boolean type for JSON true/false values.
     *
     * @note Bool type could be cast to Number by `to/move` function in this library, but not equal in `==` operator.
     */
    using Bool = bool;
    static_assert( Bool{} == false, "deafult Bool type must be false");
    // static_assert( std::is_same_v<Bool, bool>, "Bool type must be bool" );


    /**s
     * @typedef Null
     * @brief JSON null type definition, using std::nullptr_t
     *
     * Represents JSON null values using the standard null pointer type.
     * Provides type safety and distinguishes from other "empty" values.
     *
     * @note Null type could not cast to false or 0 in this library,
     */
    using Null = std::nullptr_t;
    static_assert( Null{} == nullptr, "default Null type must be nullptr");
    // static_assert(std::is_same_v<Null, std::nullptr_t>, "Null type must be std::nullptr_t");


    /**
     * @concept json_type
     * @brief Concept to constrain template types to valid JSON types
     *
     * Ensures that template parameters are one of the six fundamental JSON types.
     * Used throughout the library for type safety and clear error messages.
     *
     * @tparam T Type to check for JSON compatibility
     */
    template<typename T>
    concept json_type = std::disjunction_v<
        std::is_same<T, Null>,
        std::is_same<T, Bool>,
        std::is_same<T, Number>,
        std::is_same<T, String>,
        std::is_same<T, Array>,
        std::is_same<T, Object>
    >;

    /**
     * @concept convertible
     * @brief The directly available types for 'to/move' function, include 'json_type'
     *
     * Includes JSON types plus additional types that can be safely converted
     * to or from JSON representation, including arithmetic types, enums,
     * and types with appropriate conversion constructors.
     *
     * @tparam T Type to check for conversion compatibility
     */
    template<typename T>
    concept convertible = std::disjunction_v<
        std::is_arithmetic<T>,
        std::is_enum<T>,
        std::is_convertible<Array, T>,
        std::is_convertible<Object, T>,
        std::is_convertible<String, T>,
        std::is_convertible<Number, T>,
        std::is_convertible<Bool, T>,
        std::is_convertible<Null, T>,
        std::is_constructible<T, Value>
    >;

    /**
     * @concept convertible_map
     * @brief Convertable k-v type, D is default mapped type
     *
     * Defines requirements for types that can be converted to/from JSON Objects.
     * The type must be a range with key_type and mapped_type, where keys can be
     * converted to/from String and values can be converted to/from Value.
     *
     * @tparam T Map-like type to check
     * @tparam D Default mapped type for failed conversions
     */
    template<typename T, typename D>
    concept convertible_map = std::ranges::range<T> && requires {
        typename T::key_type;
        typename T::mapped_type;
        requires std::is_constructible_v<typename T::mapped_type, D>;
        requires std::is_convertible_v<String, typename T::key_type>;
        requires convertible<typename T::mapped_type>;
        requires std::is_default_constructible_v<T>;
        requires std::is_default_constructible_v<typename T::mapped_type>;
        requires std::is_copy_constructible_v<typename T::mapped_type>;
    } && requires (T t, String s, typename T::mapped_type m) {
        t.emplace(static_cast<typename T::key_type>(s), std::move(m));
    };

    /**
     * @concept convertible_array
     * @brief Convertable array type, D is default value type
     *
     * Defines requirements for types that can be converted to/from JSON Arrays.
     * The type must be a range with value_type that can be converted to/from Value.
     *
     * @tparam T Array-like type to check
     * @tparam D Default value type for failed conversions
     */
    template<typename T, typename D>
    concept convertible_array =  std::ranges::range<T> && requires {
        typename T::value_type;
        requires std::is_constructible_v<typename T::value_type,D>;
        requires convertible<typename T::value_type>;
        requires std::is_default_constructible_v<T>;
        requires std::is_default_constructible_v<typename T::value_type>;
        requires std::is_copy_constructible_v<typename T::value_type>;
    } && requires (T t, typename T::value_type v) {
        t.emplace_back(std::move(v));
    };

    /**
     * @class Value
     * @brief Universal container class for JSON values
     * 
     * The core class of the JSON library that can store and manipulate any JSON value type.
     * Uses std::variant internally for type-safe storage and provides comprehensive
     * type conversion, serialization, and manipulation capabilities.
     *
     * Features:
     * - Type-safe storage using std::variant
     * - Flexible conversion system with multiple strategies (to, to_if, to_or, move variants)
     * - STL-compatible container access for objects and arrays
     * - Comprehensive comparison operators
     * - Exception-safe operations with noexcept guarantees where possible
     * - Support for both compact and pretty-printed serialization
     *
     * Stores and manipulates JSON values of any type using std::variant for type-safe storage.
     * Supports all JSON data types: objects, arrays, strings, numbers, booleans and null.
     * 
     * @note Class size is 40 bytes, default constructs to null type
     */
    class Value{
        /**
         * @brief Internal data storage type
         */
        using Variant = std::variant<
            Null,       ///< JSON null value storage
            Bool,       ///< JSON boolean value storage
            Number,     ///< JSON number storage
            String,     ///< JSON string storage
            Array,      ///< JSON array storage
            Object      ///< JSON object storage
        >;

        Variant m_data { Null{} };      ///< Actual data storage, defaults to null

    public:

        /**
         * @brief Get the current JSON type
         * @return The current Type of this Value
         */
        [[nodiscard]]
        constexpr Type type() const noexcept { return static_cast<Type>(m_data.index()); }

        /**
         * @brief Get the string name of current JSON (should only be used for debug)
         * @return The current Type Name(string) of this Value
         */
        [[nodiscard]]
        constexpr String type_name() const noexcept {
            switch ( this->type() ) {
                case Type::eObject: return "Object";
                case Type::eArray:  return "Array";
                case Type::eString: return "String";
                case Type::eNumber: return "Number";
                case Type::eBool:   return "Bool";
                case Type::eNull:   return "Null";
                default: return "Unknown Enum Value"; // should never happen
            }
        }

        /**
         * @brief check if this Value is null
         * @return true if this Value is null, false otherwise
         */
        [[nodiscard]]
        constexpr bool is_nul() const noexcept { return type() == Type::eNull; }

        /**
         * @brief check if this Value is boolean
         * @return true if this Value is boolean, false otherwise
         */
        [[nodiscard]]
        constexpr bool is_bol() const noexcept { return type() == Type::eBool; }

        /**
         * @brief check if this Value is numeric
         * @return true if this Value is numeric, false otherwise
         */
        [[nodiscard]]
        constexpr bool is_num() const noexcept { return type() == Type::eNumber; }

        /**
         * @brief check if this Value is string
         * @return true if this Value is string, false otherwise
         */
        [[nodiscard]]
        constexpr bool is_str() const noexcept { return type() == Type::eString; }

        /**
         * @brief check if this Value is array
         * @return true if this Value is array, false otherwise
         */
        [[nodiscard]]
        constexpr bool is_arr() const noexcept { return type() == Type::eArray; }

        /**
         * @brief check if this Value is object
         * @return true if this Value is object, false otherwise
         */
        [[nodiscard]]
        constexpr bool is_obj() const noexcept { return type() == Type::eObject; }

        /**
         * @brief Get the Null value from this Value
         * @throw std::bad_variant_access if this Value is not Null
         * @return Reference to Null value (Null itself is immutable)
         * @note Overloads:
         * - `Null& nul() &`              : For non-const lvalues
         * - `Null&& nul() &&`            : For rvalues (enables moving)
         * - `const Null& nul() const &`  : For const lvalues
         * - `const Null&& nul() const &&`: For const rvalues (rarely needed)
         */
        [[nodiscard]]
        constexpr Null& nul() & { return std::get<Null>(m_data); }
        [[nodiscard]]
        constexpr Null&& nul() && { return std::get<Null>(std::move(m_data)); }
        [[nodiscard]]
        constexpr const Null& nul() const & { return std::get<Null>(m_data); }
        [[nodiscard]]
        constexpr const Null&& nul() const && { return std::get<Null>(std::move(m_data)); }

        /**
         * @brief Get the Bool value from this Value
         * @throw std::bad_variant_access if this Value is not Bool
         * @return Mutable reference to Bool value
         * @note Overloads:
         * - `Bool& bol() &`              : For non-const lvalues
         * - `Bool&& bol() &&`            : For rvalues (enables moving)
         * - `const Bool& bol() const &`  : For const lvalues
         * - `const Bool&& bol() const &&`: For const rvalues (rarely needed)
         */
        [[nodiscard]]
        constexpr Bool& bol() & { return std::get<Bool>(m_data); }
        [[nodiscard]]
        constexpr Bool&& bol() && { return std::get<Bool>(std::move(m_data)); }
        [[nodiscard]]
        constexpr const Bool& bol() const & { return std::get<Bool>(m_data); }
        [[nodiscard]]
        constexpr const Bool&& bol() const && { return std::get<Bool>(std::move(m_data)); }

        /**
         * @brief Get the Number value from this Value
         * @throw std::bad_variant_access if this Value is not Number
         * @return Mutable reference to Number value
         * @note Overloads:
         * - `Number& num() &`              : For non-const lvalues
         * - `Number&& num() &&`            : For rvalues (enables moving)
         * - `const Number& num() const &`  : For const lvalues
         * - `const Number&& num() const &&`: For const rvalues (rarely needed)
         */
        [[nodiscard]]
        constexpr Number& num() & { return std::get<Number>(m_data); }
        [[nodiscard]]
        constexpr Number&& num() && { return std::get<Number>(std::move(m_data)); }
        [[nodiscard]]
        constexpr const Number& num() const & { return std::get<Number>(m_data); }
        [[nodiscard]]
        constexpr const Number&& num() const && { return std::get<Number>(std::move(m_data)); }

        /**
         * @brief Get the String value from this Value
         * @throw std::bad_variant_access if this Value is not String
         * @return Mutable reference to String value
         * @note Overloads:
         * - `String& str() &`              : For non-const lvalues
         * - `String&& str() &&`            : For rvalues (enables moving)
         * - `const String& str() const &`  : For const lvalues
         * - `const String&& str() const &&`: For const rvalues (rarely needed)
         */
        [[nodiscard]]
        constexpr String& str() & { return std::get<String>(m_data); }
        [[nodiscard]]
        constexpr String&& str() && { return std::get<String>(std::move(m_data)); }
        [[nodiscard]]
        constexpr const String& str() const & { return std::get<String>(m_data); }
        [[nodiscard]]
        constexpr const String&& str() const && { return std::get<String>(std::move(m_data)); }

        /**
         * @brief Get the Array value from this Value
         * @throw std::bad_variant_access if this Value is not Array
         * @return Mutable reference to Array value
         * @note Overloads:
         * - `Array& arr() &`              : For non-const lvalues
         * - `Array&& arr() &&`            : For rvalues (enables moving)
         * - `const Array& arr() const &`  : For const lvalues
         * - `const Array&& arr() const &&`: For const rvalues (rarely needed)
         */
        [[nodiscard]]
        constexpr Array& arr() & { return std::get<Array>(m_data); }
        [[nodiscard]]
        constexpr Array&& arr() && { return std::get<Array>(std::move(m_data)); }
        [[nodiscard]]
        constexpr const Array& arr() const & { return std::get<Array>(m_data); }
        [[nodiscard]]
        constexpr const Array&& arr() const && { return std::get<Array>(std::move(m_data)); }

        /**
         * @brief Get the Object value from this Value
         * @throw std::bad_variant_access if this Value is not Object
         * @return Mutable reference to Object value
         * @note Overloads:
         * - `Object& obj() &`              : For non-const lvalues.
         * - `Object&& obj() &&`            : For rvalues (enables moving).
         * - `const Object& obj() const &`  : For const lvalues.
         * - `const Object&& obj() const &&`: For const rvalues (rarely needed).
         */
        [[nodiscard]]
        constexpr Object& obj() & { return std::get<Object>(m_data); }
        [[nodiscard]]
        constexpr Object&& obj() && { return std::get<Object>(std::move(m_data)); }
        [[nodiscard]]
        constexpr const Object& obj() const & { return std::get<Object>(m_data); }
        [[nodiscard]]
        constexpr const Object&& obj() const && { return std::get<Object>(std::move(m_data)); }

        /**
         * @brief Default constructor, initializes to null type and null data
         */
        constexpr Value() = default;
        
        /**
         * @brief Default destructor
         */
        ~Value() = default;

        /**
         * @brief Default copy constructor
         */
        Value(const Value&) = default; 
        
        /**
         * @brief Default copy assignment operator
         * @return Reference to this Value object
         */
        Value& operator=(const Value&) = default;

        /**
         * @brief Move constructor
         * @param other The Value object to move from
         * 
         * Moves data from another Value object. Source object becomes null.
         */
        Value(Value&& other) noexcept {
            m_data = std::move(other.m_data);
            other.m_data = Null{};
        }

        /**
         * @brief Move assignment operator
         * @param other The Value object to move from
         * @return Reference to this Value object
         * 
         * Moves data from another Value object. Source object becomes null.
         */
        Value& operator=(Value&& other) noexcept {
            if (this == &other) return *this;
            m_data = std::move(other.m_data);
            other.m_data = Null{};
            return *this;
        }

        /**
         * @brief Clear data to nullptr and reset type to null
         */
        template<json_type T = Null>
        void reset() noexcept {
            if constexpr(std::is_same_v<T, Null>) {
                m_data = Null{};
            } else if constexpr(std::is_same_v<T, Bool>) {
                m_data = Bool{};
            } else if constexpr(std::is_same_v<T, Number>) {
                m_data = Number{};
            } else if constexpr(std::is_same_v<T, String>) {
                m_data = String{};
            } else if constexpr(std::is_same_v<T, Array>) {
                m_data = Array{};
            } else if constexpr(std::is_same_v<T, Object>) {
                m_data = Object{};
            }
        }

        /**
         * @brief Construct Value from nullptr
         */
        Value(const Null) noexcept {}

        /**
         * @brief Assign Value from nullptr
         */
        Value& operator=(const Null) noexcept {
            m_data = Null{};
            return *this;
        }
        
        /**
         * @brief Construct Value from boolean
         * @param bol Boolean value to initialize
         */
        Value(const Bool bol) noexcept : m_data(bol) {}

        /**
         * @brief Assign Value from boolean
         * @param bol Boolean value to assign
         */
        Value& operator=(const Bool bol) noexcept {
            m_data = bol;
            return *this;
        }

        /**
         * @brief Construct Value from numeric type
         * @param num Numeric value to initialize
         */
        Value(Number num) noexcept : m_data(num) {}

        /**
         * @brief Assign Value from numeric type
         * @param num Numeric value to assign
         */
        Value& operator=(const Number num) noexcept {
            m_data = num;
            return *this;
        }

        /**
         * @brief Construct Value from numeric types
         * @tparam T Numeric type (integral or floating-point or enum)
         * @warning Every enum class will be handled as Integer. including json::Type.
         */
        template<typename T>
        requires (std::is_arithmetic_v<T> || std::is_enum_v<T>)
        Value(T num) noexcept : m_data( static_cast<Number>(num) ) {}

        /**
         * @brief Assign Value from numeric types
         * @tparam T Numeric type (integral or floating-point or enum)
         * @warning Every enum class will be handled as Integer. including json::Type.
         */
        template<typename T>
        requires (std::is_arithmetic_v<T> || std::is_enum_v<T>)
        Value& operator=(T num) noexcept {
            m_data = static_cast<Number>(num);
            return *this;
        }

        /**
         * @brief Construct Value from string types
         * @param str String value to initialize
         */
        Value(String str) noexcept : m_data(std::move(str)) {}

        /**
         * @brief Construct Value from C-style string literal
         * @param str C-style string literal or const char*
         */
        Value(const char* str) noexcept : m_data(String{str}) {}

        /**
         * @brief Assign Value from string types
         * @param str String value to assign
         */
        Value& operator=(String str) noexcept {
            m_data = std::move(str);
            return *this;
        }

        /**
         * @brief Assign Value from C-style string literal
         * @param str C-style string literal or const char*
         */
        Value& operator=(const char* str) noexcept {
            m_data = String{str};
            return *this;
        }
        
        /**
         * @brief Construct Value from Object
         * @param obj Object to initialize
         */
        Value(Object obj) noexcept : m_data(std::move(obj)) {}


        /**
         * @brief Assign Value from Object
         * @param obj Object to assign
         */
        Value& operator=(Object obj) noexcept {
            m_data = std::move(obj);
            return *this;
        }
        
        /**
         * @brief Construct Value from Array
         * @param arr Array to initialize
         */
        Value(Array arr) noexcept : m_data(std::move(arr)) {}

        /**
         * @brief Assign Value from Array
         * @param arr Array to assign
         */
        Value& operator=(Array arr) noexcept {
            m_data = std::move(arr);
            return *this;
        }


        /**
         * @brief Construct Value from Array types
         * @tparam T Array type, must be a range and value type must be convertible to Value
         * @param other The array-like object to initialize from
         */
        template<std::ranges::range T>
        requires std::is_constructible_v<Value, typename T::value_type>
        explicit Value(T other) noexcept : m_data( Array{} ) {
            auto& arr = std::get<Array>(m_data);
            for (auto& item : other) {
                arr.emplace_back( static_cast<Value>(std::move(item)) );
            }
        }

        /**
         * @brief Construct Value from Object types (map-like)
         * @tparam T Object type, must be a range and key_type must be convertible to String, mapped_type must be convertible to Value
         * @param other The map-like object to initialize from
         */
        template<std::ranges::range T>
        requires std::is_convertible_v<typename T::key_type, String> && std::is_constructible_v<Value, typename T::mapped_type>
        explicit Value(T other) noexcept : m_data( Object{} ) {
            auto& obj = std::get<Object>(m_data);
            for (auto& [key, val]: other) {
                obj.emplace( static_cast<String>(key), static_cast<Value>(std::move(val)) );
            }
        }

        /**
         * @brief Access object member by key
         * @param key The key to access
         * @note if Value is mutable, you can use this operator to create a new key-value pair
         * @return Reference to Value, creates if not exists
         * @throw std::bad_variant_access if not an object
         */
        [[nodiscard]]
        Value& operator[](const String& key) { return std::get<Object>(m_data)[key]; }
        /**
         * @brief Access object member by key
         * @param key The key to access
         * @return Const Reference to Value, throws if key doesn't exist
         * @throw std::bad_variant_access if not an object
         * @throw std::out_of_range if key doesn't exist
         */
        [[nodiscard]]
        const Value& operator[](const String& key) const { return std::get<Object>(m_data).at(key); }
        
        /**
         * @brief Access array element by index
         * @param index The index to access
         * @return Reference to Value at index
         * @throw std::bad_variant_access if not an array
         * @warning if index out of bounds, application will crash or occur other undefined behavior 
         */
        [[nodiscard]]
        Value& operator[](const std::size_t index) { return std::get<Array>(m_data)[index]; }

        /**
         * @brief Access array element by index
         * @param index The index to access
         * @return Const Reference to Value at index
         * @throw std::bad_variant_access if not an array
         * @throw std::out_of_range if key doesn't exist
         */
        const Value& operator[](const std::size_t index) const { return std::get<Array>(m_data).at(index); }


        /**
         * @brief Safe access to object member by key
         * @param key The key to access
         * @return Reference to Value
         * @throw std::bad_variant_access if not an object
         * @throw std::out_of_range if key doesn't exist
         */
        [[nodiscard]]
        Value& at(const String& key) { return std::get<Object>(m_data).at(key); }

        /**
         * @brief Safe access to object member by key
         * @param key The key to access
         * @return Const Reference to Value
         * @throw std::bad_variant_access if not an object
         * @throw std::out_of_range if key doesn't exist
         */
        const Value& at(const String& key) const { return std::get<Object>(m_data).at(key); }

        
        /**
         * @brief Safe access to array element by index
         * @param index The index to access
         * @return Reference to Value at index
         * @throws std::bad_variant_access if not an array
         * @throws std::out_of_range if index out of bounds
         */
        [[nodiscard]]
        Value& at(const std::size_t index) { return std::get<Array>(m_data).at(index); }

        /**
         * @brief Safe access to array element by index
         * @param index The index to access
         * @return Const Reference to Value at index
         * @throws std::bad_variant_access if not an array
         * @throws std::out_of_range if index out of bounds
         */
        const Value& at(const std::size_t index) const { return std::get<Array>(m_data).at(index); }
        


        /**
         * @brief Serialize Value to string buffer back ( Must succeed )
         * @param out Reference to string buffer to write output
         */
        void write(String& out) const noexcept;
        
        /**
         * @brief Serialize Value to output stream
         * @param out Output stream to write JSON data
         * @note 
         * output to stream may be failed If an unknown problem occurs (e.g. stream is closed or not writable)
         * when stream.fail() return true, this function will quickly return but not throw any exception.
         */
        void write(std::ostream& out) const noexcept;

        /**
         * @brief Serialize Value to compact JSON string ( Must succeed )
         * @return Compact JSON string without formatting
         */
        [[nodiscard]]
        String dump() const noexcept {
            String res;
            this->write(res);
            return res;
        }

        /**
         * @brief Serialize Value to pretty-formatted string buffer
         * @param out Reference to string buffer to write output
         * @param space_num Number of spaces per indentation level (default: 2)
         * @param depth Current indentation depth (default: 0)
         * @param max_space Maximum allowed indentation spaces (default: 512)
         * @return True if serialization succeeded, false if max_space exceeded
         */
        Bool writef(
            String& out,
            std::uint16_t space_num = 2,
            std::uint16_t depth = 0,
            std::uint32_t max_space = 512
        ) const noexcept;

        /**
         * @brief Serialize Value to pretty-formatted output stream
         * @param out Output stream to write JSON data
         * @param space_num Number of spaces per indentation level (default: 2)
         * @param depth Current indentation depth (default: 0)
         * @param max_space Maximum allowed indentation spaces (default: 512)
         * @return True if serialization succeeded, false if max_space exceeded or stream failed
         */
        Bool writef(
            std::ostream& out, 
            std::uint16_t space_num = 2,
            std::uint16_t depth = 0,
            std::uint32_t max_space = 512
        ) const noexcept;


        /**
         * @brief Serialize Value to pretty-formatted JSON string
         * @param space_num Number of spaces per indentation level (default: 2)
         * @param depth Initial depth (default: 0)
         * @param max_space Maximum allowed spaces to prevent memory explosion (default: 512)
         * @return Formatted JSON string or nullopt if max_space exceeded
         */
        [[nodiscard]]
        std::optional<String> dumpf(
            const std::uint16_t space_num = 2, 
            const std::uint16_t depth = 0, 
            const std::uint32_t max_space = 512
        ) const noexcept {
            if(String res; this->writef(res, space_num, depth, max_space)) return res;
            return std::nullopt;
        }


        /**
         * @brief type conversion, copy inner value to specified type
         * @tparam T The target type to convert to
         * @tparam D The mapped_type or value_type of the target type, used for range conversion, default is Null for other types(useless).
         * @param default_range_elem if T is a range type and is not json::Array or json::Object, must be specified for safe conversion. Else, please use default value.
         * @return The converted value
         * @note Number is double, so conversions to integral (and enum) types will round to nearest.
         * @details
         * Attempt sequence of conversions:
         * inner value type -> target type
         * 1. Null -> Null
         * 2. Object -> Object
         * 3. Array -> Array
         * 4. String -> String
         * 5. Bool -> Bool
         * 6. Number -> enum types (us llround, round to nearest)
         * 7. Number -> integral types (us llround, round to nearest)
         * 8. Number -> floating_point types
         * 9. Any -> T is constructible from json::Value
         * 10. Object -> implicit convertible types
         * 11. Array -> implicit convertible types
         * 12. String -> implicit convertible types
         * 13. Number -> implicit convertible types
         * 14. Bool -> implicit convertible types
         * 15. Null -> implicit convertible types (Null is not convertible to bool !!!!!)
         * 16. Object -> Try copy to `range && String->key_type && Value->mapped_type types && have default_range_value`
         * 17. Array -> Try copy to `range && Value->value_type types && have default_range_value`
         * 18. return std::nullopt;
         */
        template<typename T, typename D = Null>
        requires convertible<T> || convertible_map<T, D> || convertible_array<T, D>
        [[nodiscard]]
        std::optional<T>  to_if( D default_range_elem = D{} ) const noexcept {
            if constexpr (std::is_same_v<T, Null>) {
                if (type() == Type::eNull) return Null{};
            } else if constexpr (std::is_same_v<T, Object>) {
                if (type() == Type::eObject) return std::get<Object>(m_data);
            } else if constexpr (std::is_same_v<T, Array>) {
                if (type() == Type::eArray) return std::get<Array>(m_data);
            } else if constexpr (std::is_same_v<T, String>) {
                if (type() == Type::eString) return std::get<String>(m_data);
            } else if constexpr (std::is_same_v<T, Bool>) {
                if (type() == Type::eBool) return std::get<Bool>(m_data);
            } else if constexpr (std::is_enum_v<T>) {
                if (type() == Type::eNumber) return static_cast<T>(std::llround(std::get<Number>(m_data)));
            } else if constexpr (std::is_integral_v<T>) {
                if (type() == Type::eNumber) return static_cast<T>(std::llround(std::get<Number>(m_data)));
            } else if constexpr (std::is_floating_point_v<T>) {
                if (type() == Type::eNumber) return static_cast<T>(std::get<Number>(m_data));
            }
            if constexpr (std::is_constructible_v<T, Value>) {
                return static_cast<T>(*this);
            }
            if constexpr (std::is_convertible_v<Object, T>) {
                if (type() == Type::eObject) return static_cast<T>(std::get<Object>(m_data));
            }
            if constexpr (std::is_convertible_v<Array, T>) {
                if (type() == Type::eArray) return static_cast<T>(std::get<Array>(m_data));
            }
            if constexpr (std::is_convertible_v<String, T>) {
                if (type() == Type::eString) return static_cast<T>(std::get<String>(m_data));
            }
            if constexpr (std::is_convertible_v<Number, T>) {
                if (type() == Type::eNumber) return static_cast<T>(std::get<Number>(m_data));
            }
            if constexpr (std::is_convertible_v<Bool, T>) {
                if (type() == Type::eBool) return static_cast<T>(std::get<Bool>(m_data));
            }
            if constexpr (std::is_convertible_v<Null, T>) {
                if (type() == Type::eNull) return static_cast<T>(Null{});
            }
            if constexpr ( convertible_map<T, D> ) {
                if (type() == Type::eObject) {
                    T result{};
                    for (auto& [key, value] : std::get<Object>(m_data)) {
                        auto val = value.to_if<typename T::mapped_type>();
                        if (!val) result.emplace(static_cast<typename T::key_type>(key), static_cast<typename T::mapped_type>(default_range_elem));
                        else result.emplace(static_cast<typename T::key_type>(key), std::move(*val));
                    }
                    return result;
                }
            }
            if constexpr ( convertible_array<T, D> ) {
                if (type() == Type::eArray) {
                    T result{};
                    for (auto& value : std::get<Array>(m_data)) {
                        auto val = value.to_if<typename T::value_type>();
                        if (!val) result.emplace_back(default_range_elem);
                        else result.emplace_back(std::move(*val));
                    }
                    return result;
                }
            }
            return std::nullopt; // return nullopt if conversion fails
        }

        /**
         * @brief type conversion, copy inner value to specified type
         * @tparam T The target type to convert to
         * @tparam D The mapped_type or value_type of the target type, used for range conversion, default is Null for other types(useless).
         * @param default_range_elem if T is a range type and is not json::Array or json::Object, must be specified for safe conversion. Else, please use default value.
         * @return The converted value
         * @throws std::runtime_error if conversion fails
         * @note Number is double, so conversions to integral (and enum) types will round to nearest.
         * @details
         * Attempt sequence of conversions:
         * inner value type -> target type
         * 1-17: equal to `to_if<T, D>()`
         * 18. throw std::runtime_error
         */
        template<typename T, typename D = Null>
        requires convertible<T> || convertible_map<T, D> || convertible_array<T, D>
        [[nodiscard]]
        T  to( D default_range_elem = D{} ) const {
            auto opt = to_if<T, D>( std::move(default_range_elem) );
            if (!opt) throw std::runtime_error("Value type mismatch");
            return *opt;
        }

        /**
         * @brief type conversion, copy inner value to specified type
         * @tparam T The target type to convert to
         * @tparam D The mapped_type or value_type of the target type, used for range conversion, default is Null for other types(useless).
         * @param default_result The default value to return if conversion fails
         * @param default_range_elem if T is a range type and is not json::Array or json::Object, must be specified for safe conversion. Else, please use default value.
         * @return The converted value or default_value if conversion fails
         * @note Number is double, so conversions to integral (and enum) types will round to nearest.
         * @details
         * Attempt sequence of conversions:
         * inner value type -> target type
         * 1-17: equal to `to_if<T, D>()`
         * 18: return default_result
         */
        template<typename T, typename D = Null>
        requires convertible<T> || convertible_map<T, D> || convertible_array<T, D>
        [[nodiscard]]
        T  to_or( T default_result, D default_range_elem = D{} ) const noexcept {
            auto opt = to_if<T, D>( std::move(default_range_elem) );
            if (!opt) return std::move(default_result);
            return *opt;
        }

        /**
         * @brief type conversion, Move or Copy inner value to specified type
         * @tparam T The target type to convert to
         * @tparam D The mapped_type or value_type of the target type, used for range conversion, default is Null for other types(useless).
         * @param default_range_elem if T is a range type and is not json::Array or json::Object, must be specified for safe conversion. Else, please use default value.
         * @return The converted value
         * @note
         * Number is double, so conversions to integral (and enum) types will round to nearest.
         * Complex types like Object, Array, String will be moved if possible.
         * Simple types like Bool, Number, Null will be copied.
         * @details
         * Attempt sequence of conversions:
         * inner value type -> target type
         * 1. Null -> Null
         * 2. Object -> Object (Move)
         * 3. Array -> Array (Move)
         * 4. String -> String (Move)
         * 5. Bool -> Bool
         * 6. Number -> enum types (us llround, round to nearest)
         * 7. Number -> integral types (us llround, round to nearest)
         * 8. Number -> floating_point types
         * 9. Any -> T is constructible from json::Value (try Move)
         * 10. Object -> implicit convertible types (try Move)
         * 11. Array -> implicit convertible types (try Move)
         * 12. String -> implicit convertible types (try Move)
         * 13. Number -> implicit convertible types
         * 14. Bool -> implicit convertible types
         * 15. Null -> implicit convertible types (Null is not convertible to bool !!!!!)
         * 16. Object -> Try copy to `range && String->key_type && Value->mapped_type types && have default_range_value`  (try Move)
         * 17. Array -> Try copy to `range && Value->value_type types && have default_range_value`  (try Move)
         * 18. return std::nullopt;
         */
        template<typename T, typename D = Null>
        requires convertible<T> || convertible_map<T, D> || convertible_array<T, D>
        [[nodiscard]]
        std::optional<T>  move_if( D default_range_elem = D{} ) noexcept {
            if constexpr (std::is_same_v<T, Null>) {
                if (type() == Type::eNull) return Null{};
            } else if constexpr (std::is_same_v<T, Object>) {
                if (type() == Type::eObject) return std::move(std::get<Object>(m_data));
            } else if constexpr (std::is_same_v<T, Array>) {
                if (type() == Type::eArray) return std::move(std::get<Array>(m_data));
            } else if constexpr (std::is_same_v<T, String>) {
                if (type() == Type::eString) return std::move(std::get<String>(m_data));
            } else if constexpr (std::is_same_v<T, Bool>) {
                if (type() == Type::eBool) return std::get<Bool>(m_data);
            } else if constexpr (std::is_enum_v<T>) {
                if (type() == Type::eNumber) return static_cast<T>(std::llround(std::get<Number>(m_data)));
            } else if constexpr (std::is_integral_v<T>) {
                if (type() == Type::eNumber) return static_cast<T>(std::llround(std::get<Number>(m_data)));
            } else if constexpr (std::is_floating_point_v<T>) {
                if (type() == Type::eNumber) return static_cast<T>(std::get<Number>(m_data));
            }
            if constexpr (std::is_constructible_v<T, Value>) {
                return static_cast<T>(std::move(*this));
            }
            if constexpr (std::is_convertible_v<Object, T>) {
                if (type() == Type::eObject) return static_cast<T>(std::move(std::get<Object>(m_data)));
            }
            if constexpr (std::is_convertible_v<Array, T>) {
                if (type() == Type::eArray) return static_cast<T>(std::move(std::get<Array>(m_data)));
            }
            if constexpr (std::is_convertible_v<String, T>) {
                if (type() == Type::eString) return static_cast<T>(std::move(std::get<String>(m_data)));
            }
            if constexpr (std::is_convertible_v<Number, T>) {
                if (type() == Type::eNumber) return static_cast<T>(std::get<Number>(m_data));
            }
            if constexpr (std::is_convertible_v<Bool, T>) {
                if (type() == Type::eBool) return static_cast<T>(std::get<Bool>(m_data));
            }
            if constexpr (std::is_convertible_v<Null, T>) {
                if (type() == Type::eNull) return static_cast<T>(Null{});
            }
            if constexpr ( convertible_map<T, D> ) {
                if (type() == Type::eObject) {
                    T result{};
                    for (auto& [key, value] : std::get<Object>(m_data)) {
                        auto val = value.move_if<typename T::mapped_type>();
                        if (!val) result.emplace(static_cast<typename T::key_type>(key), static_cast<typename T::mapped_type>(default_range_elem));
                        else result.emplace(static_cast<typename T::key_type>(key), std::move(*val));
                    }
                    return result;
                }
            }
            if constexpr ( convertible_array<T, D> ) {
                if (type() == Type::eArray) {
                    T result{};
                    for (auto& value : std::get<Array>(m_data)) {
                        auto val = value.move_if<typename T::value_type>();
                        if (!val) result.emplace_back( static_cast<typename T::value_type>(default_range_elem) );
                        else result.emplace_back(std::move(*val));
                    }
                    return result;
                }
            }
            return std::nullopt; // return nullopt if conversion fails
        }

        /**
         * @brief type conversion, Move or Copy inner value to specified type
         * @tparam T The target type to convert to
         * @tparam D The mapped_type or value_type of the target type, used for range conversion, default is Null for other types(useless).
         * @param default_range_elem if T is a range type and is not json::Array or json::Object, must be specified for safe conversion. Else, please use default value.
         * @return The converted value
         * @throws std::runtime_error if conversion fails
         * @note 
         * Number is double, so conversions to integral (and enum) types will round to nearest.
         * Complex types like Object, Array, String will be moved if possible.
         * Simple types like Bool, Number, Null will be copied.
         * @details
         * Attempt sequence of conversions:
         * inner value type -> target type
         * 1-17: equal to `move_if<T, D>()`
         * 18: throw std::runtime_error
         */
        template<typename T, typename D = Null>
        requires convertible<T> || convertible_map<T, D> || convertible_array<T, D>
        [[nodiscard]]
        T  move( D default_range_elem = D{} ) {
            auto opt = move_if<T, D>( std::move(default_range_elem) );
            if (!opt) throw std::runtime_error("Value type mismatch");
            return *opt;
        }




        /**
         * @brief type conversion, Move or Copy inner value to specified type
         * @tparam T The target type to convert to
         * @tparam D The mapped_type or value_type of the target type, used for range conversion, default is Null for other types(useless).
         * @param default_result The default value to return if conversion fails
         * @param default_range_elem if T is a range type and is not json::Array or json::Object, must be specified for safe conversion. Else, please use default value.
         * @return The converted value
         * @note 
         * Number is double, so conversions to integral (and enum) types will round to nearest.
         * Complex types like Object, Array, String will be moved if possible.
         * Simple types like Bool, Number, Null will be copied.
         * @details
         * Attempt sequence of conversions:
         * inner value type -> target type
         * 1-17: equal to `move_if<T, D>()`
         * 18: return default_result;
         */
        template<typename T, typename D = Null>
        requires convertible<T> || convertible_map<T, D> || convertible_array<T, D>
        [[nodiscard]]
        T  move_or( T default_result, D default_range_elem = D{} ) noexcept {
            auto opt = move_if<T, D>( std::move(default_range_elem) );
            if (!opt) return default_result;
            return *opt;
        }


        /**
         * @brief For Value-to-Value accurate comparison
         * @tparam T Type that satisfies std::equality_comparable concept
         * @warning Number is double, so the comparison of numbers will be very strict
         * @param other The value to compare with
         * @return True if values are equal, false otherwise
         */
        [[nodiscard]]
        bool operator==(const Value& other) const noexcept {
            if (type() != other.type()) return false; // Different types cannot be equal
            switch (type()) {
                case Type::eNull: return true; // Both are null
                case Type::eBool: return std::get<Bool>(m_data) == std::get<Bool>(other.m_data);
                case Type::eNumber: return std::get<Number>(m_data) == std::get<Number>(other.m_data);
                case Type::eString: return std::get<String>(m_data) == std::get<String>(other.m_data);
                case Type::eObject: return std::get<Object>(m_data) == std::get<Object>(other.m_data);
                case Type::eArray: return std::get<Array>(m_data) == std::get<Array>(other.m_data);
            }
            return false; // Should never reach here, but added for safety
        }


        /**
         * @brief Generic equality comparison operator
         * @tparam T Type that satisfies std::equality_comparable concept
         * @param other The value to compare with
         * @return True if values are equal, false otherwise
         * 
         * @details
         * rules:
         * A. different json types are not equal( Bool, Number, String, Array, Object, Null )
         * B. IF T is Value, use Value::operator==(const Value& other) ,
         *    it's Accurate comparison !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
         * --------------------------------------------------------------------------------------------
         * 1.  IF T is Null, return true if this Value is null, false otherwise
         * 2.  Else if T is Bool, return true if Value is Bool and Equal to Tvalue
         * 3.  Else if T is Number, return true if Value is Number and Equal to Tvalue
         * 4.  Else if T is String, return true if Value is String and Equal to Tvalue
         * 5.  Else if T is Array, return true if Value is Array and Equal to Tvalue
         * 6.  Else if T is Object, return true if Value is Object and Equal to Tvalue
         * 7.  Else if T is enum, return true if Value is Number and Equal to Tvalue, Number will be rounded to nearest integer
         * 8.  Else if T is integral, return true if Value is Number or Bool and Equal to Tvalue, Number will be rounded to nearest integer
         * 9.  Else if T is floating_point, return true if Value is Number and Equal to Tvalue, double to double comparison is very strict
         * 10. Else if T is convertible to std::string_view, return true if Value is String and Equal to std::string_view( Tvalue )
         * 11. Else if T is equality_comparable and constructible from Value, return true if Tvalue == T(*this);
         * 12. Else if Value is constructible from T, return true if *this == json::Value(Tvalue);
         * 13. Else return false
         */
        template<typename  T>
        requires (!std::is_same_v<T, Value>)
        [[nodiscard]]
        bool operator==(const T& other) const noexcept {
            if constexpr ( std::is_same_v<T,Null> ) {
                return type() == Type::eNull;
            } else if constexpr ( std::is_same_v<T,Bool> ) {
                if ( type() == Type::eBool ) return std::get<Bool>(m_data) == other;
            } else if constexpr ( std::is_same_v<T,Number> ) {
                if ( type() == Type::eNumber ) return std::get<Number>(m_data) == other;
            } else if constexpr ( std::is_same_v<T,String> ) {
                if ( type() == Type::eString ) return std::get<String>(m_data) == other;
            } else if constexpr ( std::is_same_v<T,Array> ) {
                if ( type() == Type::eArray ) return std::get<Array>(m_data) == other;
            } else if constexpr ( std::is_same_v<T,Object> ) {
                if ( type() == Type::eObject ) return std::get<Object>(m_data) == other;
            } else if constexpr (std::is_enum_v<T>) {
                if ( type() == Type::eNumber) return static_cast<T>(std::llround(std::get<Number>(m_data))) == other;
            } else if constexpr (std::is_integral_v<T>) {
                if ( type() == Type::eNumber) return static_cast<T>(std::llround(std::get<Number>(m_data))) == other;
            } else if constexpr (std::is_floating_point_v<T>) {
                if ( type() == Type::eNumber) return static_cast<T>(std::get<Number>(m_data)) == other;
            } else if constexpr (std::is_convertible_v<T, std::string_view>) {
                if( type() == Type::eString) return std::get<String>(m_data) == std::string_view( other );
            } else if constexpr (std::equality_comparable<T> && std::is_constructible_v<T, Value>) {
                return other == static_cast<T>(*this);     // Use T's operator==
            } else if constexpr (std::is_constructible_v<Value, T>) {
                return static_cast<Value>(other) == *this; // Use Value's operator==
            }
            return false;
        }

        /**
         * @brief Get the size of container types ( Object, Array )
         * @return Size of the container, or 0 for non-container types (including string)
         * @warning String.size() is always 0, because it is not a container type in this context
         */
        [[nodiscard]]
        std::size_t size() const noexcept {
            switch (type()) {
                case Type::eObject: return std::get<Object>(m_data).size();
                case Type::eArray:  return std::get<Array>(m_data).size();
                default: return 0;
            }
        }

        /**
         * @brief Check if the Value is empty
         * @return False if the Value is Object or Array and is not empty , True otherwise
         */
        [[nodiscard]]
        bool empty() const noexcept {
            if (type() == Type::eObject) return std::get<Object>(m_data).empty();
            if (type() == Type::eArray) return std::get<Array>(m_data).empty();
            return true;
        }

        /**
         * @brief Push an element to the back of the array
         * @param value The value to push
         * @return True if the element was pushed, false if the Value is not an array
         */
        bool push_back(const Value& value) noexcept {
            if (type() != Type::eArray) return false;
            std::get<Array>(m_data).emplace_back(value);
            return true;
        }

        /**
         * @brief Push an element to the back of the array
         * @param value The value to push
         * @return True if the element was pushed, false if the Value is not an array
         */
        bool push_back(Value&& value) noexcept {
            if (type() != Type::eArray) return false;
            std::get<Array>(m_data).emplace_back(std::move(value));
            return true;
        }

        /**
         * @brief Pop an element from the back of the array
         * @return True if the element was popped, false if the Value is not an array or is empty
         */
        bool pop_back() noexcept {
            if (type() != Type::eArray || std::get<Array>(m_data).empty()) return false;
            std::get<Array>(m_data).pop_back();
            return true;
        }

        /**
         * @brief Insert an element into the array by index
         * @param index The index to insert at
         * @param value The value to insert
         * @return True if the element was inserted, false if the Value is not an array or index is out of bounds
         */
        bool insert(const std::size_t index, const Value& value) noexcept {
            if (type() != Type::eArray || index > std::get<Array>(m_data).size()) return false;
            auto& arr = std::get<Array>(m_data);
            arr.emplace(arr.begin() + index, value);
            return true;
        }

        /**
         * @brief Insert an element into the array by index
         * @param index The index to insert at
         * @param value The value to insert
         * @return True if the element was inserted, false if the Value is not an array or index is out of bounds
         */
        bool insert(const std::size_t index, Value&& value) noexcept {
            if (type() != Type::eArray || index > std::get<Array>(m_data).size()) return false;
            auto& arr = std::get<Array>(m_data);
            arr.emplace(arr.begin() + index, std::move(value));
            return true;
        }

        /**
         * @brief Insert multiple elements into the array by index
         * @param index The index to insert at
         * @param count The number of elements to insert
         * @param value The value to insert
         * @return True if the elements were inserted, false if the Value is not an array or index is out of bounds
         */
        bool insert(const std::size_t index,const std::size_t count, const Value& value) noexcept {
            if (type() != Type::eArray || index > std::get<Array>(m_data).size()) return false;
            auto& arr = std::get<Array>(m_data);
            arr.insert(arr.begin() + index, count, value);
            return true;
        }

        /**
         * @brief Erase an element from the array by index
         * @param index The index of the element to erase
         * @return True if the element was erased, false if the Value is not an array or index is out of bounds
         */
        bool erase(const std::size_t index) noexcept {
            if (type() != Type::eArray || index >= std::get<Array>(m_data).size()) return false;
            auto& arr = std::get<Array>(m_data);
            arr.erase(arr.begin() + index);
            return true;
        }
        /**
         * @brief Insert a key-value pair into the object
         * @param key The key to insert
         * @param value The value to insert
         * @return True if the key was inserted, false if the Value is not an object
         */
        bool insert(String key, const Value& value) noexcept {
            if (type() != Type::eObject) return false;
            std::get<Object>(m_data).emplace(std::move(key), value);
            return true;
        }
        /**
         * @brief Insert a key-value pair into the object
         * @param key The key to insert
         * @param value The value to insert
         * @return True if the key was inserted, false if the Value is not an object
         */
        bool insert(String key, Value&& value) noexcept {
            if (type() != Type::eObject) return false;
            std::get<Object>(m_data).emplace(std::move(key), std::move(value));
            return true;
        }
        /**
         * @brief Erase a key from the object
         * @param key The key to erase
         * @return True if the key was erased, false if the Value is not an object or the key does not exist
         */
        bool erase(const String& key) noexcept {
            if (type() != Type::eObject) return false;
            return std::get<Object>(m_data).erase(key);
        }

        /**
         * @brief Check if object contains a key
         * @param key The key to check for
         * @return True if the type is object and key exists in object, false otherwise
         */
        [[nodiscard]]
        bool contains(const String& key) const noexcept {
            if (type() != Type::eObject) return false;
            return std::get<Object>(m_data).contains(key);
        }

    };

    /**
     * @brief Parse JSON string into Value object
     * @param text The JSON string to parse
     * @param max_depth Maximum nesting depth (default: 256)
     * @return std::expected<Value, ParseError> Parsed Value or error
     */
    std::expected<Value, ParseError> read(std::string_view text, std::int32_t max_depth = 256 ) noexcept;

    /**
     * @brief Parse JSON string from input-stream into Value object
     * @param is_test input-stream
     * @param max_depth Maximum nesting depth (default: 256)
     * @return std::expected<Value, ParseError> Parsed Value or error
     */
    std::expected<Value, ParseError> read(std::istream& is_test, std::int32_t max_depth = 256 ) noexcept;

} // namespace vct::tools::json


module :private;


namespace vct::tools::json{


    std::string error_name(const ParseError error) {
        switch (error) {
            case ParseError::eNone: return "None";
            case ParseError::eEmptyData: return "EmptyData";
            case ParseError::eDepthExceeded: return "DepthExceeded";
            case ParseError::eIllegalEscape: return "IllegalEscape";
            case ParseError::eInvalidNumber: return "InvalidNumber";
            case ParseError::eRedundantText: return "RedundantText";
            case ParseError::eUnclosedArray: return "UnclosedArray";
            case ParseError::eUnclosedObject: return "UnclosedObject";
            case ParseError::eUnclosedString: return "UnclosedString";
            case ParseError::eUnknownFormat: return "UnknownFormat";
            default: return "Unknown Enum Value";
        }
    }

    void escape_to(String& out, const std::string_view str) noexcept {
        out.push_back('\"');
        for (const char c : str) {
            switch (c) {
                case '\\': out.append(R"(\\)"); break;
                case '\"': out.append(R"(\")"); break;
                case '\r': out.append(R"(\r)"); break;
                case '\n': out.append(R"(\n)"); break;
                case '\f': out.append(R"(\f)"); break;
                case '\t': out.append(R"(\t)"); break;
                case '\b': out.append(R"(\b)"); break;
                default: {
                    if (static_cast<unsigned char>(c) < 0x20) {
                        out.append(std::format("\\u{:04x}", static_cast<unsigned char>(c)));
                    } else out.push_back(c);
                } break;
            }
        }
        out.push_back('"');
    }

    void escape_to(std::ostream& out, const std::string_view str) noexcept {
        out.put('\"');
        for (const char c : str) {
            switch (c) {
                case '\\': out << R"(\\)"; break;
                case '\"': out << R"(\")"; break;
                case '\r': out << R"(\r)"; break;
                case '\n': out << R"(\n)"; break;
                case '\f': out << R"(\f)"; break;
                case '\t': out << R"(\t)"; break;
                case '\b': out << R"(\b)"; break;
                default: {
                    if (static_cast<unsigned char>(c) < 0x20) {
                        std::print(out, "\\u{:04x}", static_cast<unsigned char>(c));
                    } else out.put(c);
                } break;
            }
        }
        out.put('\"');
    }


    constexpr std::array<std::uint8_t, 256> hex_table = [] {
        std::array<std::uint8_t, 256> table{};
        for (int i = 0; i < 256; ++i) table[i] = 255; // Invalid marker
        for (int i = 0; i <= 9; ++i) table['0' + i] = i;
        for (int i = 0; i <= 5; ++i) table['A' + i] = 10 + i;
        for (int i = 0; i <= 5; ++i) table['a' + i] = 10 + i;
        return table;
    }();


    template<typename T>
    concept char_iterator =  std::disjunction_v<
        std::is_same<T, std::string_view::const_iterator>,
        std::is_same<T, std::istreambuf_iterator<char>>
    >;


    std::uint32_t hex4_next(char_iterator auto& it, const char_iterator auto end_ptr) noexcept {
        // `it` wat in `\uABCD`'s A position, and not be end_ptr
        std::uint32_t result{ 0 };

        const std::uint8_t d1 = hex_table[static_cast<unsigned char>(*it)];
        if (d1 == 255) return std::numeric_limits<std::uint32_t>::max(); // Invalid if not a hex digit
        result = result << 4 | d1;

        ++it; // External `++it` may reduce some instructions
        if(it == end_ptr) return std::numeric_limits<std::uint32_t>::max(); // Invalid if not enough characters
        const std::uint8_t d2 = hex_table[static_cast<unsigned char>(*it)];
        if (d2 == 255) return std::numeric_limits<std::uint32_t>::max(); // Invalid if not a hex digit
        result = result << 4 | d2;

        ++it;
        if(it == end_ptr) return std::numeric_limits<std::uint32_t>::max(); // Invalid if not enough characters
        const std::uint8_t d3 = hex_table[static_cast<unsigned char>(*it)];
        if (d3 == 255) return std::numeric_limits<std::uint32_t>::max(); // Invalid if not a hex digit
        result = result << 4 | d3;

        ++it;
        if(it == end_ptr) return std::numeric_limits<std::uint32_t>::max(); // Invalid if not enough characters
        const std::uint8_t d4 = hex_table[static_cast<unsigned char>(*it)];
        if (d4 == 255) return std::numeric_limits<std::uint32_t>::max(); // Invalid if not a hex digit
        result = result << 4 | d4;

        // `it` wat in `\uABCD`'s D position
        return result;
    }


    bool unescape_unicode_next(
        String& out,
        char_iterator auto& it,
        const char_iterator auto end_ptr
    ) noexcept {
        // it was in `\uABCD`'s `u` position
        ++it;
        if (it == end_ptr) return false;
        // `it` was in `\uXXXX`'s A position

        // move to \uABCD's D position and get hex4 value
        std::uint32_t code_point = hex4_next( it , end_ptr);
        if(code_point > 0xFFFF) return false;
        // `it` was in `\uXXXX`'s D position and not be `end_ptr`, if hex4_next successful

        // [0xD800 , 0xE000) is agent pair, which is two consecutive \u encoding
        if (code_point >= 0xD800 && code_point <= 0xDFFF) {
            // agent pair, must be high agent + low agent
            // high agent [\uD800, \uDBFF]
            // low agent [\uDC00, \uDFFF]

            // first char must be high agent
            if (code_point >= 0xDC00) return false;

            // second char must be low agent
            ++it;
            if(it == end_ptr || *it != '\\') return false;
            ++it;
            if(it == end_ptr || *it != 'u') return false;
            ++it;
            if(it == end_ptr) return false;
            // `it` was in `\uXXXX`'s A position, and be not end_ptr

            // move to \uABCD's D position and get hex4 value
            const std::uint32_t low_code_point = hex4_next( it, end_ptr );
            if( 0xDFFF < low_code_point ||  low_code_point < 0xDC00 ) return false;
            // `it` was in `\uXXXX`'s D position and not be `end_ptr`, if hex4_next successful

            // combine the agent pair into a single code point
            code_point = 0x10000 + ((code_point - 0xD800) << 10) + (low_code_point - 0xDC00);
        }

        // encode the code point to UTF-8
        if (code_point <= 0x7F) {
            out.push_back(static_cast<char>(code_point));
        } else if (code_point <= 0x7FF) {
            out.push_back(static_cast<char>(code_point >> 6 | 0xC0));
            out.push_back(static_cast<char>(code_point & 0x3F | 0x80));
        } else if (code_point <= 0xFFFF) {
            out.push_back(static_cast<char>(code_point >> 12 | 0xE0));
            out.push_back(static_cast<char>(code_point >> 6 & 0x3F | 0x80));
            out.push_back(static_cast<char>(code_point & 0x3F | 0x80));
        } else if (code_point <= 0x10FFFF) {
            out.push_back(static_cast<char>(code_point >> 18 | 0xF0));
            out.push_back(static_cast<char>(code_point >> 12 & 0x3F | 0x80));
            out.push_back(static_cast<char>(code_point >> 6 & 0x3F | 0x80));
            out.push_back(static_cast<char>(code_point & 0x3F | 0x80));
        } else return false;
        return true;
    }


    std::expected<std::string, ParseError> unescape_next(
        char_iterator auto& it,
        const char_iterator auto end_ptr
    ) noexcept {
        std::string res;

        ++it;
        if (it != end_ptr && *it != '\"')  res.reserve( 128 );

        while (it != end_ptr && *it != '\"') {
            if (*it == '\\') {
                ++it;
                if (it == end_ptr) return std::unexpected( ParseError::eUnclosedString );
                switch (*it) {
                    case '\"': res.push_back('\"'); break;
                    case '\\': res.push_back('\\'); break;
                    case 'n':  res.push_back('\n'); break;
                    case 'r':  res.push_back('\r'); break;
                    case 't':  res.push_back('\t'); break;
                    case 'f':  res.push_back('\f'); break;
                    case 'b':  res.push_back('\b'); break;
                    case 'u': case 'U': if (!unescape_unicode_next(res, it, end_ptr)) return std::unexpected( ParseError::eIllegalEscape ); break;
                    default: return std::unexpected( ParseError::eIllegalEscape );
                }
            } else if ( *it == '\b' || *it == '\n' || *it == '\f' || *it == '\r' /* || *it == '\t' */) {
                return std::unexpected( ParseError::eIllegalEscape );
            } else {
                res.push_back( *it );
            }
            ++it;
        }
        if(it == end_ptr) return std::unexpected( ParseError::eUnclosedString );
        ++it;
        res.shrink_to_fit();
        return res;
    }


    std::expected<Value, ParseError> reader(
        char_iterator auto& it,
        const char_iterator auto end_ptr,
        const std::int32_t max_depth
    ) noexcept {
        // Check for maximum depth
        if(max_depth < 0) return std::unexpected( ParseError::eDepthExceeded );
        // return value
        Value json;
        // Check the first character to determine the type
        switch (*it) {
            case '{': {
                // Object type
                ++it;
                json = Object{};
                auto& object = json.obj();
                // Parse the object
                while(it != end_ptr){
                    // Skip spaces
                    while (it != end_ptr && std::isspace(*it)) ++it;
                    if(it == end_ptr || *it == '}') break;
                    // find key
                    if (*it != '\"') return std::unexpected( ParseError::eUnknownFormat );
                    auto key = unescape_next(it, end_ptr);
                    if(!key) return std::unexpected( key.error() );
                    // find ':'
                    while (it != end_ptr && std::isspace(*it)) ++it;
                    if(it == end_ptr || *it != ':') return std::unexpected( ParseError::eUnknownFormat );
                    ++it;
                    // find value
                    while (it != end_ptr && std::isspace(*it)) ++it;
                    if (it == end_ptr) break;
                    auto value = reader(it, end_ptr, max_depth - 1);
                    if(!value) return value;
                    // add to object
                    object.emplace(std::move(*key), std::move(*value));

                    while(it != end_ptr && std::isspace(*it)) ++it;
                    if(it == end_ptr) break;
                    if(*it == ',') ++it;
                    else if(*it != '}') return std::unexpected( ParseError::eUnknownFormat );
                }
                if(it == end_ptr) return std::unexpected( ParseError::eUnclosedObject );
                ++it;
            } break;
            case '[': {
                // Array type
                ++it;
                json = Array{};
                auto& array = json.arr();
                if (it != end_ptr && *it != ']') array.reserve(8);
                while(it != end_ptr){
                    // Skip spaces
                    while (it != end_ptr && std::isspace(*it)) ++it;
                    if(it == end_ptr || *it == ']') break;
                    // find value
                    auto value = reader(it, end_ptr, max_depth - 1);
                    if(!value) return value;
                    // add to array
                    array.emplace_back(std::move(*value));

                    while(it != end_ptr && std::isspace(*it)) ++it;
                    if(it == end_ptr) break;
                    if(*it == ',') ++it;
                    else if(*it != ']') return std::unexpected( ParseError::eUnknownFormat );
                }
                if(it == end_ptr) return std::unexpected( ParseError::eUnclosedArray );
                ++it;
                array.shrink_to_fit();
            } break;
            case '\"': {
                // String type
                auto str = unescape_next(it, end_ptr);
                if(!str) return str;
                json = std::move(*str);
            } break;
            case 't': {
                // true
                if (++it == end_ptr || *it != 'r' ||
                    ++it == end_ptr || *it != 'u' ||
                    ++it == end_ptr || *it != 'e'
                ) return std::unexpected( ParseError::eUnknownFormat );
                json = Bool{true};
                ++it;
            } break;
            case 'f': {
                // false
                if (++it == end_ptr || *it != 'a' ||
                    ++it == end_ptr || *it != 'l' ||
                    ++it == end_ptr || *it != 's' ||
                    ++it == end_ptr || *it != 'e'
                ) return std::unexpected( ParseError::eUnknownFormat );
                json = Bool{false};
                ++it;
            } break;
            case 'n': {
                // null
                if (++it == end_ptr || *it != 'u' ||
                    ++it == end_ptr || *it != 'l' ||
                    ++it == end_ptr || *it != 'l'
                ) return std::unexpected( ParseError::eUnknownFormat );
                json = Null{};
                ++it;
            } break;
            default: {
                if(* it == 'e' || *it == 'E' ) return std::unexpected( ParseError::eUnknownFormat );
                // begin with e/E is invalid, other invalid type will be handled after

                // number
                std::uint8_t buffer_len{};
                char buffer[25];    // Reserve enough space for typical numbers
                // std::array<char, 24> buffer;

                while(buffer_len < 25 && it != end_ptr &&
                    (std::isdigit(*it)  || *it=='-' || *it=='.' || *it=='e' || *it=='E' || *it=='+')
                ) buffer[buffer_len++] = *it++;
                if( buffer_len == 0 || buffer_len == 25 ) return std::unexpected( ParseError::eInvalidNumber );

                Number value;
                if(const auto [ptr, ec] = std::from_chars(buffer, buffer + buffer_len, value);
                    ec != std::errc{} || ptr != buffer + buffer_len
                ) return std::unexpected( ParseError::eInvalidNumber );

                json = value;
            } break;
        }
        return json;
    }


    std::expected<Value, ParseError> read(const std::string_view text, const std::int32_t max_depth) noexcept{
        auto it = text.begin();
        const auto end_ptr = text.end();
        // Skip spaces
        while(it != end_ptr && std::isspace(*it)) ++it;
        if(it == end_ptr) return std::unexpected( ParseError::eEmptyData );
        // Parse the JSON
        const auto result = reader(it, end_ptr, max_depth-1);
        if(!result) return result;
        // check for trailing spaces
        while(it != end_ptr && std::isspace(*it)) ++it;
        if(it != end_ptr) return std::unexpected( ParseError::eRedundantText );
        return result;
    }

    std::expected<Value, ParseError> read(std::istream& is_test, const std::int32_t max_depth) noexcept{
        auto it = std::istreambuf_iterator<char>(is_test);
        constexpr auto end_ptr = std::istreambuf_iterator<char>();
        // Skip spaces
        while(it != end_ptr && std::isspace(*it)) ++it;
        if(it == end_ptr) return std::unexpected( ParseError::eEmptyData );
        // Parse the JSON
        const auto result = reader(it, end_ptr, max_depth-1);
        if(!result) return result;
        // check for trailing spaces
        while(it != end_ptr && std::isspace(*it)) ++it;
        if(it != end_ptr) return std::unexpected( ParseError::eRedundantText );
        return result;
    }


    void Value::write(String& out) const noexcept {
        switch (type()) {
            case Type::eObject: {
                out.push_back('{');
                for (const auto& [key, val] : std::get<Object>(m_data)) {
                    // out.append(escape(key));
                    escape_to(out, key);
                    out.push_back(':');
                    val.write(out);
                    out.push_back(',');
                }
                if (*out.rbegin() == ',') *out.rbegin() = '}';
                else out.push_back('}');
            } break;
            case Type::eArray: {
                out.push_back('[');
                for (const auto& val : std::get<Array>(m_data)) {
                    val.write(out);
                    out.push_back(',');
                }
                if (*out.rbegin() == ',') *out.rbegin() = ']';
                else out.push_back(']');
            } break;
            case Type::eBool:
                out.append(std::get<Bool>(m_data) ? "true" : "false");
                break;
            case Type::eNull:
                out.append("null");
                break;
            case Type::eString:
                escape_to(out, std::get<String>(m_data));
                break;
            case Type::eNumber: {
                char buffer[24]; // Reserve enough space for typical numbers
                const auto [ptr, ec]  = std::to_chars(
                    buffer,
                    buffer + 24,
                    std::get<Number>(m_data),
                    std::chars_format::general,
                    17
                );
                if(ec != std::errc{}) {
                    out.append(std::format("{:.17}",std::get<Number>(m_data)));
                } else out.append(buffer, ptr);
            } break;
        }
    }


    void Value::write(std::ostream& out) const noexcept {
        if(out.fail()) return;
        switch (type()) {
            case Type::eObject: {
                out.put('{');
                for(bool first = true;
                    const auto& [key, val] : std::get<Object>(m_data)
                ) {
                    if(!first) out.put(',');
                    else first = false;
                    escape_to(out, key);
                    out.put(':');
                    val.write(out);
                    if(out.fail()) return;
                }
                out.put('}');
            } break;
            case Type::eArray: {
                out.put('[');
                for(bool first = true;
                    const auto& val : std::get<Array>(m_data)
                ) {
                    if(!first) out.put(',');
                    else first = false;
                    val.write(out);
                    if(out.fail()) return;
                }
                out.put(']');
            } break;
            case Type::eBool:
                out << (std::get<Bool>(m_data) ? "true" : "false");
                break;
            case Type::eNull:
                out << "null";
                break;
            case Type::eString:
                escape_to(out, std::get<String>(m_data));
                break;
            case Type::eNumber: {
                char buffer[24]; // Reserve enough space for typical numbers
                const auto [ptr, ec]  = std::to_chars(
                    buffer,
                    buffer + 24,
                    std::get<Number>(m_data),
                    std::chars_format::general,
                    17
                );
                if(ec != std::errc{}) {
                    std::print(out, "{:.17}", std::get<Number>(m_data));
                } else out.write(buffer, static_cast<std::size_t>(ptr-buffer));
            } break;
        }
    }


    Bool Value::writef(
        String& out,
        const std::uint16_t space_num,
        const std::uint16_t depth,
        const std::uint32_t max_space
    ) const noexcept {
        const std::uint32_t tabs  = depth * space_num + space_num;
        if(tabs > max_space) return false;

        switch (type()) {
            case Type::eObject: {
                out.push_back('{');
                for (const auto& [key, val] : std::get<Object>(m_data)) {
                    out.push_back('\n');
                    out.append(tabs, ' ');
                    escape_to(out, key);
                    out.append(": ");
                    if(!val.writef(out, space_num, depth + 1, max_space)) return false;
                    out.push_back(',');
                }
                if (*out.rbegin() == ',') *out.rbegin() = '\n';
                if(!std::get<Object>(m_data).empty()){
                    out.append(tabs - space_num, ' ');
                }
                out.push_back('}');
            } break;
            case Type::eArray: {
                out.push_back('[');
                for (const auto& val : std::get<Array>(m_data)) {
                    out.push_back('\n');
                    out.append(tabs, ' ');
                    if(!val.writef(out, space_num, depth + 1, max_space)) return false;
                    out.push_back(',');
                }
                if (*out.rbegin() == ',') *out.rbegin() = '\n';
                if(!std::get<Array>(m_data).empty()){
                    out.append(tabs - space_num, ' ');
                }
                out.push_back(']');
            } break;
            case Type::eBool:
                out.append(std::get<Bool>(m_data) ? "true" : "false");
                break;
            case Type::eNull:
                out.append("null");
                break;
            case Type::eString:
                escape_to(out, std::get<String>(m_data));
                break;
            case Type::eNumber: {
                char buffer[24]; // Reserve enough space for typical numbers
                const auto [ptr, ec]  = std::to_chars(
                    buffer,
                    buffer + 24,
                    std::get<Number>(m_data),
                    std::chars_format::general,
                    17
                );
                if(ec != std::errc{}) {
                    out.append(std::format("{:.17}",std::get<Number>(m_data)));
                } else out.append(buffer, ptr);
            } break;
        }
        return true;
    }


    Bool Value::writef(
        std::ostream& out,
        const std::uint16_t space_num,
        const std::uint16_t depth,
        const std::uint32_t max_space
    ) const noexcept {
        if(out.fail()) return false;
        const std::uint32_t tabs  = depth * space_num + space_num;
        if(tabs > max_space) return false;

        switch (type()) {
            case Type::eObject: {
                out.put('{');
                bool first = true;
                for(const auto& [key, val] : std::get<Object>(m_data)) {
                    if(!first) out.put(',');
                    else first = false;
                    out.put('\n');
                    out << std::setfill(' ') << std::setw(tabs) << "";
                    escape_to(out, key);
                    out.put(':');
                    out.put(' ');
                    if(!val.writef(out, space_num, depth + 1, max_space)) return false;
                }
                if(!first) out.put('\n');
                if(!std::get<Object>(m_data).empty()){
                    out << std::setfill(' ') << std::setw(tabs - space_num) << "";
                }
                out.put('}');
            } break;
            case Type::eArray: {
                out.put('[');
                bool first = true;
                for (const auto& val : std::get<Array>(m_data)) {
                    if(!first) out.put(',');
                    else first = false;
                    out.put('\n');
                    out << std::setfill(' ') << std::setw(tabs) << "";
                    if(!val.writef(out, space_num, depth + 1, max_space)) return false;
                }
                if(!first) out.put('\n');
                if(!std::get<Array>(m_data).empty()){
                    out << std::setfill(' ') << std::setw(tabs - space_num) << "";
                }
                out.put(']');
            } break;
            case Type::eBool:
                out << (std::get<Bool>(m_data) ? "true" : "false");
                break;
            case Type::eNull:
                out << "null";
                break;
            case Type::eString:
                escape_to(out, std::get<String>(m_data));
                break;
            case Type::eNumber: {
                char buffer[24]; // Reserve enough space for typical numbers
                const auto [ptr, ec]  = std::to_chars(
                    buffer,
                    buffer + 24,
                    std::get<Number>(m_data),
                    std::chars_format::general,
                    17
                );
                if(ec != std::errc{}) {
                    std::print(out, "{:.17}", std::get<Number>(m_data));
                } else out.write(buffer, static_cast<std::size_t>(ptr-buffer));
            } break;
        }
        if(out.fail()) return false;
        return true;
    }


} // namespace vct::tools::json




