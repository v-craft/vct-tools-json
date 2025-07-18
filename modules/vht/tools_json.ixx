export module vct.tools.json;

import std;

/**
 * @namespace vct::tools::json
 * @brief Core namespace for JSON parsing library
 * 
 * Provides comprehensive JSON data parsing, serialization and manipulation functionality.
 * Supports all standard JSON data types: objects, arrays, strings, numbers, booleans and null.
 * 
 * Default use std::map
 * If you want to use std::unordered_map instead, define M_VCT_TOOLS_JSON_UNORDERED_MAP by CMakeLists.txt.
 * 
 * @author Mysvac
 * @date 2025-07-17
 * @version 1.0
 */
export namespace vct::tools::json{

    /**
     * @enum Type
     * @brief JSON data type enumeration
     */
    enum class Type{
        eNull = 0,  ///< JSON null type, null value
        eBool,      ///< JSON boolean type, true or false value
        eNumber,    ///< JSON number type, integer or floating-point value
        eString,    ///< JSON string type, Unicode character sequence enclosed in double quotes
        eArray,     ///< JSON array type, ordered list of values
        eObject     ///< JSON object type, unordered collection of key-value pairs
    };

    /**
     * @enum ParseError
     * @brief Enumeration of possible JSON parsing errors
     */
    enum class ParseError {
        eNone = 0,          ///< No error occurred
        eEmptyData,         ///< Empty data provided for parsing
        eDepthExceeded,     ///< Maximum depth exceeded during parsing
        eIllegalEscape,     ///< Illegal escape sequence in string literal
        eInvalidNumber,     ///< Invalid number format encountered
        eUnclosedString,    ///< Unclosed string literal in JSON data
        eUnclosedObject,    ///< Unclosed object literal in JSON data
        eUnclosedArray,     ///< Unclosed array literal in JSON data
        eUnknownFormat      ///< Invalid JSON format encountered
    };


    // Forward declaration
    class Value;
    
    /**
     * @typedef String
     * @brief JSON string type definition, using std::string
     */
    using String = std::string;
    static_assert(std::is_same_v<String, std::string>, "String type must be std::string");

#ifndef M_VCT_TOOLS_JSON_UNORDERED_MAP
    // default to std::map

    /**
     * @typedef Object
     * @brief JSON object type definition, using std::map for key-value pairs
     */
    using Object = std::map<String,Value>;
    static_assert(std::is_same_v<Object, std::map<String,Value>>, "Object type must be std::map<String,Value>");

#else // define M_VCT_TOOLS_JSON_UNORDERED_MAP to use std::unordered_map

    /**
     * @typedef Object
     * @brief JSON object type definition, using std::unordered_map for key-value pairs
     */
    using Object = std::unordered_map<String,Value>;
    static_assert(std::is_same_v<Object, std::unordered_map<String,Value>>, "Object type must be std::unordered_map<String,Value>");

#endif  // M_VCT_TOOLS_JSON_UNORDERED_MAP

    /**
     * @typedef Array
     * @brief JSON array type definition, using std::vector
     */
    using Array = std::vector<Value>;
    static_assert(std::is_same_v<Array, std::vector<Value>>, "Array type must be std::vector");

    /**
     * @typedef Number
     * @brief JSON number type definition, using double for precision
     */
    using Number = double;
    static_assert(std::is_same_v<Number, double>, "Number type must be double");

    /**
     * @typedef Bool
     * @brief JSON boolean type definition, using bool
     */
    using Bool = bool;
    static_assert(std::is_same_v<Bool, bool>, "Bool type must be bool");

    /**
     * @typedef Null
     * @brief JSON null type definition, using std::nullptr_t
     */
    using Null = std::nullptr_t;
    static_assert(std::is_same_v<Null, std::nullptr_t>, "Null type must be std::nullptr_t");

    /**
     * @brief Concept to constrain template types to valid JSON types
     */
    template<typename T>
    concept json_types = std::disjunction_v<
        std::is_same<T, Null>,
        std::is_same<T, Bool>,
        std::is_same<T, Number>,
        std::is_same<T, String>,
        std::is_same<T, Array>,
        std::is_same<T, Object>
    >;

    /**
     * @class Value
     * @brief Universal container class for JSON values
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
            Number,     ///< JSON number storage (unified using Number)
            String,     ///< JSON string storage
            Array,      ///< JSON array storage
            Object      ///< JSON object storage
        >;
        
        Variant m_data { Null{} };      ///< Actual data storage, defaults to null
        Type m_type { Type::eNull };     ///< Type identifier for current value

    public:
        /**
         * @brief Clear data to nullptr and reset type to null
         */
        void reset() noexcept {
            m_data = Null{}; // Reset to null
            m_type = Type::eNull; // Reset type
        }

        /**
         * @brief Get the current JSON type
         * @return The current Type of this Value
         */
        [[nodiscard]]
        Type type() const noexcept { return m_type; }

        /**
         * @brief Get the current JSON type name
         * @return The current Type Name of this Value
         */
        [[nodiscard]]
        String type_name() const noexcept {
            switch (m_type) {
                case Type::eObject: return "Object";
                case Type::eArray:  return "Array";
                case Type::eString: return "String";
                case Type::eNumber: return "Number";
                case Type::eBool:   return "Bool";
                case Type::eNull:   return "Null";
            }
        }

        /**
         * @brief Default constructor, initializes to null type and null data
         */
        Value() = default;
        
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
         * @param other The Value object to copy from
         * @return Reference to this Value object
         */
        Value& operator=(const Value&) = default;

        /**
         * @brief Move constructor
         * @param jsonValue The Value object to move from
         * 
         * Moves data from another Value object. Source object becomes null.
         */
        Value(Value&& jsonValue) noexcept {
            m_data = std::move(jsonValue.m_data);
            jsonValue.m_data = Null{};
            m_type = jsonValue.m_type;
            jsonValue.m_type = Type::eNull;
        }

        /**
         * @brief Move assignment operator
         * @param jsonValue The Value object to move from
         * @return Reference to this Value object
         * 
         * Moves data from another Value object. Source object becomes null.
         */
        Value& operator=(Value&& jsonValue) noexcept {
            if (this == &jsonValue) return *this;
            m_data = std::move(jsonValue.m_data);
            jsonValue.m_data = Null{};
            m_type = jsonValue.m_type;
            jsonValue.m_type = Type::eNull;
            return *this;
        }

        /**
         * @brief Construct Value with specified JSON type
         * @param type The JSON type to initialize
         */
        Value(const Type type) noexcept : m_type(type) { clear_data();}
        
        /**
         * @brief Assign Value with specified JSON type
        * @param type The JSON type to initialize
         */
        Value& operator=(const Type type) noexcept{
            m_type = type;
            clear_data();
            return *this;
        }

        /**
         * @brief Clear data only without changing type
         * 
         * Resets data to default values: string->string(), number->0.0, bool->false, null->nullptr, etc.
         */
        void clear_data() noexcept {
            switch (m_type) {
                case Type::eObject: m_data = Object();  break;
                case Type::eArray:  m_data = Array();   break;
                case Type::eString: m_data = String();  break;
                case Type::eNumber: m_data = Number();  break;
                case Type::eBool:   m_data = Bool();    break;
                case Type::eNull:   m_data = Null();    break;
            }
        }

        /**
         * @brief Construct Value from numeric types
         * @tparam T Numeric type (integral or floating-point)
         * @param value The numeric value to store
         * 
         * Converts any numeric type to double and stores as JSON number.
         */
        template <typename T> 
        requires (std::integral<T> || std::floating_point<T>)
        Value(T value) noexcept : m_type(Type::eNumber), m_data(static_cast<Number>(value)) {}

        /**
         * @brief Assign Value from numeric types
         */
        template <typename T> 
        requires (std::integral<T> || std::floating_point<T>)
        Value& operator=(T value) noexcept {
            m_type = Type::eNumber;
            m_data = static_cast<Number>(value);
            return *this;
        }

        /**
         * @brief Construct Value from string-convertible types
         * @tparam T String-convertible type
         * @param value The string value to store
         * 
         * Uses perfect forwarding for efficient string handling.
         */
        template <typename T> 
        requires std::convertible_to<T, String>
        Value(T&& value) noexcept : m_type(Type::eString), m_data(String(std::forward<T>(value))) {}

        /**
         * @brief Assign Value from string-convertible types
         */
        template <typename T> 
        requires std::convertible_to<T, String>
        Value& operator=(T&& value) noexcept {
            m_type = Type::eString;
            m_data = String(std::forward<T>(value));
            return *this;
        }

        /**
         * @brief Construct Value from nullptr
         */
        Value(const Null) noexcept : m_type(Type::eNull), m_data(Null{}) {}
        /**
         * @brief Assign Value from nullptr
         */
        Value& operator=(const Null) noexcept {
            m_type = Type::eNull;
            m_data = Null{};
            return *this;
        }
        
        /**
         * @brief Construct Value from boolean
         */
        Value(const Bool value) noexcept : m_type(Type::eBool), m_data(value) {}
        /**
         * @brief Assign Value from boolean
         */
        Value& operator=(const Bool value) noexcept {
            m_type = Type::eBool;
            m_data = value;
            return *this;
        }
        
        /**
         * @brief Construct Value from Object (copy)
         */
        Value(const Object& value) noexcept : m_type(Type::eObject), m_data(value) {}
        /**
         * @brief Assign Value from Object (copy)
         */
        Value& operator=(const Object& value) noexcept {
            m_type = Type::eObject;
            m_data = value;
            return *this;
        }
        
        /**
         * @brief Construct Value from Object (move)
         */
        Value(Object&& value) noexcept : m_type(Type::eObject), m_data(std::move(value)) {}
        /**
         * @brief Assign Value from Object (move)
         */
        Value& operator=(Object&& value) noexcept {
            m_type = Type::eObject;
            m_data = std::move(value);
            return *this;
        }
        
        /**
         * @brief Construct Value from Array (copy)
         */
        Value(const Array& value) noexcept : m_type(Type::eArray), m_data(value) {}
        /**
         * @brief Assign Value from Array (copy)
         */
        Value& operator=(const Array& value) noexcept {
            m_type = Type::eArray;
            m_data = value;
            return *this;
        }
        
        /**
         * @brief Construct Value from Array (move)
         */
        Value(Array&& value) noexcept : m_type(Type::eArray), m_data(std::move(value)) {}
        /**
         * @brief Assign Value from Array (move)
         */
        Value& operator=(Array&& value) noexcept {
            m_type = Type::eArray;
            m_data = std::move(value);
            return *this;
        }

        /**
         * @brief Access object member by key (non-const)
         * @param key The key to access
         * @return Reference to Value, creates if not exists
         * @throws std::runtime_error if not an object
         */
        [[nodiscard]]
        Value& operator[](const String& key) {
            if (m_type != Type::eObject) throw std::runtime_error("Value is not an object");
            return std::get<Object>(m_data)[key];
        }
        
        /**
         * @brief Access object member by key (const)
         * @param key The key to access
         * @return Const reference to Value
         * @throws std::runtime_error if not an object
         * @throws std::out_of_range if key doesn't exist
         */
        [[nodiscard]]
        const Value& operator[](const String& key) const {
            if (m_type != Type::eObject) throw std::runtime_error("Value is not an object");
            return std::get<Object>(m_data).at(key);
        }
        
        /**
         * @brief Access array element by index (non-const)
         * @param index The index to access
         * @return Reference to Value at index
         * @throws std::runtime_error if not an array
         * @throws std::out_of_range if index out of bounds
         */
        [[nodiscard]]
        Value& operator[](const std::size_t index) {
            if (m_type != Type::eArray) throw std::runtime_error("Value is not an array");
            return std::get<Array>(m_data)[index];
        }
        
        /**
         * @brief Access array element by index (const)
         * @param index The index to access
         * @return Const reference to Value at index
         * @throws std::runtime_error if not an array
         * @throws std::out_of_range if index out of bounds
         */
        [[nodiscard]]
        const Value& operator[](const std::size_t index) const {
            if (m_type != Type::eArray) throw std::runtime_error("Value is not an array");
            return std::get<Array>(m_data).at(index);
        }

        /**
         * @brief Safe access to object member by key
         * @param key The key to access
         * @return Reference to Value
         * @throws std::runtime_error if not an object
         * @throws std::out_of_range if key doesn't exist
         */
        [[nodiscard]]
        Value& at(const String& key) {
            if (m_type != Type::eObject) throw std::runtime_error("Value is not an object");
            return std::get<Object>(m_data).at(key);
        }
        
        /**
         * @brief Safe access to object member by key (const)
         */
        [[nodiscard]]
        const Value& at(const String& key) const {
            if (m_type != Type::eObject) throw std::runtime_error("Value is not an object");
            return std::get<Object>(m_data).at(key);
        }
        
        /**
         * @brief Safe access to array element by index
         * @param index The index to access
         * @return Reference to Value at index
         * @throws std::runtime_error if not an array
         * @throws std::out_of_range if index out of bounds
         */
        [[nodiscard]]
        Value& at(const std::size_t index) {
            if (m_type != Type::eArray) throw std::runtime_error("Value is not an array");
            return std::get<Array>(m_data).at(index);
        }
        
        /**
         * @brief Safe access to array element by index (const)
         */
        [[nodiscard]]
        const Value& at(const std::size_t index) const {
            if (m_type != Type::eArray) throw std::runtime_error("Value is not an array");
            return std::get<Array>(m_data).at(index);
        }

        /**
         * @brief Get the size of container types
         * @return Size of the container, or 0 for non-container types (including string)
         */
        [[nodiscard]]
        const std::size_t size() const noexcept {
            switch (m_type) {
                case Type::eObject: return std::get<Object>(m_data).size();
                case Type::eArray: return std::get<Array>(m_data).size();
                default: return 0;
            }
        }
        
        /**
         * @brief Check if object contains a key
         * @param key The key to check for
         * @return True if the type is object and key exists in object, false otherwise
         */
        [[nodiscard]]
        Bool contains(const String& key) const noexcept {
            if (m_type != Type::eObject) return false;
            return std::get<Object>(m_data).contains(key);
        }

        /**
         * @brief Check if Value matches specified type
         * @param type The JSON type to check against
         * @return True if Value's type matches the specified type, false otherwise
         */
        [[nodiscard]]
        Bool is(const Type type) const noexcept {
            return m_type == type;
        }

        /**
         * @brief Get const reference to underlying data
         * @tparam T The JSON type to retrieve (must satisfy json_types concept)
         * @return Const reference to underlying data
         * @throws std::runtime_error if type doesn't match
         */
        template<json_types T>
        [[nodiscard]]
        const T&  get_ref() const {
            if constexpr (std::is_same_v<T, Null>) {
                if (m_type == Type::eNull) return std::get<Null>(m_data);
            }
            if constexpr (std::is_same_v<T, Bool>) {
                if (m_type == Type::eBool) return std::get<Bool>(m_data);
            }
            if constexpr (std::is_same_v<T, Number>) {
                if (m_type == Type::eNumber) return std::get<Number>(m_data);
            }
            if constexpr (std::is_same_v<T, String>) {
                if (m_type == Type::eString) return std::get<String>(m_data);
            }
            if constexpr (std::is_same_v<T, Array>) {
                if (m_type == Type::eArray) return std::get<Array>(m_data);
            }
            if constexpr (std::is_same_v<T, Object>) {
                if (m_type == Type::eObject) return std::get<Object>(m_data);
            }
            throw std::runtime_error("Value type mismatch");
        }

        /**
         * @brief Get mutable reference to underlying data
         * @tparam T The JSON type to retrieve (must satisfy json_types concept)
         * @return Mutable reference to underlying data
         * @throws std::runtime_error if type doesn't match
         */
        template<json_types T>
        [[nodiscard]]
        T& get_ref() {
            if constexpr (std::is_same_v<T, Null>) {
                if (m_type == Type::eNull) return std::get<Null>(m_data);
            }
            if constexpr (std::is_same_v<T, Bool>) {
                if (m_type == Type::eBool) return std::get<Bool>(m_data);
            }
            if constexpr (std::is_same_v<T, Number>) {
                if (m_type == Type::eNumber) return std::get<Number>(m_data);
            }
            if constexpr (std::is_same_v<T, String>) {
                if (m_type == Type::eString) return std::get<String>(m_data);
            }
            if constexpr (std::is_same_v<T, Array>) {
                if (m_type == Type::eArray) return std::get<Array>(m_data);
            }
            if constexpr (std::is_same_v<T, Object>) {
                if (m_type == Type::eObject) return std::get<Object>(m_data);
            }
            throw std::runtime_error("Value type mismatch");
        }

        /**
         * @brief Get (Copy) value with type conversion
         * @tparam T The target type to convert to
         * @return The converted value
         * @throws std::runtime_error if conversion fails
         * 
         * Supports various conversions including direct type matches, 
         * numeric conversions, and enum conversions.
         * 
         * @note Number is double, so conversions to integral (and enum) types will round to nearest.
         * @details
         * priority of conversions:
         * - Null -> Null
         * - Object -> Object
         * - Array -> Array
         * - String -> String
         * - Bool -> Bool
         * - Number -> enum types (us llround, round to nearest)
         * - Number -> integral types (us llround, round to nearest)
         * - Number -> floating_point types
         * - Object -> convertible types
         * - String -> convertible types
         * - Array -> convertible types
         * - Number -> convertible types
         * - Bool -> convertible types
         * - Null -> convertible types
         */
        template<typename T>
        requires json_types<T> || std::disjunction_v<
            std::is_arithmetic<T>,
            std::is_enum<T>,
            std::is_convertible<Array, T>,
            std::is_convertible<Object, T>,
            std::is_convertible<String, T>,
            std::is_convertible<Number, T>,
            std::is_convertible<Bool, T>,
            std::is_convertible<Null, T>
        > [[nodiscard]]
        T  get() const {
            if constexpr (std::is_same_v<T, Null>) {
                if (m_type == Type::eNull) return Null();
            }
            if constexpr (std::is_same_v<T, Object>) {
                if (m_type == Type::eObject) return std::get<Object>(m_data);
            }
            if constexpr (std::is_same_v<T, Array>) {
                if (m_type == Type::eArray) return std::get<Array>(m_data);
            }
            if constexpr (std::is_same_v<T, String>) {
                if (m_type == Type::eString) return std::get<String>(m_data);
            }
            if constexpr (std::is_same_v<T, Bool>) {
                if (m_type == Type::eBool) return std::get<Bool>(m_data);
            }
            if constexpr (std::is_enum_v<T>) {
                if (m_type == Type::eNumber) return static_cast<T>(std::llround(std::get<Number>(m_data)));
            }
            if constexpr (std::is_integral_v<T>) {
                if (m_type == Type::eNumber) return static_cast<T>(std::llround(std::get<Number>(m_data)));
            }
            if constexpr (std::is_floating_point_v<T>) {
                if (m_type == Type::eNumber) return static_cast<T>(std::get<Number>(m_data));
            }
            if constexpr (std::is_convertible_v<Object, T>) {
                if (m_type == Type::eObject) return static_cast<T>(std::get<Object>(m_data));
            }
            if constexpr (std::is_convertible_v<String, T>) {
                if (m_type == Type::eString) return static_cast<T>(std::get<String>(m_data));
            }
            if constexpr (std::is_convertible_v<Array, T>) {
                if (m_type == Type::eArray) return static_cast<T>(std::get<Array>(m_data));
            }
            if constexpr (std::is_convertible_v<Number, T>) {
                if (m_type == Type::eNumber) return static_cast<T>(std::get<Number>(m_data));
            }
            if constexpr (std::is_convertible_v<Bool, T>) {
                if (m_type == Type::eBool) return static_cast<T>(std::get<Bool>(m_data));
            }
            if constexpr (std::is_convertible_v<Null, T>) {
                if (m_type == Type::eNull) return static_cast<T>(nullptr);
            }
            throw std::runtime_error("Value type mismatch");
        }


        /**
         * @brief For Value-to-Value accurate comparison
         * @tparam T Type that satisfies std::equality_comparable concept
         * @param other The value to compare with
         * @return True if values are equal, false otherwise
         */
        bool operator==(const Value& other) const noexcept {
            if (m_type != other.m_type) return false; // Different types cannot be equal
            switch (m_type) {
                case Type::eNull: return true; // Both are null
                case Type::eBool: return std::get<Bool>(m_data) == std::get<Bool>(other.m_data);
                case Type::eNumber: return std::get<Number>(m_data) == std::get<Number>(other.m_data);
                case Type::eString: return std::get<String>(m_data) == std::get<String>(other.m_data);
                case Type::eObject: return std::get<Object>(m_data) == std::get<Object>(other.m_data);
                case Type::eArray: return std::get<Array>(m_data) == std::get<Array>(other.m_data);
            }
        }

        /**
         * @brief Generic equality comparison operator
         * @tparam T Type that satisfies std::equality_comparable concept
         * @param other The value to compare with
         * @return True if values are equal, false otherwise
         * 
         * @details
         * rules:
         * 0. IF T is Value, use Value::operator==(const Value& other) , it's
         *     Accurate comparison !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
         * --------------------------------------------------------------------------------------------
         * 1.  IF T is Null, return true if this Value is null, false otherwise
         * 2.  Else if T is Bool, return true if Value is Bool or Number ans Equal to Tvalue
         * 3.  Else if T is Number, return true if Value is Bool or Number and Equal to Tvalue
         * 4.  Else if T is String, return true if Value is String and Equal to Tvalue
         * 5.  Else if T is Array, return true if Value is Array and Equal to Tvalue
         * 6.  Else if T is Object, return true if Value is Object and Equal to Tvalue
         * 7.  Else if T is enum, return true if Value is Number or Bool and Equal to Tvalue
         * 8.  Else if T is integral, return true if Value is Number or Bool and Equal to Tvalue
         * 9.  Else if T is floating_point, return true if Value is Number and Equal to Tvalue
         * 10. Else if T is convertible to std::string_view, return true if Value is String and Equal to std::string_view( Tvalue )
         * 11. Else return false
         */
        template<typename  T>
        requires (!std::is_same_v<T, Value> && std::equality_comparable<T>)
        bool operator==(const T& other) const noexcept {
            if constexpr ( std::is_same_v<T,Null> ) {
                return m_type == Type::eNull;
            } else if constexpr ( std::is_same_v<T,Bool> ) {
                if ( m_type == Type::eBool ) return std::get<Bool>(m_data) == other;
                if ( m_type == Type::eNumber ) return static_cast<Bool>(std::get<Number>(m_data)) == other;
            } else if constexpr ( std::is_same_v<T,Number> ) {
                if (m_type == Type::eBool) return static_cast<T>(std::get<Bool>(m_data)) == other;
                if ( m_type == Type::eNumber ) return std::get<Number>(m_data) == other;
            } else if constexpr ( std::is_same_v<T,String> ) {
                if ( m_type == Type::eString ) return std::get<String>(m_data) == other;
            } else if constexpr ( std::is_same_v<T,Array> ) {
                if ( m_type == Type::eArray ) return std::get<Array>(m_data) == other;
            } else if constexpr ( std::is_same_v<T,Object> ) {
                if ( m_type == Type::eObject ) return std::get<Object>(m_data) == other;
            } else if constexpr (std::is_enum_v<T>) {
                if (m_type == Type::eNumber) return static_cast<T>(std::llround(std::get<Number>(m_data))) == other;
                if (m_type == Type::eBool) return static_cast<T>(std::get<Bool>(m_data)) == other;
            } else if constexpr (std::is_integral_v<T>) {
                if (m_type == Type::eNumber) return static_cast<T>(std::llround(std::get<Number>(m_data))) == other;
                if (m_type == Type::eBool) return static_cast<T>(std::get<Bool>(m_data)) == other;
            } else if constexpr (std::is_floating_point_v<T>) {
                if (m_type == Type::eNumber) return static_cast<T>(std::get<Number>(m_data)) == other;
                if (m_type == Type::eBool) return static_cast<T>(std::get<Bool>(m_data)) == other;
            } else if constexpr (std::is_convertible_v<T, std::string_view>) {
                if(m_type == Type::eString) return std::get<String>(m_data) == std::string_view( other );
            } 
            return false;
        }
        static_assert( std::is_convertible_v<Bool, Number>, "Bool must be convertible to Number" );
    

        /**
         * @brief Serialize Value to string buffer back
         * @param str Reference to string buffer to write output
         */
        void serialize_to(String& str) const noexcept;
        
        /**
         * @brief Serialize Value to output stream
         * @param os Output stream to write JSON data
         */
        void serialize_to(std::ostream& os) const noexcept;

        /**
         * @brief Serialize Value to compact JSON string
         * @return Compact JSON string without formatting
         */
        String serialize() const noexcept {
            String res;
            serialize_to(res);
            return res;
        }

        /**
         * @brief Serialize Value to pretty-formatted string buffer
         * @param str Reference to string buffer to write output 
         * @param space_num Number of spaces per indentation level (default: 2)
         * @param depth Current indentation depth (default: 0)
         * @param max_space Maximum allowed indentation spaces (default: 512)
         * @return True if serialization succeeded, false if max_space exceeded
         */
        Bool serialize_pretty_to(
            String& str, 
            const std::uint16_t space_num = 2, 
            const std::uint16_t depth = 0, 
            const std::uint32_t max_space = 512
        ) const noexcept;

        /**
         * @brief Serialize Value to pretty-formatted output stream
         * @param out Output stream to write JSON data
         * @param space_num Number of spaces per indentation level
         * @param depth Current indentation depth
         * @param max_space Maximum allowed indentation spaces
         * @return True if serialization succeeded, false if max_space exceeded
         */
        Bool serialize_pretty_to(
            std::ostream& out, 
            const std::uint16_t space_num, 
            const std::uint16_t depth, 
            const std::uint32_t max_space
        ) const noexcept;


        /**
         * @brief Serialize Value to pretty-formatted JSON string
         * @param space_num Number of spaces per indentation level (default: 2)
         * @param depth Initial depth (default: 0)
         * @param max_space Maximum allowed spaces to prevent memory explosion (default: 512)
         * @return Formatted JSON string or nullopt if max_space exceeded
         */
        std::optional<String> serialize_pretty(
            const std::uint16_t space_num = 2, 
            const std::uint16_t depth = 0, 
            const std::uint32_t max_space = 512
        ) const noexcept {
            String res;
            if(serialize_pretty_to(res, space_num, depth, max_space)) return res;
            return std::nullopt;
        }



    };



    /**
     * @brief Parse JSON string into Value object
     * @param text The JSON string to parse
     * @param max_depth Maximum nesting depth (default: 256)
     * @return std::expected<Value, ParseError> Parsed Value or error
     */
    std::expected<Value, ParseError> deserialize(const std::string_view text, const std::int32_t max_depth = 256 ) noexcept;

    /**
     * @brief Parse JSON string from input-stream into Value object
     * @param is_test input-stream
     * @param max_depth Maximum nesting depth (default: 256)
     * @return std::expected<Value, ParseError> Parsed Value or error
     */
    std::expected<Value, ParseError> deserialize(std::istream& is_test, const std::int32_t max_depth = 256 ) noexcept;


    /**
     * @brief Parse JSON string into Value object
     * @param text The JSON string to parse
     * @param max_depth Maximum nesting depth (default: 256)
     * @return std::expected<Value, ParseError> Parsed Value or error
     */
    std::expected<Value, ParseError> parse(const std::string_view text, const std::int32_t max_depth = 256 ) noexcept {
        return deserialize(text, max_depth);
    }

    /**
     * @brief Parse JSON string from input-stream into Value object
     * @param is_test input-stream
     * @param max_depth Maximum nesting depth (default: 256)
     * @return std::expected<Value, ParseError> Parsed Value or error
     */
    std::expected<Value, ParseError> parse(std::istream& is_test, const std::int32_t max_depth = 256 ) noexcept {
        return deserialize(is_test, max_depth);
    }

}
