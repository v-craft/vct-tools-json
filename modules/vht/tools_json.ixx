export module vct.tools.json;

import std;

/**
 * @namespace vct::tools::json
 * @brief Core namespace for JSON parsing library
 * @details
 * Provides comprehensive JSON data parsing, serialization and manipulation functionality.
 * The library supports all standard JSON data types including objects, arrays, strings, numbers, booleans and null values.
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
        eObject, ///< JSON object type, unordered collection of key-value pairs
        eArray,  ///< JSON array type, ordered list of values
        eString, ///< JSON string type, Unicode character sequence enclosed in double quotes
        eNumber, ///< JSON number type, integer or floating-point value
        eBool,   ///< JSON boolean type, true or false value
        eNull,   ///< JSON null type, null value
    };

    // Forward declaration
    class Value;
    
    /**
     * @typedef Object
     * @brief JSON object type definition
     */
    using Object = std::map<std::string,Value>;
    
    /**
     * @typedef Array
     * @brief JSON array type definition
     */
    using Array = std::vector<Value>;

    /**
     * @concept json_types
     * @brief Concept defining valid JSON data types
     * 
     * Used to ensure type safety in template functions that work with JSON data.
     */
    template<typename T>
    concept json_types = std::disjunction_v<
        std::is_same<T, std::nullptr_t>,
        std::is_same<T, bool>,
        std::is_same<T, double>,
        std::is_same<T, std::string>,
        std::is_same<T, Object>,
        std::is_same<T, Array>
    >;

    /**
     * @enum ParseError
     * @brief Enumeration of possible JSON parsing errors
     * 
     * Used to indicate specific errors that can occur during JSON parsing.
     * Each error corresponds to a specific issue encountered while parsing the JSON string.
     */
    enum class ParseError {
        eDepthExceeded,     ///< Maximum depth exceeded during parsing
        eEmptyData,         ///< Empty data provided for parsing
        eUnknownFormat,     ///< Invalid JSON format encountered
        eUnclosedString,    ///< Unclosed string literal in JSON data
        eUnclosedObject,    ///< Unclosed object literal in JSON data
        eUnclosedArray,     ///< Unclosed array literal in JSON data
        eIllegalEscape,     ///< Illegal escape sequence in string literal
        eInvalidNumber,     ///< Invalid number format encountered
    };

    /**
     * @class Value
     * @brief Universal container class for JSON values
     * 
     * Used to store and manipulate JSON values of any type. Internally uses std::variant for type-safe value storage.
     * Supports all standard JSON data types: objects, arrays, strings, numbers, booleans and null.
     * 
     * @note Class size is 40 bytes, including variant data storage and type identifier
     * 
     * @details 
     * - Uses std::variant to store actual data
     * - Uses Type enum to identify current value type
     * - Default constructs to null type
     */
    class Value{
        /**
         * @typedef Variant
         * @brief Internal data storage type
         */
        using Variant = std::variant<
            std::nullptr_t, ///< JSON null value storage
            bool,           ///< JSON boolean value storage
            double,         ///< JSON number storage (unified using double)
            std::string,    ///< JSON string storage
            Object,         ///< JSON object storage
            Array           ///< JSON array storage
        >;
        
        Variant m_data { nullptr }; ///< Actual data storage, defaults to null
        Type m_type { Type::eNull };   ///< Type identifier for current value

    public:
        /**
         * @brief Clear data to nullptr and reset type to null
         */
        void reset() noexcept {
            m_data = nullptr; // Reset to null
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
        std::string type_name() const noexcept;

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
         * Constructs a Value by moving data from another Value object.
         * The source object is left in a valid but null state after the move.
         * This is more efficient than copying for large objects or arrays.
         */
        Value(Value&& jsonValue) noexcept {
            m_data = std::move(jsonValue.m_data);
            jsonValue.m_data = nullptr;
            m_type = jsonValue.m_type;
            jsonValue.m_type = Type::eNull;
        }

        /**
         * @brief Move assignment operator
         * @param jsonValue The Value object to move from
         * @return Reference to this Value object
         * 
         * Assigns by moving data from another Value object.
         * The source object is left in a valid but null state after the move.
         */
        Value& operator=(Value&& jsonValue) noexcept {
            if (this == &jsonValue) return *this;
            m_data = std::move(jsonValue.m_data);
            jsonValue.m_data = nullptr;
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
         * eString become std::string(), eObject become Object(), eArray become Array(),
         * eNumber becomes 0.0, eBool becomes false, eNull becomes nullptr.
         */
        void clear_data() noexcept;

        /**
         * @brief Construct Value from numeric types
         * @tparam T Numeric type (integral or floating-point)
         * @param value The numeric value to store
         * 
         * Template constructor for any numeric type. Converts the value to double
         * and stores it as a JSON number type. Supports both integers and floating-point numbers.
         */
        template <typename T> 
        requires (std::integral<T> || std::floating_point<T>)
        Value(T value) noexcept : m_type(Type::eNumber), m_data(static_cast<double>(value)) {}

        /**
         * @brief Assign Value from numeric types
         */
        template <typename T> 
        requires (std::integral<T> || std::floating_point<T>)
        Value& operator=(T value) noexcept {
            m_type = Type::eNumber;
            m_data = static_cast<double>(value);
            return *this;
        }

        /**
         * @brief Construct Value from string-convertible types
         * @tparam T String-convertible type
         * @param value The string value to store
         * 
         * Template constructor for any type convertible to std::string. 
         * Uses perfect forwarding to efficiently handle both lvalue and rvalue references.
         */
        template <typename T> 
        requires std::convertible_to<T, std::string>
        Value(T&& value) noexcept : m_type(Type::eString), m_data(std::string(std::forward<T>(value))) {}

        /**
         * @brief Assign Value from string-convertible types
         */
        template <typename T> 
        requires std::convertible_to<T, std::string>
        Value& operator=(T&& value) noexcept {
            m_type = Type::eString;
            m_data = std::string(std::forward<T>(value));
            return *this;
        }

        /**
         * @brief Construct Value from nullptr
         */
        Value(const std::nullptr_t) noexcept : m_type(Type::eNull), m_data(nullptr) {}
        /**
         * @brief Assign Value from nullptr
         */
        Value& operator=(const std::nullptr_t) noexcept {
            m_type = Type::eNull;
            m_data = nullptr;
            return *this;
        }
        
        /**
         * @brief Construct Value from boolean
         */
        Value(const bool value) noexcept : m_type(Type::eBool), m_data(value) {}
        /**
         * @brief Assign Value from boolean
         */
        Value& operator=(const bool value) noexcept {
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
         * @brief Serialize Value to compact JSON string
         * @return Compact JSON string without spaces and indentation
         * @note Implementation details are in tools_json.cpp
         */
        std::string serialize() const noexcept;
        
        /**
         * @brief Serialize Value to pretty-formatted JSON string
         * @param space_num Number of spaces for each indentation level (default: 2)
         * @param depth Initial indentation depth (default: 0)
         * @param max_space Maximum allowed indentation spaces to prevent memory explosion (default: 1024)
         * @return Optional pretty-formatted JSON string, nullopt if max_space exceeded
         */
        std::optional<std::string> serialize_pretty(
            const std::uint16_t space_num = 2, 
            const std::uint16_t depth = 0, 
            const std::uint32_t max_space = 1024
        ) const noexcept;

        /**
         * @brief Access object member by key (non-const)
         * @param key The key to access in the object
         * @return Reference to the Value associated with the key
         * @throws std::runtime_error if Value is not an object type
         * 
         * Provides access to object members by key. If the key doesn't exist,
         * it will be created with a null Value. Only works with object types.
         */
        [[nodiscard]]
        Value& operator[](const std::string& key) {
            if (m_type != Type::eObject) throw std::runtime_error("Value is not an object");
            return std::get<Object>(m_data)[key];
        }
        
        /**
         * @brief Access object member by key (const)
         * @param key The key to access in the object
         * @return Const reference to the Value associated with the key
         * @throws std::runtime_error if Value is not an object type
         * @throws std::out_of_range if key doesn't exist
         */
        [[nodiscard]]
        const Value& operator[](const std::string& key) const {
            if (m_type != Type::eObject) throw std::runtime_error("Value is not an object");
            return std::get<Object>(m_data).at(key);
        }
        
        /**
         * @brief Access array element by index (non-const)
         * @param index The index to access in the array
         * @return Reference to the Value at the specified index
         * @throws std::runtime_error if Value is not an array type
         * @throws std::out_of_range if index is out of bounds
         */
        [[nodiscard]]
        Value& operator[](const std::size_t index) {
            if (m_type != Type::eArray) throw std::runtime_error("Value is not an array");
            return std::get<Array>(m_data)[index];
        }
        
        /**
         * @brief Access array element by index (const)
         * @param index The index to access in the array
         * @return Const reference to the Value at the specified index
         * @throws std::runtime_error if Value is not an array type
         * @throws std::out_of_range if index is out of bounds
         */
        [[nodiscard]]
        const Value& operator[](const std::size_t index) const {
            if (m_type != Type::eArray) throw std::runtime_error("Value is not an array");
            return std::get<Array>(m_data).at(index);
        }

        /**
         * @brief Safe access to object member by key (non-const)
         * @param key The key to access in the object
         * @return Reference to the Value associated with the key
         * @throws std::runtime_error if Value is not an object type
         * @throws std::out_of_range if key doesn't exist
         */
        [[nodiscard]]
        Value& at(const std::string& key) {
            if (m_type != Type::eObject) throw std::runtime_error("Value is not an object");
            return std::get<Object>(m_data).at(key);
        }
        
        /**
         * @brief Safe access to object member by key (const)
         * @param key The key to access in the object
         * @return Const reference to the Value associated with the key
         * @throws std::runtime_error if Value is not an object type
         * @throws std::out_of_range if key doesn't exist
         */
        [[nodiscard]]
        const Value& at(const std::string& key) const {
            if (m_type != Type::eObject) throw std::runtime_error("Value is not an object");
            return std::get<Object>(m_data).at(key);
        }
        
        /**
         * @brief Safe access to array element by index (non-const)
         * @param index The index to access in the array
         * @return Reference to the Value at the specified index
         * @throws std::runtime_error if Value is not an array type
         * @throws std::out_of_range if index is out of bounds
         */
        [[nodiscard]]
        Value& at(const std::size_t index) {
            if (m_type != Type::eArray) throw std::runtime_error("Value is not an array");
            return std::get<Array>(m_data).at(index);
        }
        
        /**
         * @brief Safe access to array element by index (const)
         * @param index The index to access in the array
         * @return Const reference to the Value at the specified index
         * @throws std::runtime_error if Value is not an array type
         * @throws std::out_of_range if index is out of bounds
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
        bool contains(const std::string& key) const noexcept {
            if (m_type != Type::eObject) return false;
            return std::get<Object>(m_data).contains(key);
        }

        /**
         * @brief Check if Value matches specified type
         * @param type The JSON type to check against
         * @return True if Value's type matches the specified type, false otherwise
         */
        [[nodiscard]]
        bool is(const Type type) const noexcept {
            return m_type == type;
        }

        /**
         * @brief Get const reference to underlying data
         * @tparam T The JSON type to retrieve (must satisfy json_types concept)
         * @return Const reference to the underlying data
         * @throws std::runtime_error if type doesn't match
         * 
         * Provides direct const access to the underlying variant data.
         * Type must match exactly or std::runtime_error will be thrown.
         * Use only when you're certain of the type.
         */
        template<json_types T>
        [[nodiscard]]
        const T&  get_ref() const {
            if constexpr (std::is_same_v<T, std::nullptr_t>) {
                if (m_type == Type::eNull) return std::get<std::nullptr_t>(m_data);
            }
            if constexpr (std::is_same_v<T, Array>) {
                if (m_type == Type::eArray) return std::get<Array>(m_data);
            }
            if constexpr (std::is_same_v<T, Object>) {
                if (m_type == Type::eObject) return std::get<Object>(m_data);
            }
            if constexpr (std::is_same_v<T, std::string>) {
                if (m_type == Type::eString) return std::get<std::string>(m_data);
            }
            if constexpr (std::is_same_v<T, bool>) {
                if (m_type == Type::eBool) return std::get<bool>(m_data);
            }
            if constexpr (std::is_same_v<T, double>) {
                if (m_type == Type::eNumber) return std::get<double>(m_data);
            }
            throw std::runtime_error("Value type mismatch");
        }

        /**
         * @brief Get mutable reference to underlying data
         * @tparam T The JSON type to retrieve (must satisfy json_types concept)
         * @return Mutable reference to the underlying data
         * @throws std::runtime_error if type doesn't match
         * 
         * Provides direct mutable access to the underlying variant data.
         * Type must match exactly or std::runtime_error will be thrown.
         * Use only when you're certain of the type.
         */
        template<json_types T>
        [[nodiscard]]
        T& get_ref() {
            if constexpr (std::is_same_v<T, std::nullptr_t>) {
                if (m_type == Type::eNull) return std::get<std::nullptr_t>(m_data);
            }
            if constexpr (std::is_same_v<T, Array>) {
                if (m_type == Type::eArray) return std::get<Array>(m_data);
            }
            if constexpr (std::is_same_v<T, Object>) {
                if (m_type == Type::eObject) return std::get<Object>(m_data);
            }
            if constexpr (std::is_same_v<T, std::string>) {
                if (m_type == Type::eString) return std::get<std::string>(m_data);
            }
            if constexpr (std::is_same_v<T, bool>) {
                if (m_type == Type::eBool) return std::get<bool>(m_data);
            }
            if constexpr (std::is_same_v<T, double>) {
                if (m_type == Type::eNumber) return std::get<double>(m_data);
            }
            throw std::runtime_error("Value type mismatch");
        }

        /**
         * @brief Get value with type conversion
         * @tparam T The target type to convert to
         * @return The converted value
         * @throws std::runtime_error if conversion fails
         * 
         * Attempts to convert the stored JSON value to the requested type.
         * Supports various conversions including:
         * - Direct type matches (string->string, bool->bool, etc.)
         * - Numeric conversions (double to any arithmetic type)
         * - Enum conversions (number to enum via int cast)
         * - Implicit conversions where supported by the type system
         * 
         * Throws std::runtime_error if the current type cannot be converted to the target type.
         * For safe access without exceptions, use get_ref() with exact type matching.
         */
        template<typename T>
        T  get() const {
            if constexpr (std::is_same_v<T, std::nullptr_t>) {
                if (m_type == Type::eNull) return nullptr;
            }
            if constexpr (std::is_same_v<T, Array>) {
                if (m_type == Type::eArray) return std::get<Array>(m_data);
            }
            if constexpr (std::is_same_v<T, Object>) {
                if (m_type == Type::eObject) return std::get<Object>(m_data);
            }
            if constexpr (std::is_same_v<T, std::string>) {
                if (m_type == Type::eString) return std::get<std::string>(m_data);
            }
            if constexpr (std::is_same_v<T, bool>) {
                if (m_type == Type::eBool) return std::get<bool>(m_data);
            }
            if constexpr (std::is_arithmetic_v<T>) {
                if (m_type == Type::eNumber) return static_cast<T>(std::get<double>(m_data));
            }
            if constexpr (std::is_enum_v<T>) {
                if (m_type == Type::eNumber) return static_cast<T>(static_cast<int>(std::get<double>(m_data)));
            }
            if constexpr (std::is_convertible_v<Array, T>) {
                if (m_type == Type::eArray) return static_cast<T>(std::get<Array>(m_data));
            }
            if constexpr (std::is_convertible_v<Object, T>) {
                if (m_type == Type::eObject) return static_cast<T>(std::get<Object>(m_data));
            }
            if constexpr (std::is_convertible_v<std::string, T>) {
                if (m_type == Type::eString) return static_cast<T>(std::get<std::string>(m_data));
            }
            if constexpr (std::is_convertible_v<double, T>) {
                if (m_type == Type::eNumber) return static_cast<T>(std::get<double>(m_data));
            }
            if constexpr (std::is_convertible_v<bool, T>) {
                if (m_type == Type::eBool) return static_cast<T>(std::get<bool>(m_data));
            }
            if constexpr (std::is_convertible_v<std::nullptr_t, T>) {
                if (m_type == Type::eNull) return static_cast<T>(nullptr);
            }
            throw std::runtime_error("Value type mismatch");
        }
    
    private:
        /**
         * @brief Helper function for compact serialization
         * @param str Reference to string buffer to write output
         * @return True if serialization succeeded, false otherwise
         * 
         * Internal helper function that performs the actual compact serialization
         * by writing directly to the provided string buffer.
         */
        bool write_out(std::string& str) const noexcept;
        
        /**
         * @brief Helper function for pretty serialization
         * @param str Reference to string buffer to write output
         * @param space_num Number of spaces for each indentation level
         * @param depth Current indentation depth
         * @param max_space Maximum allowed indentation spaces
         * @return True if serialization succeeded, false if max_space exceeded
         * 
         * Internal helper function that performs the actual pretty serialization
         * with proper indentation and formatting by writing directly to the provided string buffer.
         */
        bool write_out_pretty(
            std::string& str, 
            const std::uint16_t space_num, 
            const std::uint16_t depth, 
            const std::uint32_t max_space
        ) const noexcept;

    };



    /**
     * @brief Parse JSON string into Value object
     * @param text The JSON string to parse
     * @param max_depth Maximum allowed nesting depth for JSON objects/arrays (default: 512)
     * @return std::expected<Value, ParseError> Parsed Value object or error code
     */
    std::expected<Value, ParseError> deserialize(const std::string_view text, const std::int32_t max_depth = 512 ) noexcept;

    /**
     * @brief Parse JSON string into Value object
     * @note Equivalent to deserialize functio
     * @param text The JSON string to parse
     * @param max_depth Maximum allowed nesting depth for JSON objects/arrays (default: 512)
     * @return std::expected<Value, ParseError> Parsed Value object or error code
     */
    std::expected<Value, ParseError> parse(const std::string_view text, const std::int32_t max_depth = 512 ) noexcept {
        return deserialize(text, max_depth);
    }

}
