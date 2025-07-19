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
        eUnknownFormat,     ///< Invalid JSON format encountered
        eRedundentText      ///< Redundant text after valid JSON data
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
    static_assert( Number{} == 0.0, "Number type must be 0.0");

    /**
     * @typedef Bool
     * @brief JSON boolean type definition, using bool
     */
    using Bool = bool;
    static_assert(std::is_same_v<Bool, bool>, "Bool type must be bool");
    static_assert( Bool{} == false, "Bool type must be false");

    /**
     * @typedef Null
     * @brief JSON null type definition, using std::nullptr_t
     */
    using Null = std::nullptr_t;
    static_assert(std::is_same_v<Null, std::nullptr_t>, "Null type must be std::nullptr_t");
    static_assert( Null{} == nullptr, "Null type must be nullptr");

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
         * @brief Check if Value matches specified type
         * @param type The JSON type to check against
         * @return True if Value's type matches the specified type, false otherwise
         */
        [[nodiscard]]
        Bool is(const Type type) const noexcept { return m_type == type; }

        /**
         * @brief Get the size of container types ( Object, Array )
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
         * @param other The Value object to move from
         * 
         * Moves data from another Value object. Source object becomes null.
         */
        Value(Value&& other) noexcept {
            m_data = std::move(other.m_data);
            other.m_data = Null{};
            m_type = other.m_type;
            other.m_type = Type::eNull;
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
            m_type = other.m_type;
            other.m_type = Type::eNull;
            return *this;
        }

        /**
         * @brief Clear data only without changing type
         * 
         * Resets data to default values: string->string(), number->0.0, bool->false, null->nullptr, etc.
         */
        void clear_data() noexcept {
            switch (m_type) {
                case Type::eObject: m_data = Object{};  break;
                case Type::eArray:  m_data = Array{};   break;
                case Type::eString: m_data = String{};  break;
                case Type::eNumber: m_data = Number{};  break;
                case Type::eBool:   m_data = Bool{};    break;
                case Type::eNull:   m_data = Null{};    break;
            }
        }

        /**
         * @brief Construct Value with specified JSON type
         * @param type The JSON type to initialize
         */
        Value(const Type type) noexcept : m_type(type) { clear_data(); }
        
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
        Value(const Bool bl) noexcept : m_type(Type::eBool), m_data(bl) {}

        /**
         * @brief Assign Value from boolean
         */
        Value& operator=(const Bool bl) noexcept {
            m_type = Type::eBool;
            m_data = bl;
            return *this;
        }

        /**
         * @brief Construct Value from numeric types
         * @tparam T Numeric type (integral or floating-point or enum)
         */
        template<typename T>
        requires (std::is_arithmetic_v<T> || std::is_enum_v<T>)
        Value(T num) noexcept : m_type(Type::eNumber), m_data( static_cast<Number>(num) ) {}

        /**
         * @brief Assign Value from numeric types
         * @tparam T Numeric type (integral or floating-point or enum)
         */
        template<typename T>
        requires (std::is_arithmetic_v<T> || std::is_enum_v<T>)
        Value& operator=(T num) noexcept {
            m_type = Type::eNumber;
            m_data = static_cast<Number>(num);
            return *this;
        }

        /**
         * @brief Construct Value from string types
         */
        Value(String str) noexcept : m_type(Type::eString), m_data(std::move(str)) {}

        /**
         * @brief Construct Value from C-style string literal
         * @param str C-style string literal or const char*
         */
        Value(const char* str) noexcept : m_type(Type::eString), m_data(String{str}) {}

        /**
         * @brief Assign Value from string types
         */
        Value& operator=(String str) noexcept {
            m_type = Type::eString;
            m_data = std::move(str);
            return *this;
        }

        /**
         * @brief Assign Value from C-style string literal
         * @param str C-style string literal or const char*
         */
        Value& operator=(const char* str) noexcept {
            m_type = Type::eString;
            m_data = String{str};
            return *this;
        }
        
        /**
         * @brief Construct Value from Object
         */
        Value(Object obj) noexcept : m_type(Type::eObject), m_data(std::move(obj)) {}


        /**
         * @brief Assign Value from Object
         */
        Value& operator=(Object obj) noexcept {
            m_type = Type::eObject;
            m_data = std::move(obj);
            return *this;
        }
        
        /**
         * @brief Construct Value from Array
         */
        Value(Array arr) noexcept : m_type(Type::eArray), m_data(std::move(arr)) {}
        /**
         * @brief Assign Value from Array
         */
        Value& operator=(Array arr) noexcept {
            m_type = Type::eArray;
            m_data = std::move(arr);
            return *this;
        }
    
        /**
         * @brief Type conversion operator to convert Value to numeric types
         * @tparam T Numeric type (integral or floating-point or enum)
         * @note 
         * if type is integer or enum, it will round the number to nearest integer.  
         * if json type is not number, it will return T{} (default value for T).
         */
        template<typename T>
        requires (std::is_arithmetic_v<T> || std::is_enum_v<T>)
        [[nodiscard]]
        explicit operator T() const noexcept {
            if(m_type != Type::eNumber)  return T{};
            if constexpr (std::is_enum_v<T>) {
                return static_cast<T>(std::llround(std::get<Number>(m_data)));
            } else if constexpr (std::is_integral_v<T>) {
                return static_cast<T>(std::llround(std::get<Number>(m_data)));
            } else {
                return static_cast<T>(std::get<Number>(m_data));
            }
        }

        /**
         * @brief Type conversion operators to convert Value to String ( std::string )
         * @note if json type is not string, it will return empty string.
         */
        [[nodiscard]]
        explicit operator String() const noexcept {
            if (m_type != Type::eString) return String{};
            return std::get<String>(m_data);
        }

        /**
         * @brief Type conversion operator to convert Value to Bool ( bool )
         * @note if json type is not bool, it will return false.
         */
        [[nodiscard]]
        explicit operator Bool() const noexcept {
            if (m_type != Type::eBool) return Bool{};
            return std::get<Bool>(m_data);
        }

        /**
         * @brief Type conversion operator to convert Value to Null ( std::nullptr_t )
         * @note return nullptr always
         */
        [[nodiscard]]
        explicit operator Null() const noexcept {
            return Null{};
        }

        /**
         * @brief Type conversion operator to convert Value to Object ( std::map<String, Value> )
         * @note if json type is not object, it will return empty object.
         */
         [[nodiscard]]
        explicit operator Object() const noexcept {
            if (m_type != Type::eObject) return Object{};
            return std::get<Object>(m_data);
        }

        /**
         * @brief Type conversion operator to convert Value to Array ( std::vector<Value> )
         * @note if json type is not array, it will return empty array.
         */
        [[nodiscard]]
        explicit operator Array() const noexcept {
            if (m_type != Type::eArray) return Array{};
            return std::get<Array>(m_data);
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
         * @warning if index out of bounds, application will crash or occur other undefined behavior 
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
         * @param key The key to access
         * @return Const reference to Value
         * @throws std::runtime_error if not an object
         * @throws std::out_of_range if key doesn't exist
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
         * @param index The index to access
         * @return Const reference to Value at index
         * @throws std::runtime_error if not an array
         * @throws std::out_of_range if index out of bounds
         */
        [[nodiscard]]
        const Value& at(const std::size_t index) const {
            if (m_type != Type::eArray) throw std::runtime_error("Value is not an array");
            return std::get<Array>(m_data).at(index);
        }

        /**
         * @brief Serialize Value to string buffer back ( Must succeed )
         * @param str Reference to string buffer to write output
         */
        void serialize_to(String& str) const noexcept;
        
        /**
         * @brief Serialize Value to output stream
         * @param os Output stream to write JSON data
         * @note 
         * output to stream may be failed If an unknown problem occurs (e.g. stream is closed or not writable)
         * when stream.fail() return true, this function will qucikly return but not throw any exception.
         */
        void serialize_to(std::ostream& os) const noexcept;

        /**
         * @brief Serialize Value to compact JSON string ( Must succeed )
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
         * @return True if serialization succeeded, false if max_space exceeded or stream failed
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

        /**
         * @brief Get const reference to underlying data
         * @tparam T The JSON type to retrieve (must satisfy json_types concept)
         * @return Const reference to underlying data
         * @throws std::runtime_error if type doesn't match
         */
        template<json_types T>
        [[nodiscard]]
        const T&  get() const {
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
        T& get() {
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
         * @brief type conversion, copy inner value to specified type
         * @tparam T The target type to convert to
         * @return The converted value
         * @throws std::runtime_error if conversion fails
         * @note Number is double, so conversions to integral (and enum) types will round to nearest.
         * @warning 
         * The type conversion function of `json::Value` supports `enum`, `arithmetic`, and `json types`, and can return default values even if they are incompatible.
         * However, **the 'to' function does not return a default value when completely incompatible**, but instead chooses to throw an exception. 
         * If you need default values, please use to_or.
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
         * 9. Object -> convertible types
         * 10. Array -> convertible types
         * 11. String -> convertible types
         * 12. Number -> convertible types
         * 13. Bool -> convertible types
         * 14. Null -> convertible types
         * 15. Any -> Value convertible types, but convertible type is not `json_types` or `is_arithmetic` or `is_enum`
         * 16. throw std::runtime_error
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
            std::is_convertible<Null, T>,
            std::is_convertible<Value, T>
        > [[nodiscard]]
        T  to() const {
            if constexpr (std::is_same_v<T, Null>) {
                if (m_type == Type::eNull) return Null{};
            } else if constexpr (std::is_same_v<T, Object>) {
                if (m_type == Type::eObject) return std::get<Object>(m_data);
            } else if constexpr (std::is_same_v<T, Array>) {
                if (m_type == Type::eArray) return std::get<Array>(m_data);
            } else if constexpr (std::is_same_v<T, String>) {
                if (m_type == Type::eString) return std::get<String>(m_data);
            } else if constexpr (std::is_same_v<T, Bool>) {
                if (m_type == Type::eBool) return std::get<Bool>(m_data);
            } else if constexpr (std::is_enum_v<T>) {
                if (m_type == Type::eNumber) return static_cast<T>(std::llround(std::get<Number>(m_data)));
            } else if constexpr (std::is_integral_v<T>) {
                if (m_type == Type::eNumber) return static_cast<T>(std::llround(std::get<Number>(m_data)));
            } else if constexpr (std::is_floating_point_v<T>) {
                if (m_type == Type::eNumber) return static_cast<T>(std::get<Number>(m_data));
            }
            if constexpr (std::is_convertible_v<Object, T>) {
                if (m_type == Type::eObject) return static_cast<T>(std::get<Object>(m_data));
            }
            if constexpr (std::is_convertible_v<Array, T>) {
                if (m_type == Type::eArray) return static_cast<T>(std::get<Array>(m_data));
            }
            if constexpr (std::is_convertible_v<String, T>) {
                if (m_type == Type::eString) return static_cast<T>(std::get<String>(m_data));
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
            if constexpr (!json_types<T> && !std::is_arithmetic_v<T> && !std::is_enum_v<T> && std::is_convertible_v<Value, T>) {
                // try convert Value to T, but T is not json_types or is_arithmetic or is_enum
                // Because these types have already been determined above
                // Value supports default values for these types of conversion functions, which can interfere with the output results
                return static_cast<T>(*this);
            }
            throw std::runtime_error("Value type mismatch");
        }

        /**
         * @brief type conversion, copy inner value to specified type
         * @tparam T The target type to convert to
         * @return The converted value
         * @note Number is double, so conversions to integral (and enum) types will round to nearest.
         * @warning 
         * The type conversion function of `json::Value` supports `enum`, `arithmetic`, and `json types`, and can return default values even if they are incompatible.
         * However, **the 'to' function does not return a default value when completely incompatible**, but instead chooses to throw an exception. 
         * If you need default values, please use to_or.
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
         * 9. Object -> convertible types
         * 10. Array -> convertible types
         * 11. String -> convertible types
         * 12. Number -> convertible types
         * 13. Bool -> convertible types
         * 14. Null -> convertible types
         * 15. Any -> Value convertible types, but convertible type is not `json_types` or `is_arithmetic` or `is_enum`
         * 16. return std::nullopt;
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
            std::is_convertible<Null, T>,
            std::is_convertible<Value, T>
        > [[nodiscard]]
        std::optional<T>  to_if() const noexcept {
            if constexpr (std::is_same_v<T, Null>) {
                if (m_type == Type::eNull) return Null{};
            } else if constexpr (std::is_same_v<T, Object>) {
                if (m_type == Type::eObject) return std::get<Object>(m_data);
            } else if constexpr (std::is_same_v<T, Array>) {
                if (m_type == Type::eArray) return std::get<Array>(m_data);
            } else if constexpr (std::is_same_v<T, String>) {
                if (m_type == Type::eString) return std::get<String>(m_data);
            } else if constexpr (std::is_same_v<T, Bool>) {
                if (m_type == Type::eBool) return std::get<Bool>(m_data);
            } else if constexpr (std::is_enum_v<T>) {
                if (m_type == Type::eNumber) return static_cast<T>(std::llround(std::get<Number>(m_data)));
            } else if constexpr (std::is_integral_v<T>) {
                if (m_type == Type::eNumber) return static_cast<T>(std::llround(std::get<Number>(m_data)));
            } else if constexpr (std::is_floating_point_v<T>) {
                if (m_type == Type::eNumber) return static_cast<T>(std::get<Number>(m_data));
            }
            if constexpr (std::is_convertible_v<Object, T>) {
                if (m_type == Type::eObject) return static_cast<T>(std::get<Object>(m_data));
            }
            if constexpr (std::is_convertible_v<Array, T>) {
                if (m_type == Type::eArray) return static_cast<T>(std::get<Array>(m_data));
            }
            if constexpr (std::is_convertible_v<String, T>) {
                if (m_type == Type::eString) return static_cast<T>(std::get<String>(m_data));
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
            if constexpr (!json_types<T> && !std::is_arithmetic_v<T> && !std::is_enum_v<T> && std::is_convertible_v<Value, T>) {
                // try convert Value to T, but T is not json_types or is_arithmetic or is_enum
                // Because these types have already been determined above
                // Value supports default values for these types of conversion functions, which can interfere with the output results
                return static_cast<T>(*this);
            }
            return std::nullopt; // return nullopt if conversion fails
        }


        /**
         * @brief type conversion, copy inner value to specified type
         * @tparam T The target type to convert to
         * @return The converted value or default_value if conversion fails
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
         * 9. Object -> convertible types
         * 10. Array -> convertible types
         * 11. String -> convertible types
         * 12. Number -> convertible types
         * 13. Bool -> convertible types
         * 14. Null -> convertible types
         * 15. Any -> Value convertible types, but convertible type is not `json_types` or `is_arithmetic` or `is_enum`
         * 16. return default_value 
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
            std::is_convertible<Null, T>,
            std::is_convertible<Value, T>
        > [[nodiscard]]
        T  to_or( T default_value ) const noexcept {
            if constexpr (std::is_same_v<T, Null>) {
                if (m_type == Type::eNull) return Null{};
            } else if constexpr (std::is_same_v<T, Object>) {
                if (m_type == Type::eObject) return std::get<Object>(m_data);
            } else if constexpr (std::is_same_v<T, Array>) {
                if (m_type == Type::eArray) return std::get<Array>(m_data);
            } else if constexpr (std::is_same_v<T, String>) {
                if (m_type == Type::eString) return std::get<String>(m_data);
            } else if constexpr (std::is_same_v<T, Bool>) {
                if (m_type == Type::eBool) return std::get<Bool>(m_data);
            } else if constexpr (std::is_enum_v<T>) {
                if (m_type == Type::eNumber) return static_cast<T>(std::llround(std::get<Number>(m_data)));
            } else if constexpr (std::is_integral_v<T>) {
                if (m_type == Type::eNumber) return static_cast<T>(std::llround(std::get<Number>(m_data)));
            } else if constexpr (std::is_floating_point_v<T>) {
                if (m_type == Type::eNumber) return static_cast<T>(std::get<Number>(m_data));
            }
            if constexpr (std::is_convertible_v<Object, T>) {
                if (m_type == Type::eObject) return static_cast<T>(std::get<Object>(m_data));
            }
            if constexpr (std::is_convertible_v<Array, T>) {
                if (m_type == Type::eArray) return static_cast<T>(std::get<Array>(m_data));
            }
            if constexpr (std::is_convertible_v<String, T>) {
                if (m_type == Type::eString) return static_cast<T>(std::get<String>(m_data));
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
            if constexpr (!json_types<T> && !std::is_arithmetic_v<T> && !std::is_enum_v<T> && std::is_convertible_v<Value, T>) {
                // try convert Value to T, but T is not json_types or is_arithmetic or is_enum
                // Because these types have already been determined above
                // Value supports default values for these types of conversion functions, which can interfere with the output results
                return static_cast<T>(*this);
            }
            return default_value;
        }



        /**
         * @brief type conversion, Move or Copy inner value to specified type
         * @tparam T The target type to convert to
         * @return The converted value
         * @throws std::runtime_error if conversion fails
         * @note 
         * Number is double, so conversions to integral (and enum) types will round to nearest.
         * Complex types like Object, Array, String will be moved if possible.
         * Simple types like Bool, Number, Null will be copied.
         * @warning 
         * The type conversion function of `json::Value` supports `enum`, `arithmetic`, and `json types`, and can return default values even if they are incompatible.
         * However, **the 'to' function does not return a default value when completely incompatible**, but instead chooses to throw an exception. 
         * If you need default values, please use move_or.
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
         * 9. Object -> convertible types (Move)
         * 10. Array -> convertible types (Move)
         * 11. String -> convertible types (Move)
         * 12. Number -> convertible types
         * 13. Bool -> convertible types
         * 14. Null -> convertible types
         * 15. Any -> Value convertible types (Move), but convertible type is not `json_types` or `is_arithmetic` or `is_enum`
         * 16. throw std::runtime_error
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
            std::is_convertible<Null, T>,
            std::is_convertible<Value, T>
        > [[nodiscard]]
        T  move() {
            if constexpr (std::is_same_v<T, Null>) {
                if (m_type == Type::eNull) return Null{};
            } else if constexpr (std::is_same_v<T, Object>) {
                if (m_type == Type::eObject) return std::move(std::get<Object>(m_data));
            } else if constexpr (std::is_same_v<T, Array>) {
                if (m_type == Type::eArray) return std::move(std::get<Array>(m_data));
            } else if constexpr (std::is_same_v<T, String>) {
                if (m_type == Type::eString) return std::move(std::get<String>(m_data));
            } else if constexpr (std::is_same_v<T, Bool>) {
                if (m_type == Type::eBool) return std::get<Bool>(m_data);
            } else if constexpr (std::is_enum_v<T>) {
                if (m_type == Type::eNumber) return static_cast<T>(std::llround(std::get<Number>(m_data)));
            } else if constexpr (std::is_integral_v<T>) {
                if (m_type == Type::eNumber) return static_cast<T>(std::llround(std::get<Number>(m_data)));
            } else if constexpr (std::is_floating_point_v<T>) {
                if (m_type == Type::eNumber) return static_cast<T>(std::get<Number>(m_data));
            }
            if constexpr (std::is_convertible_v<Object, T>) {
                if (m_type == Type::eObject) return static_cast<T>(std::move(std::get<Object>(m_data)));
            }
            if constexpr (std::is_convertible_v<Array, T>) {
                if (m_type == Type::eArray) return static_cast<T>(std::move(std::get<Array>(m_data)));
            }
            if constexpr (std::is_convertible_v<String, T>) {
                if (m_type == Type::eString) return static_cast<T>(std::move(std::get<String>(m_data)));
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
            if constexpr (!json_types<T> && !std::is_arithmetic_v<T> && !std::is_enum_v<T> && std::is_convertible_v<Value, T>) {
                // try convert Value to T, but T is not json_types or is_arithmetic or is_enum
                // Because these types have already been determined above
                // Value supports default values for these types of conversion functions, which can interfere with the output results
                return static_cast<T>(std::move(*this));
            }
            throw std::runtime_error("Value type mismatch");
        }

        /**
         * @brief type conversion, Move or Copy inner value to specified type
         * @tparam T The target type to convert to
         * @return The converted value
         * @note 
         * Number is double, so conversions to integral (and enum) types will round to nearest.
         * Complex types like Object, Array, String will be moved if possible.
         * Simple types like Bool, Number, Null will be copied.
         * @warning 
         * The type conversion function of `json::Value` supports `enum`, `arithmetic`, and `json types`, and can return default values even if they are incompatible.
         * However, **the 'to' function does not return a default value when completely incompatible**, but instead chooses to throw an exception. 
         * If you need default values, please use move_or.
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
         * 9. Object -> convertible types (Move)
         * 10. Array -> convertible types (Move)
         * 11. String -> convertible types (Move)
         * 12. Number -> convertible types
         * 13. Bool -> convertible types
         * 14. Null -> convertible types
         * 15. Any -> Value convertible types (Move), but convertible type is not `json_types` or `is_arithmetic` or `is_enum`
         * 16. return std::nullopt;
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
            std::is_convertible<Null, T>,
            std::is_convertible<Value, T>
        > [[nodiscard]]
        std::optional<T>  move_if() noexcept {
            if constexpr (std::is_same_v<T, Null>) {
                if (m_type == Type::eNull) return Null{};
            } else if constexpr (std::is_same_v<T, Object>) {
                if (m_type == Type::eObject) return std::move(std::get<Object>(m_data));
            } else if constexpr (std::is_same_v<T, Array>) {
                if (m_type == Type::eArray) return std::move(std::get<Array>(m_data));
            } else if constexpr (std::is_same_v<T, String>) {
                if (m_type == Type::eString) return std::move(std::get<String>(m_data));
            } else if constexpr (std::is_same_v<T, Bool>) {
                if (m_type == Type::eBool) return std::get<Bool>(m_data);
            } else if constexpr (std::is_enum_v<T>) {
                if (m_type == Type::eNumber) return static_cast<T>(std::llround(std::get<Number>(m_data)));
            } else if constexpr (std::is_integral_v<T>) {
                if (m_type == Type::eNumber) return static_cast<T>(std::llround(std::get<Number>(m_data)));
            } else if constexpr (std::is_floating_point_v<T>) {
                if (m_type == Type::eNumber) return static_cast<T>(std::get<Number>(m_data));
            }
            if constexpr (std::is_convertible_v<Object, T>) {
                if (m_type == Type::eObject) return static_cast<T>(std::move(std::get<Object>(m_data)));
            }
            if constexpr (std::is_convertible_v<Array, T>) {
                if (m_type == Type::eArray) return static_cast<T>(std::move(std::get<Array>(m_data)));
            }
            if constexpr (std::is_convertible_v<String, T>) {
                if (m_type == Type::eString) return static_cast<T>(std::move(std::get<String>(m_data)));
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
            if constexpr (!json_types<T> && !std::is_arithmetic_v<T> && !std::is_enum_v<T> && std::is_convertible_v<Value, T>) {
                // try convert Value to T, but T is not json_types or is_arithmetic or is_enum
                // Because these types have already been determined above
                // Value supports default values for these types of conversion functions, which can interfere with the output results
                return static_cast<T>(std::move(*this));
            }
            return std::nullopt; // return nullopt if conversion fails
        }

        /**
         * @brief type conversion, Move or Copy inner value to specified type
         * @tparam T The target type to convert to
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
         * 9. Object -> convertible types (Move)
         * 10. Array -> convertible types (Move)
         * 11. String -> convertible types (Move)
         * 12. Number -> convertible types
         * 13. Bool -> convertible types
         * 14. Null -> convertible types
         * 15. Any -> Value convertible types (Move), but convertible type is not `json_types` or `is_arithmetic` or `is_enum`
         * 16. return default_value
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
            std::is_convertible<Null, T>,
            std::is_convertible<Value, T>
        > [[nodiscard]]
        T  move_or( T default_value ) noexcept {
            if constexpr (std::is_same_v<T, Null>) {
                if (m_type == Type::eNull) return Null{};
            } else if constexpr (std::is_same_v<T, Object>) {
                if (m_type == Type::eObject) return std::move(std::get<Object>(m_data));
            } else if constexpr (std::is_same_v<T, Array>) {
                if (m_type == Type::eArray) return std::move(std::get<Array>(m_data));
            } else if constexpr (std::is_same_v<T, String>) {
                if (m_type == Type::eString) return std::move(std::get<String>(m_data));
            } else if constexpr (std::is_same_v<T, Bool>) {
                if (m_type == Type::eBool) return std::get<Bool>(m_data);
            } else if constexpr (std::is_enum_v<T>) {
                if (m_type == Type::eNumber) return static_cast<T>(std::llround(std::get<Number>(m_data)));
            } else if constexpr (std::is_integral_v<T>) {
                if (m_type == Type::eNumber) return static_cast<T>(std::llround(std::get<Number>(m_data)));
            } else if constexpr (std::is_floating_point_v<T>) {
                if (m_type == Type::eNumber) return static_cast<T>(std::get<Number>(m_data));
            }
            if constexpr (std::is_convertible_v<Object, T>) {
                if (m_type == Type::eObject) return static_cast<T>(std::move(std::get<Object>(m_data)));
            }
            if constexpr (std::is_convertible_v<Array, T>) {
                if (m_type == Type::eArray) return static_cast<T>(std::move(std::get<Array>(m_data)));
            }
            if constexpr (std::is_convertible_v<String, T>) {
                if (m_type == Type::eString) return static_cast<T>(std::move(std::get<String>(m_data)));
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
            if constexpr (!json_types<T> && !std::is_arithmetic_v<T> && !std::is_enum_v<T> && std::is_convertible_v<Value, T>) {
                // try convert Value to T, but T is not json_types or is_arithmetic or is_enum
                // Because these types have already been determined above
                // Value supports default values for these types of conversion functions, which can interfere with the output results
                return static_cast<T>(std::move(*this));
            }
            return default_value; // Return default value if conversion fails
        }


        /**
         * @brief For Value-to-Value accurate comparison
         * @tparam T Type that satisfies std::equality_comparable concept
         * @warning Number is double, so the comparison of numbers will be very strict
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
         * 0. IF T is Value, use Value::operator==(const Value& other) ,
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
         * 11. Else return false
         */
        template<typename  T>
        requires (!std::is_same_v<T, Value> && std::equality_comparable<T>)
        bool operator==(const T& other) const noexcept {
            if constexpr ( std::is_same_v<T,Null> ) {
                return m_type == Type::eNull;
            } else if constexpr ( std::is_same_v<T,Bool> ) {
                if ( m_type == Type::eBool ) return std::get<Bool>(m_data) == other;
            } else if constexpr ( std::is_same_v<T,Number> ) {
                if ( m_type == Type::eNumber ) return std::get<Number>(m_data) == other;
            } else if constexpr ( std::is_same_v<T,String> ) {
                if ( m_type == Type::eString ) return std::get<String>(m_data) == other;
            } else if constexpr ( std::is_same_v<T,Array> ) {
                if ( m_type == Type::eArray ) return std::get<Array>(m_data) == other;
            } else if constexpr ( std::is_same_v<T,Object> ) {
                if ( m_type == Type::eObject ) return std::get<Object>(m_data) == other;
            } else if constexpr (std::is_enum_v<T>) {
                if (m_type == Type::eNumber) return static_cast<T>(std::llround(std::get<Number>(m_data))) == other;
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

    };

    /**
     * @brief Parse JSON string into Value object (Equal to `parse( ... )`)
     * @param text The JSON string to parse
     * @param max_depth Maximum nesting depth (default: 256)
     * @return std::expected<Value, ParseError> Parsed Value or error
     */
    std::expected<Value, ParseError> deserialize(const std::string_view text, const std::int32_t max_depth = 256 ) noexcept;

    /**
     * @brief Parse JSON string from input-stream into Value object (Equal to `parse( ... )`)
     * @param is_test input-stream
     * @param max_depth Maximum nesting depth (default: 256)
     * @return std::expected<Value, ParseError> Parsed Value or error
     */
    std::expected<Value, ParseError> deserialize(std::istream& is_test, const std::int32_t max_depth = 256 ) noexcept;


    /**
     * @brief Parse JSON string into Value object (Equal to `deserialize( ... )`)
     * @param text The JSON string to parse
     * @param max_depth Maximum nesting depth (default: 256)
     * @return std::expected<Value, ParseError> Parsed Value or error
     */
    std::expected<Value, ParseError> parse(const std::string_view text, const std::int32_t max_depth = 256 ) noexcept {
        return deserialize(text, max_depth);
    }

    /**
     * @brief Parse JSON string from input-stream into Value object (Equal to `deserialize( ... )`)
     * @param is_test input-stream
     * @param max_depth Maximum nesting depth (default: 256)
     * @return std::expected<Value, ParseError> Parsed Value or error
     */
    std::expected<Value, ParseError> parse(std::istream& is_test, const std::int32_t max_depth = 256 ) noexcept {
        return deserialize(is_test, max_depth);
    }

}
