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
        eRedundantText      ///< Redundant text after valid JSON data
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
     * @note Number type could be cast to Bool by `to/move` function in this library, but not equal in `==` operator.
     */
    using Number = double;
    static_assert(std::is_same_v<Number, double>, "Number type must be double");
    static_assert( Number{} == 0.0, "Number type must be 0.0");

    /**
     * @typedef Bool
     * @brief JSON boolean type definition, using bool
     * @note Bool type could be cast to Number by `to/move` function in this library, but not equal in `==` operator.
     */
    using Bool = bool;
    static_assert(std::is_same_v<Bool, bool>, "Bool type must be bool");
    static_assert( Bool{} == false, "Bool type must be false");

    /**
     * @typedef Null
     * @brief JSON null type definition, using std::nullptr_t
     * @note Null type could not cast to false or 0 in this library,
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
     * @brief The available types for 'to/move' function, must include 'json types'
     */
    template<typename T>
    concept convertible_types = json_types<T> || std::disjunction_v<
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
     * @brief Convertable k-v type, D is default mapped type
     */
    template<typename T, typename D>
    concept convertible_map_types = std::ranges::range<T> && requires {
        typename T::key_type;
        typename T::mapped_type;
        requires std::is_constructible_v<D,typename T::mapped_type>;
        requires std::is_convertible_v<String, typename T::key_type>;
        requires convertible_types<typename T::mapped_type>;
        requires std::is_default_constructible_v<T>;
        requires std::is_default_constructible_v<typename T::mapped_type>;
    } && requires (T t, String s, typename T::mapped_type m) {
        t.emplace(static_cast<typename T::key_type>(s), std::move(m));
    };

    /**
     * @brief Convertable array type, D is default value type
     */
    template<typename T, typename D>
    concept convertible_array_types =  std::ranges::range<T> && requires {
        typename T::value_type;
        requires std::is_constructible_v<D,typename T::value_type>;
        requires convertible_types<typename T::value_type>;
        requires std::is_default_constructible_v<T>;
        requires std::is_default_constructible_v<typename T::value_type>;
    } && requires (T t, typename T::value_type v) {
        t.emplace_back(std::move(v));
    };

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
        std::size_t size() const noexcept {
            switch (m_type) {
                case Type::eObject: return std::get<Object>(m_data).size();
                case Type::eArray:  return std::get<Array>(m_data).size();
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
            return "Unknown"; // Should never happen
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
        Value(const Null) noexcept {}

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
        Value(const Bool bl) noexcept : m_data(bl), m_type(Type::eBool) {}

        /**
         * @brief Assign Value from boolean
         */
        Value& operator=(const Bool bl) noexcept {
            m_type = Type::eBool;
            m_data = bl;
            return *this;
        }


        /**
         * @brief Construct Value from numeric type
         * @param num Numeric value to initialize
         */
        Value(Number num) noexcept : m_data(num), m_type(Type::eNumber) {}

        /**
         * @brief Assign Value from numeric type
         * @param num Numeric value to assign
         */
        Value& operator=(const Number num) noexcept {
            m_type = Type::eNumber;
            m_data = num;
            return *this;
        }

        /**
         * @brief Construct Value from numeric types
         * @tparam T Numeric type (integral or floating-point or enum)
         */
        template<typename T>
        requires (std::is_arithmetic_v<T> || std::is_enum_v<T>)
        Value(T num) noexcept : m_data( static_cast<Number>(num) ), m_type(Type::eNumber) {}

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
        Value(String str) noexcept : m_data(std::move(str)), m_type(Type::eString) {}

        /**
         * @brief Construct Value from C-style string literal
         * @param str C-style string literal or const char*
         */
        Value(const char* str) noexcept : m_data(String{str}), m_type(Type::eString) {}

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
        Value(Object obj) noexcept : m_data(std::move(obj)), m_type(Type::eObject) {}


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
        Value(Array arr) noexcept : m_data(std::move(arr)), m_type(Type::eArray) {}
        /**
         * @brief Assign Value from Array
         */
        Value& operator=(Array arr) noexcept {
            m_type = Type::eArray;
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
        explicit Value(T other) noexcept : m_data( Array{} ), m_type( Type::eArray ) {
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
        explicit Value(T other) noexcept : m_data( Object{} ), m_type( Type::eObject ) {
            auto& obj = std::get<Object>(m_data);
            for (auto& [key, val]: other) {
                obj.emplace( static_cast<String>(key), static_cast<Value>(std::move(val)) );
            }
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
         * when stream.fail() return true, this function will quickly return but not throw any exception.
         */
        void serialize_to(std::ostream& os) const noexcept;

        /**
         * @brief Serialize Value to compact JSON string ( Must succeed )
         * @return Compact JSON string without formatting
         */
        [[nodiscard]]
        String serialize() const noexcept {
            String res;
            this->serialize_to(res);
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
        Bool serialize_pretty_to(
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
        std::optional<String> serialize_pretty(
            const std::uint16_t space_num = 2, 
            const std::uint16_t depth = 0, 
            const std::uint32_t max_space = 512
        ) const noexcept {
            if(String res; this->serialize_pretty_to(res, space_num, depth, max_space)) return res;
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
         * @tparam D The mapped_type or value_type of the target type, used for range conversion, default is Null for other types(useless).
         * @param default_range_value if T is a range type and is not json::Array or json::Object, must be specified for safe conversion. Else, please use default value.
         * @return The converted value
         * @throws std::runtime_error if conversion fails
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
         * 16. throw std::runtime_error
         */
        template<typename T, typename D = Null>
        requires convertible_types<T> || convertible_map_types<T, D> || convertible_array_types<T, D>
        [[nodiscard]]
        T  to( D default_range_value = D{} ) const {
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
            if constexpr (std::is_constructible_v<T, Value>) {
                return static_cast<T>(*this);
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
                if (m_type == Type::eNull) return static_cast<T>(Null{});
            }
            if constexpr ( convertible_map_types<T, D> ) {
                if (m_type == Type::eObject) {
                    T result{};
                    for (auto& [key, value] : std::get<Object>(m_data)) {
                        auto val = value.to_if<typename T::mapped_type>();
                        if (!val) result.emplace(static_cast<typename T::key_type>(key), default_range_value);
                        else result.emplace(static_cast<typename T::key_type>(key), std::move(*val));
                    }
                    return result;
                }
            }
            if constexpr ( convertible_array_types<T, D> ) {
                if (m_type == Type::eArray) {
                    T result{};
                    for (auto& value : std::get<Array>(m_data)) {
                        auto val = value.to_if<typename T::value_type>();
                        if (!val) result.emplace_back(default_range_value);
                        else result.emplace_back(std::move(*val));
                    }
                    return result;
                }
            }
            throw std::runtime_error("Value type mismatch");
        }

        /**
         * @brief type conversion, copy inner value to specified type
         * @tparam T The target type to convert to
         * @tparam D The mapped_type or value_type of the target type, used for range conversion, default is Null for other types(useless).
         * @param default_range_value if T is a range type and is not json::Array or json::Object, must be specified for safe conversion. Else, please use default value.
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
        requires convertible_types<T> || convertible_map_types<T, D> || convertible_array_types<T, D>
        [[nodiscard]]
        std::optional<T>  to_if( D default_range_value = D{} ) const noexcept {
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
            if constexpr (std::is_constructible_v<T, Value>) {
                return static_cast<T>(*this);
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
                if (m_type == Type::eNull) return static_cast<T>(Null{});
            }
            if constexpr ( convertible_map_types<T, D> ) {
                if (m_type == Type::eObject) {
                    T result{};
                    for (auto& [key, value] : std::get<Object>(m_data)) {
                        auto val = value.to_if<typename T::mapped_type>();
                        if (!val) result.emplace(static_cast<typename T::key_type>(key), default_range_value);
                        else result.emplace(static_cast<typename T::key_type>(key), std::move(*val));
                    }
                    return result;
                }
            }
            if constexpr ( convertible_array_types<T, D> ) {
                if (m_type == Type::eArray) {
                    T result{};
                    for (auto& value : std::get<Array>(m_data)) {
                        auto val = value.to_if<typename T::value_type>();
                        if (!val) result.emplace_back(default_range_value);
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
         * @param default_result The default value to return if conversion fails
         * @param default_range_value if T is a range type and is not json::Array or json::Object, must be specified for safe conversion. Else, please use default value.
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
         * 9. Any -> T is constructible from json::Value
         * 10. Object -> implicit convertible types
         * 11. Array -> implicit convertible types
         * 12. String -> implicit convertible types
         * 13. Number -> implicit convertible types
         * 14. Bool -> implicit convertible types
         * 15. Null -> implicit convertible types (Null is not convertible to bool !!!!!)
         * 16. Object -> Try copy to `range && String->key_type && Value->mapped_type types && have default_range_value`
         * 17. Array -> Try copy to `range && Value->value_type types && have default_range_value`
         * 18. return default_result
         */
        template<typename T, typename D = Null>
        requires convertible_types<T> || convertible_map_types<T, D> || convertible_array_types<T, D>
        [[nodiscard]]
        T  to_or( T default_result,  D default_range_value = D{} ) const noexcept {
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
            if constexpr (std::is_constructible_v<T, Value>) {
                return static_cast<T>(*this);
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
                if (m_type == Type::eNull) return static_cast<T>(Null{});
            }
            if constexpr ( convertible_map_types<T, D> ) {
                if (m_type == Type::eObject) {
                    T result{};
                    for (auto& [key, value] : std::get<Object>(m_data)) {
                        auto val = value.to_if<typename T::mapped_type>();
                        if (!val) result.emplace(static_cast<typename T::key_type>(key), static_cast<typename T::mapped_type>(default_range_value));
                        else result.emplace(static_cast<typename T::key_type>(key), std::move(*val));
                    }
                    return result;
                }
            }
            if constexpr ( convertible_array_types<T, D> ) {
                if (m_type == Type::eArray) {
                    T result{};
                    for (auto& value : std::get<Array>(m_data)) {
                        auto val = value.to_if<typename T::value_type>();
                        if (!val) result.emplace_back( static_cast<typename T::value_type>(default_range_value));
                        else result.emplace_back(std::move(*val));
                    }
                    return result;
                }
            }
            return default_result;
        }



        /**
         * @brief type conversion, Move or Copy inner value to specified type
         * @tparam T The target type to convert to
         * @tparam D The mapped_type or value_type of the target type, used for range conversion, default is Null for other types(useless).
         * @param default_range_value if T is a range type and is not json::Array or json::Object, must be specified for safe conversion. Else, please use default value.
         * @return The converted value
         * @throws std::runtime_error if conversion fails
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
         * 18. throw std::runtime_error
         */
        template<typename T, typename D = Null>
        requires convertible_types<T> || convertible_map_types<T, D> || convertible_array_types<T, D>
        [[nodiscard]]
        T  move( D default_range_value = D{} ) {
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
            if constexpr (std::is_constructible_v<T, Value>) {
                return static_cast<T>(std::move(*this));
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
                if (m_type == Type::eNull) return static_cast<T>(Null{});
            }
            if constexpr ( convertible_map_types<T, D> ) {
                if (m_type == Type::eObject) {
                    T result{};
                    for (auto& [key, value] : std::get<Object>(m_data)) {
                        auto val = value.move_if<typename T::mapped_type>();
                        if (!val) result.emplace(static_cast<typename T::key_type>(key), static_cast<typename T::mapped_type>(default_range_value));
                        else result.emplace(static_cast<typename T::key_type>(key), std::move(*val));
                    }
                    return result;
                }
            }
            if constexpr ( convertible_array_types<T, D> ) {
                if (m_type == Type::eArray) {
                    T result{};
                    for (auto& value : std::get<Array>(m_data)) {
                        auto val = value.move_if<typename T::value_type>();
                        if (!val) result.emplace_back( static_cast<typename T::value_type>(default_range_value) );
                        else result.emplace_back(std::move(*val));
                    }
                    return result;
                }
            }
            throw std::runtime_error("Value type mismatch");
        }

        /**
         * @brief type conversion, Move or Copy inner value to specified type
         * @tparam T The target type to convert to
         * @tparam D The mapped_type or value_type of the target type, used for range conversion, default is Null for other types(useless).
         * @param default_range_value if T is a range type and is not json::Array or json::Object, must be specified for safe conversion. Else, please use default value.
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
        requires convertible_types<T> || convertible_map_types<T, D> || convertible_array_types<T, D>
        [[nodiscard]]
        std::optional<T>  move_if( D default_range_value = D{} ) noexcept {
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
            if constexpr (std::is_constructible_v<T, Value>) {
                return static_cast<T>(std::move(*this));
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
                if (m_type == Type::eNull) return static_cast<T>(Null{});
            }
            if constexpr ( convertible_map_types<T, D> ) {
                if (m_type == Type::eObject) {
                    T result{};
                    for (auto& [key, value] : std::get<Object>(m_data)) {
                        auto val = value.move_if<typename T::mapped_type>();
                        if (!val) result.emplace(static_cast<typename T::key_type>(key), static_cast<typename T::mapped_type>(default_range_value));
                        else result.emplace(static_cast<typename T::key_type>(key), std::move(*val));
                    }
                    return result;
                }
            }
            if constexpr ( convertible_array_types<T, D> ) {
                if (m_type == Type::eArray) {
                    T result{};
                    for (auto& value : std::get<Array>(m_data)) {
                        auto val = value.move_if<typename T::value_type>();
                        if (!val) result.emplace_back( static_cast<typename T::value_type>(default_range_value) );
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
         * @param default_result The default value to return if conversion fails
         * @param default_range_value if T is a range type and is not json::Array or json::Object, must be specified for safe conversion. Else, please use default value.
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
         * 18. return default_value
         */
        template<typename T, typename D = Null>
        requires convertible_types<T> || convertible_map_types<T, D> || convertible_array_types<T, D>
        [[nodiscard]]
        T  move_or( T default_result, D default_range_value = D{} ) noexcept {
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
            if constexpr (std::is_constructible_v<T, Value>) {
                return static_cast<T>(std::move(*this));
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
                if (m_type == Type::eNull) return static_cast<T>(Null{});
            }
            if constexpr ( convertible_map_types<T, D> ) {
                if (m_type == Type::eObject) {
                    T result{};
                    for (auto& [key, value] : std::get<Object>(m_data)) {
                        auto val = value.move_if<typename T::mapped_type>();
                        if (!val) result.emplace(static_cast<typename T::key_type>(key), static_cast<typename T::mapped_type>(default_range_value));
                        else result.emplace(static_cast<typename T::key_type>(key), std::move(*val));
                    }
                    return result;
                }
            }
            if constexpr ( convertible_array_types<T, D> ) {
                if (m_type == Type::eArray) {
                    T result{};
                    for (auto& value : std::get<Array>(m_data)) {
                        auto val = value.move_if<typename T::value_type>();
                        if (!val) result.emplace_back( static_cast<typename T::value_type>(default_range_value) );
                        else result.emplace_back(std::move(*val));
                    }
                    return result;
                }
            }
            return default_result; // Return default value if conversion fails
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
            } else if constexpr (std::is_floating_point_v<T>) {
                if (m_type == Type::eNumber) return static_cast<T>(std::get<Number>(m_data)) == other;
            } else if constexpr (std::is_convertible_v<T, std::string_view>) {
                if(m_type == Type::eString) return std::get<String>(m_data) == std::string_view( other );
            } else if constexpr (std::equality_comparable<T> && std::is_constructible_v<T, Value>) {
                return other == static_cast<T>(*this);     // Use T's operator==
            } else if constexpr (std::is_constructible_v<Value, T>) {
                return static_cast<Value>(other) == *this; // Use Value's operator==
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
    std::expected<Value, ParseError> deserialize(std::string_view text, std::int32_t max_depth = 256 ) noexcept;

    /**
     * @brief Parse JSON string from input-stream into Value object (Equal to `parse( ... )`)
     * @param is_test input-stream
     * @param max_depth Maximum nesting depth (default: 256)
     * @return std::expected<Value, ParseError> Parsed Value or error
     */
    std::expected<Value, ParseError> deserialize(std::istream& is_test, std::int32_t max_depth = 256 ) noexcept;


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
