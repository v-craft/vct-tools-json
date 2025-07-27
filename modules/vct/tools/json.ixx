/**
 * @file json.ixx
 * @brief JSON module for vct.tools
 * @author Mysvac
 * @date 2025-07-27
 *
 * This module provides a JSON parser and serializer.
 */
export module vct.tools.json;

import std;

/**
 * @namespace vct::tools::json
 * @brief Namespace for JSON related tools and types.
 * @note Non-export content.
 */
namespace vct::tools::json {

    /**
     * @brief Concept to check if a type is a character iterator.
     * @note Non-export.
     */
    template<typename T>
    concept char_iterator =  std::disjunction_v<
        std::is_same<T, std::string_view::const_iterator>,
        std::is_same<T, std::istreambuf_iterator<char>>
    >;

    /**
     * @brief A lookup table for hexadecimal characters.
     * @note Non-export.
     */
    constexpr std::array<std::uint8_t, 256> hex_table = [] {
        std::array<std::uint8_t, 256> table{};
        for (int i = 0; i < 256; ++i) table[i] = 255; // Invalid marker
        for (int i = 0; i <= 9; ++i) table['0' + i] = i;
        for (int i = 0; i <= 5; ++i) table['A' + i] = 10 + i;
        for (int i = 0; i <= 5; ++i) table['a' + i] = 10 + i;
        return table;
    }();
}

/**
 * @namespace vct::tools::json
 * @brief Namespace for JSON related tools and types.
 * @note Export content.
 */
export namespace vct::tools::json {

    /**
     * @brief Concept to check if a type is a JSON type.
     * @tparam J The JSON Container Class.
     * @tparam T The type to check.
     */
    template<typename J, typename T>
    concept json_type = std::disjunction_v<
        std::is_same<T, typename J::Null>,
        std::is_same<T, typename J::Bool>,
        std::is_same<T, typename J::Number>,
        std::is_same<T, typename J::String>,
        std::is_same<T, typename J::Array>,
        std::is_same<T, typename J::Object>
    >;

    /**
     * @brief Concept to check if JSON Container class is directly constructible from a type.
     * @tparam J The JSON Container Class.
     * @tparam T The type to check.
     */
    template<typename J, typename T>
    concept constructible = std::disjunction_v<
        std::is_arithmetic<T>,
        std::is_enum<T>,
        std::is_convertible<T, typename J::Array>,
        std::is_convertible<T, typename J::Object>,
        std::is_convertible<T, typename J::String>,
        std::is_convertible<T, typename J::Number>,
        std::is_convertible<T, typename J::Bool>,
        std::is_convertible<T, typename J::Null>
    >;

    /**
     * @brief Concept to check if JSON Container class is constructible from an array-like type.
     * @tparam J The JSON Container Class.
     * @tparam T The type to check.
     */
    template<typename J, typename T>
    concept constructible_array = std::ranges::range<T> &&
        std::is_constructible_v<J, typename T::value_type>;

    /**
     * @brief Concept to check if JSON Container class is constructible from a map-like type.
     * @tparam J The JSON Container Class.
     * @tparam T The type to check.
     */
    template<typename J, typename T>
    concept constructible_map = std::ranges::range<T> &&
        std::is_convertible_v<typename T::key_type, typename J::String> &&
        std::is_constructible_v<J, typename T::mapped_type>;

    /**
     * @brief Concept to check if JSON Container class is directly convertible to a type.
     * @tparam J The JSON Container Class.
     * @tparam T The type to check.
     */
    template<typename J, typename T>
    concept convertible = std::disjunction_v<
        std::is_arithmetic<T>,
        std::is_enum<T>,
        std::is_convertible<typename J::Array, T>,
        std::is_convertible<typename J::Object, T>,
        std::is_convertible<typename J::String, T>,
        std::is_convertible<typename J::Number, T>,
        std::is_convertible<typename J::Bool, T>,
        std::is_convertible<typename J::Null, T>,
        std::is_constructible<T, J>
    >;

    /**
     * @brief Concept to check if JSON Container class is convertible to a map-like type.
     * @tparam J The JSON Container Class.
     * @tparam T The type to check.
     * @tparam D The default mapped_type in map-like type.
     */
    template<typename J, typename T, typename D>
    concept convertible_map = std::ranges::range<T> && requires {
        requires std::is_constructible_v<typename T::mapped_type, D>;
        requires std::is_convertible_v<typename J::String, typename T::key_type>;
        requires convertible<J, typename T::mapped_type>;
        requires std::is_default_constructible_v<T>;
        requires std::is_default_constructible_v<typename T::mapped_type>;
        requires std::is_copy_constructible_v<typename T::mapped_type>;
    } && requires (T t, typename J::String s, typename T::mapped_type m) {
        t.emplace(static_cast<typename T::key_type>(s), std::move(m));
    };

    /**
     * @brief Concept to check if JSON Container class is convertible to an array-like type.
     * @tparam J The JSON Container Class.
     * @tparam T The type to check.
     * @tparam D The default value_type in array-like type.
     */
    template<typename J, typename T, typename D>
    concept convertible_array =  std::ranges::range<T> && requires {
        requires std::is_constructible_v<typename T::value_type,D>;
        requires convertible<J, typename T::value_type>;
        requires std::is_default_constructible_v<T>;
        requires std::is_default_constructible_v<typename T::value_type>;
        requires std::is_copy_constructible_v<typename T::value_type>;
    } && requires (T t, typename T::value_type v) {
        t.emplace_back(std::move(v));
    };

    /**
     * @brief Enum class representing the type of JSON data.
     */
    enum class Type{
        eNull = 0,  ///< Null type
        eBool,      ///< Boolean type
        eNumber,    ///< Number type
        eString,    ///< String type
        eArray,     ///< Array type
        eObject     ///< Object type
    };

    /**
     * @brief Enum class representing the possible parse errors.
     */
    enum class ParseError {
        eNone = 0,          ///< No error
        eEmptyData,         ///< Empty data
        eRedundantText,     ///< Redundant text after valid JSON
        eDepthExceeded,     ///< Maximum depth exceeded
        eIllegalEscape,     ///< Illegal escape sequence in string
        eInvalidNumber,     ///< Invalid number format
        eUnclosedString,    ///< Unclosed string literal
        eUnclosedObject,    ///< Unclosed object literal
        eUnclosedArray,     ///< Unclosed array literal
        eUnknownFormat,     ///< Unknown format or character
        eUnknownError       ///< Unknown error occurred
    };

    /**
     * @brief Get the name of the JSON data type.
     * @return The name of the JSON data type as a string.
     */
    [[nodiscard]]
    constexpr const char* type_name(const Type type) noexcept {
        switch ( type ) {
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
     * @brief Get the name of a ParseError enum value.
     * @param error The ParseError enum value.
     * @return The name of the error as a string.
     */
    [[nodiscard]]
    constexpr const char* error_name(const ParseError error) noexcept {
        switch (error) {
            case ParseError::eNone: return "None";
            case ParseError::eEmptyData: return "EmptyData";
            case ParseError::eDepthExceeded: return "DepthExceeded";
            case ParseError::eIllegalEscape: return "IllegalEscape";
            case ParseError::eInvalidNumber: return "InvalidNumber";
            case ParseError::eRedundantText: return "RedundantText";
            case ParseError::eUnclosedString: return "UnclosedString";
            case ParseError::eUnclosedObject: return "UnclosedObject";
            case ParseError::eUnclosedArray: return "UnclosedArray";
            case ParseError::eUnknownFormat: return "UnknownFormat";
            case ParseError::eUnknownError: return "UnknownError";
            default: return "Unknown Enum Value";
        }
    }

    /**
     * @brief A JSON container class that can represent various JSON data types.
     * @tparam UseOrderedMap  Use `std::map` for JSON objects if true, otherwise use `std::unordered_map`.
     * @tparam AllocatorType A allocator template for the containers, default is `std::allocator`.
     * @note AllocatorType is used for the string, array, and object types. String is always `std::basic_string< ... >`。
     */
    template<
        bool UseOrderedMap = true,
        template<typename U> class AllocatorType = std::allocator
    >
    requires requires{
        typename std::basic_string<char, std::char_traits<char>, AllocatorType<char>>;
        typename std::vector<int, AllocatorType<int>>;
        typename std::map<std::string, int, std::less<std::string>, AllocatorType<std::pair<const std::string, int>>>;
        typename std::unordered_map<std::string, int, std::hash<std::string>, std::equal_to<std::string>, AllocatorType<std::pair<const std::string, int>>>;
    }
    class Json {
    public:
        /**
         * @brief Json's Null Type, must be `std::nullptr_t`.
         */
        using Null = std::nullptr_t;
        /**
         * @brief Json's Bool Type, must be `bool`.
         */
        using Bool = bool;
        /**
         * @brief Json's Number Type, must be `double`.
         * @note When converted to integers, it will be rounded to the nearest integer.
         */
        using Number = double;
        /**
         * @brief Json's String Type, `std::basic_string<char, std::char_traits<char>, AllocatorType<char>>`.
         * @note default is `std::string`.
         */
        using String = std::basic_string<char, std::char_traits<char>, AllocatorType<char>>;
        /**
         * @brief Json's Array Type, `vector<Json, AllocatorType<Json>>`.
         * @note default is `std::vector<Json>`.
         */
        using Array = std::vector<Json, AllocatorType<Json>>;
        /**
         * @brief Json's Object Type, `std::map` or `std::unordered_map`.
         * @note default is `std::map<std::string, Json>`.
         */
        using Object = std::conditional_t<UseOrderedMap,
            std::map<String, Json, std::less<String>, AllocatorType<std::pair<const String, Json>>>,
            std::unordered_map<String, Json, std::hash<String>, std::equal_to<String>, AllocatorType<std::pair<const String, Json>>>
        >;

    protected:
        std::variant<
            Null,
            Bool,
            Number,
            String,
            Array,
            Object
        > m_data { Null{} };

    private:
        /**
         * @brief Escape a string to a string back.
         * @param out The output string to append the escaped string to.
         * @param str The string to escape.
         */
        static void escape_to(String& out, const std::string_view str) {
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

        /**
         * @brief Escape a string to an out-stream.
         * @param out The output stream to append the escaped string to.
         * @param str The string to escape.
         */
        static void escape_to(std::ostream& out, const std::string_view str) {
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

        /**
         * @brief Unescape a Unicode escape sequence in a string, and move ptr.
         * @param out The output string to append the unescaped Unicode character to.
         * @param it The iterator pointing to the current position in the string.
         * @param end_ptr The end iterator of the string.
         * @return `true` if the unescape was successful, `false` if it failed.
         */
        static bool unescape_unicode_next(
            String& out,
            char_iterator auto& it,
            const char_iterator auto end_ptr
        ) {
            // it was in `\uABCD`'s `u` position
            ++it;
            if (it == end_ptr) return false;
            // `it` was in `\uXXXX`'s A position

            // move to \uABCD's D position and get hex4 value
            std::uint32_t code_point{ 0 };
            {
                const std::uint8_t d1 = hex_table[static_cast<unsigned char>(*it)];
                if (d1 == 255) return false; // Invalid if not a hex digit
                code_point = code_point << 4 | d1;

                ++it; // External `++it` may reduce some instructions
                if(it == end_ptr) return false; // Invalid if not enough characters
                const std::uint8_t d2 = hex_table[static_cast<unsigned char>(*it)];
                if (d2 == 255) return false; // Invalid if not a hex digit
                code_point = code_point << 4 | d2;

                ++it;
                if(it == end_ptr) return false; // Invalid if not enough characters
                const std::uint8_t d3 = hex_table[static_cast<unsigned char>(*it)];
                if (d3 == 255) return false; // Invalid if not a hex digit
                code_point = code_point << 4 | d3;

                ++it;
                if(it == end_ptr) return false; // Invalid if not enough characters
                const std::uint8_t d4 = hex_table[static_cast<unsigned char>(*it)];
                if (d4 == 255) return false; // Invalid if not a hex digit
                code_point = code_point << 4 | d4;
            }
            // `it` was in `\uABCD`'s D position and not be `end_ptr`, if hex4_next successful

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
                std::uint32_t low_code_point{ 0 };
                {
                    const std::uint8_t d1 = hex_table[static_cast<unsigned char>(*it)];
                    if (d1 == 255) return false; // Invalid if not a hex digit
                    low_code_point = low_code_point << 4 | d1;

                    ++it; // External `++it` may reduce some instructions
                    if(it == end_ptr) return false; // Invalid if not enough characters
                    const std::uint8_t d2 = hex_table[static_cast<unsigned char>(*it)];
                    if (d2 == 255) return false; // Invalid if not a hex digit
                    low_code_point = low_code_point << 4 | d2;

                    ++it;
                    if(it == end_ptr) return false; // Invalid if not enough characters
                    const std::uint8_t d3 = hex_table[static_cast<unsigned char>(*it)];
                    if (d3 == 255) return false; // Invalid if not a hex digit
                    low_code_point = low_code_point << 4 | d3;

                    ++it;
                    if(it == end_ptr) return false; // Invalid if not enough characters
                    const std::uint8_t d4 = hex_table[static_cast<unsigned char>(*it)];
                    if (d4 == 255) return false; // Invalid if not a hex digit
                    low_code_point = low_code_point << 4 | d4;
                }
                if( 0xDFFF < low_code_point ||  low_code_point < 0xDC00 ) return false;
                // `it` was in `\uABCD`'s D position and not be `end_ptr`, if hex4_next successful

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

        /**
         * @brief Unescape the string in a JSON string, and move ptr.
         * @param it The iterator pointing to the current position in the string.
         * @param end_ptr The end iterator of the string.
         * @return An expected String containing the unescaped string, or a ParseError if an error occurred.
         */
        static std::expected<String, ParseError> unescape_next(
            char_iterator auto& it,
            const char_iterator auto end_ptr
        ) {
            String res;
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

        /**
         * @brief Read a JSON value from the input iterator and create Json Object.
         * @param it The iterator pointing to the current position in the input.
         * @param end_ptr The end iterator of the input.
         * @param max_depth The maximum depth of nested JSON objects/arrays allowed.
         * @return An expected Json object containing the parsed JSON value, or a ParseError if an error occurred.
         */
        static std::expected<Json, ParseError> reader(
            char_iterator auto& it,
            const char_iterator auto end_ptr,
            const std::int32_t max_depth
        )  {
            // Check for maximum depth
            if(max_depth < 0) return std::unexpected( ParseError::eDepthExceeded );
            // return value
            Json json;
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
                    if(!str) return std::unexpected( str.error() );
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

    public:
        /**
         * @brief Get the type of the JSON data.
         * @return The type of the JSON data as a Type enum value.
         */
        [[nodiscard]]
        constexpr Type type() const noexcept { return static_cast<Type>(m_data.index()); }

        /**
         * @brief Check if the JSON data is of type Null.
         */
        [[nodiscard]]
        constexpr bool is_nul() const noexcept { return type() == Type::eNull; }

        /**
         * @brief Check if the JSON data is of type Bool.
         */
        [[nodiscard]]
        constexpr bool is_bol() const noexcept { return type() == Type::eBool; }

        /**
         * @brief Check if the JSON data is of type Number.
         */
        [[nodiscard]]
        constexpr bool is_num() const noexcept { return type() == Type::eNumber; }

        /**
         * @brief Check if the JSON data is of type String.
         */
        [[nodiscard]]
        constexpr bool is_str() const noexcept { return type() == Type::eString; }

        /**
         * @brief Check if the JSON data is of type Array.
         */
        [[nodiscard]]
        constexpr bool is_arr() const noexcept { return type() == Type::eArray; }

        /**
         * @brief Check if the JSON data is of type Object.
         */
        [[nodiscard]]
        constexpr bool is_obj() const noexcept { return type() == Type::eObject; }


        /**
         * @brief Get a reference to the Null type.
         * @throw std::bad_variant_access if the JSON data is not of type Null.
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
         * @brief Get a reference to the Bool type.
         * @throw std::bad_variant_access if the JSON data is not of type Bool.
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
         * @brief Get a reference to the Number type.
         * @throw std::bad_variant_access if the JSON data is not of type Number.
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
         * @brief Get a reference to the String type.
         * @throw std::bad_variant_access if the JSON data is not of type String.
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
         * @brief Get a reference to the Array type.
         * @throw std::bad_variant_access if the JSON data is not of type Array.
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
         * @brief Get a reference to the Object type.
         * @throw std::bad_variant_access if the JSON data is not of type Object.
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
         * @brief Default constructor for Json, data is Null type.
         */
        constexpr Json() noexcept = default;
        /**
         * @brief Destructor for Json, no special cleanup needed.
         */
        ~Json() noexcept = default;
        /**
         * @brief Copy constructor for Json, uses default copy semantics.
         */
        Json(const Json&) = default;
        /**
         * @brief Copy assignment operator for Json, uses default copy semantics.
         */
        Json& operator=(const Json&) = default;

        /**
         * @brief Move constructor for Json, transfers ownership of data.
         * @param other The Json object to move from, will be set to Null.
         */
        Json(Json&& other) noexcept {
            m_data = std::move(other.m_data);
            other.m_data = Null{};
        }
        /**
         * @brief Move assignment operator for Json, transfers ownership of data.
         * @param other The Json object to move from, will be set to Null.
         */
        Json& operator=(Json&& other) noexcept {
            if (this == &other) return *this;
            m_data = std::move(other.m_data);
            other.m_data = Null{};
            return *this;
        }

        /**
         * @brief Constructor for Json that accepts various types and converts them to Json.
         * @tparam T The type of the other object to convert to Json.
         * @param other The object to convert to Json.
         * @note `noexpect` when other is simple-type or r-value complex-json-type
         */
        template<typename T>
        requires constructible<Json, std::remove_cvref_t<T>>
        Json(T&& other) noexcept (
            std::is_arithmetic_v<std::remove_cvref_t<T>> ||
            std::is_enum_v<std::remove_cvref_t<T>> ||
            std::is_same_v<std::remove_cvref_t<T>, Null> ||
            std::is_same_v<std::remove_cvref_t<T>, Bool> ||
            (std::is_rvalue_reference_v<T&&> && (
                std::is_same_v<std::remove_cvref_t<T>, String> ||
                std::is_same_v<std::remove_cvref_t<T>, Array> ||
                std::is_same_v<std::remove_cvref_t<T>, Object>
            ))
        ) {
            if constexpr(std::is_same_v<T, Null>) {
                m_data = Null{};
            } else if constexpr(std::is_same_v<T, Bool>) {
                m_data = other;
            } else if constexpr(std::is_same_v<T, Number>) {
                m_data = other;
            } else if constexpr(std::is_same_v<T, String>) {
                m_data = std::forward<T>(other);
            } else if constexpr(std::is_same_v<T, Array>) {
                m_data = std::forward<T>(other);
            } else if constexpr(std::is_same_v<T, Object>) {
                m_data = std::forward<T>(other);
            } else if constexpr(std::is_arithmetic_v<T> || std::is_enum_v<T>) {
                m_data = static_cast<Number>(other);
            } else if constexpr(std::is_convertible_v<T, String>) {
                m_data = static_cast<String>(std::forward<T>(other));
            } else if constexpr(std::is_convertible_v<T, Object>) {
                m_data = static_cast<Object>(std::forward<T>(other));
            } else if constexpr(std::is_convertible_v<T,Array>) {
                m_data = static_cast<Array>(std::forward<T>(other));
            } else if constexpr(std::is_convertible_v<T,Number>) {
                m_data = static_cast<Number>(std::forward<T>(other));
            } else if constexpr(std::is_convertible_v<T,Bool>) {
                m_data = static_cast<Bool>(std::forward<T>(other));
            } else {
                m_data = Null{};
            }
        }

        /**
         * @brief Assignment operator for Json that accepts various types and converts them to Json.
         * @tparam T The type of the other object to convert to Json.
         * @param other The object to convert to Json.
         * @note `noexpect` when other is simple-type or r-value complex-json-type
         */
        template<typename T>
        requires constructible<Json, std::remove_cvref_t<T>>
        Json& operator=(T&& other) noexcept (
            std::is_arithmetic_v<std::remove_cvref_t<T>> ||
            std::is_enum_v<std::remove_cvref_t<T>> ||
            std::is_same_v<std::remove_cvref_t<T>, Null> ||
            std::is_same_v<std::remove_cvref_t<T>, Bool> ||
            (std::is_rvalue_reference_v<T&&> && (
                std::is_same_v<std::remove_cvref_t<T>, String> ||
                std::is_same_v<std::remove_cvref_t<T>, Array> ||
                std::is_same_v<std::remove_cvref_t<T>, Object>
            ))
        ) {
            if constexpr(std::is_same_v<T, Null>) {
                m_data = Null{};
            } else if constexpr(std::is_same_v<T, Bool>) {
                m_data = other;
            } else if constexpr(std::is_same_v<T, Number>) {
                m_data = other;
            } else if constexpr(std::is_same_v<T, String>) {
                m_data = std::forward<T>(other);
            } else if constexpr(std::is_same_v<T, Array>) {
                m_data = std::forward<T>(other);
            } else if constexpr(std::is_same_v<T, Object>) {
                m_data = std::forward<T>(other);
            } else if constexpr(std::is_arithmetic_v<T> || std::is_enum_v<T>) {
                m_data = static_cast<Number>(other);
            } else if constexpr(std::is_convertible_v<T, String>) {
                m_data = static_cast<String>(std::forward<T>(other));
            } else if constexpr(std::is_convertible_v<T, Object>) {
                m_data = static_cast<Object>(std::forward<T>(other));
            } else if constexpr(std::is_convertible_v<T,Array>) {
                m_data = static_cast<Array>(std::forward<T>(other));
            } else if constexpr(std::is_convertible_v<T,Number>) {
                m_data = static_cast<Number>(std::forward<T>(other));
            } else if constexpr(std::is_convertible_v<T,Bool>) {
                m_data = static_cast<Bool>(std::forward<T>(other));
            } else {
                m_data = Null{};
            }
            return *this;
        }

        /**
         * @brief Constructor for Json that accepts array-like types and converts them to Json.
         * @tparam T The type of the other object to convert to Json.
         * @param other The object to convert to Json.
         */
        template<typename T>
        requires !constructible<Json, std::remove_cvref_t<T>> && !constructible_map<Json, std::remove_cvref_t<T>> && constructible_array<Json, std::remove_cvref_t<T>>
        explicit Json(T&& other) : m_data( Array{} ) {
            auto& arr = std::get<Array>(m_data);
            for (auto&& item : std::forward<T>(other)) {
                arr.emplace_back( static_cast<Json>(static_cast<typename std::remove_cvref_t<T>::value_type>( std::forward<decltype(item)>(item))));
            }
        }

        /**
         * @brief Constructor for Json that accepts map-like types and converts them to Json.
         * @tparam T The type of the other object to convert to Json.
         * @param other The object to convert to Json.
         */
        template<typename T>
        requires !constructible<Json, std::remove_cvref_t<T>> && constructible_map<Json, std::remove_cvref_t<T>>
        explicit Json(T&& other) : m_data( Object{} ) {
            auto& obj = std::get<Object>(m_data);
            for (auto&& [key, val] : std::forward<T>(other)) {
                obj.emplace( static_cast<String>(key), static_cast<Json>(static_cast<typename std::remove_cvref_t<T>::mapped_type>(std::forward<decltype(val)>(val))) );
            }
        }

        /**
         * @brief Reset the JSON data to a specific type.
         * @tparam T The type to reset the JSON data to, defaults to Null.
         */
        template<typename T = Null>
        requires json_type<Json, T>
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
         * @brief Accessor for JSON data using the subscript operator.
         */
        [[nodiscard]]
        Json& operator[](const String& key) { return std::get<Object>(m_data)[key]; }
        [[nodiscard]]
        const Json& operator[](const String& key) const { return std::get<Object>(m_data).at(key); }
        [[nodiscard]]
        Json& operator[](const std::size_t index) { return std::get<Array>(m_data)[index]; }
        [[nodiscard]]
        const Json& operator[](const std::size_t index) const { return std::get<Array>(m_data).at(index); }

        /**
         * @brief Accessor for JSON data using the at() method.
         */
        [[nodiscard]]
        Json& at(const String& key) { return std::get<Object>(m_data).at(key); }
        [[nodiscard]]
        const Json& at(const String& key) const { return std::get<Object>(m_data).at(key); }
        [[nodiscard]]
        Json& at(const std::size_t index) { return std::get<Array>(m_data).at(index); }
        [[nodiscard]]
        const Json& at(const std::size_t index) const { return std::get<Array>(m_data).at(index); }

        /**
         * @brief Write the JSON data to a string back.
         */
        void write(String& out) const {
            switch (type()) {
                case Type::eObject: {
                    out.push_back('{');
                    for (const auto& [key, val] : std::get<Object>(m_data)) {
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

        /**
         * @brief Write the JSON data to an output stream.
         */
        void write(std::ostream& out) const {
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

        /**
         * @brief Dump the JSON data to a string.
         */
        [[nodiscard]]
        String dump() const {
            String res;
            this->write(res);
            return res;
        }

        /**
         * @brief Write the JSON data to a string with formatting.
         * @param out The output string to write to.
         * @param space_num The number of spaces to use for indentation (default is 2).
         * @param depth The current depth of indentation (default is 0).
         * @param max_space The maximum number of spaces allowed for indentation in a line (default is 512).
         * @return True if the writing was successful, false if it exceeded max_space.
         */
        Bool writef(
            String& out,
            const std::uint16_t space_num = 2,
            const std::uint16_t depth = 0,
            const std::uint32_t max_space = 512
        ) const {
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

        /**
         * @brief Write the JSON data to an output stream with formatting.
         * @param out The output stream to write to.
         * @param space_num The number of spaces to use for indentation (default is 2).
         * @param depth The current depth of indentation (default is 0).
         * @param max_space The maximum number of spaces allowed for indentation in a line (default is 512).
         * @return True if the writing was successful, false if it exceeded max_space.
         */
        Bool writef(
            std::ostream& out,
            const std::uint16_t space_num = 2,
            const std::uint16_t depth = 0,
            const std::uint32_t max_space = 512
        ) const {
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

        /**
         * @brief Dump the JSON data to a string with formatting.
         * @param space_num The number of spaces to use for indentation (default is 2).
         * @param depth The current depth of indentation (default is 0).
         * @param max_space The maximum number of spaces allowed for indentation in a line (default is 512).
         * @return A string containing the formatted JSON data.
         */
        [[nodiscard]]
        std::optional<String> dumpf(
            const std::uint16_t space_num = 2,
            const std::uint16_t depth = 0,
            const std::uint32_t max_space = 512
        ) const {
            if(String res; this->writef(res, space_num, depth, max_space)) return res;
            return std::nullopt;
        }

        /**
         * @brief Parse a JSON string or stream into a Json object.
         * @param text The JSON string to parse.
         * @param max_depth The maximum depth of nested structures allowed (default is 256).
         * @return A Json object if parsing is successful, or an error if it fails.
         */
        [[nodiscard]]
        static std::expected<Json, ParseError> parse(const std::string_view text, const std::int32_t max_depth = 256)  {
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
        [[nodiscard]]
        static std::expected<Json, ParseError> parse(std::istream& is_text, const std::int32_t max_depth = 256) {
            auto it = std::istreambuf_iterator<char>(is_text);
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
        requires convertible<Json, T> || convertible_map<Json, T, D> || convertible_array<Json, T, D>
        [[nodiscard]]
        std::optional<T>  to_if( D default_range_elem = D{} ) const {
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
            if constexpr (std::is_constructible_v<T, Json>) {
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
            if constexpr ( convertible_map<Json, T, D> ) {
                if (type() == Type::eObject) {
                    T result{};
                    for (auto& [key, value] : std::get<Object>(m_data)) {
                        auto val = value.template to_if<typename T::mapped_type>();
                        if (!val) result.emplace(static_cast<typename T::key_type>(key), static_cast<typename T::mapped_type>(default_range_elem));
                        else result.emplace(static_cast<typename T::key_type>(key), std::move(*val));
                    }
                    return result;
                }
            }
            if constexpr ( convertible_array<Json, T, D> ) {
                if (type() == Type::eArray) {
                    T result{};
                    for (auto& value : std::get<Array>(m_data)) {
                        auto val = value.template to_if<typename T::value_type>();
                        if (!val) result.emplace_back(default_range_elem);
                        else result.emplace_back(std::move(*val));
                    }
                    return result;
                }
            }
            return std::nullopt;
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
        requires convertible<Json, T> || convertible_map<Json, T, D> || convertible_array<Json, T, D>
        [[nodiscard]]
        T  to( D default_range_elem = D{} ) const {
            auto opt = to_if<T, D>( std::move(default_range_elem) );
            if (!opt) throw std::runtime_error("Cast fail.");
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
        requires convertible<Json, T> || convertible_map<Json, T, D> || convertible_array<Json, T, D>
        [[nodiscard]]
        T  to_or( T default_result, D default_range_elem = D{} ) const {
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
        requires convertible<Json, T> || convertible_map<Json, T, D> || convertible_array<Json, T, D>
        [[nodiscard]]
        std::optional<T>  move_if( D default_range_elem = D{} ) {
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
            if constexpr (std::is_constructible_v<T, Json>) {
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
            if constexpr ( convertible_map<Json, T, D> ) {
                if (type() == Type::eObject) {
                    T result{};
                    for (auto& [key, value] : std::get<Object>(m_data)) {
                        auto val = value.template move_if<typename T::mapped_type>();
                        if (!val) result.emplace(static_cast<typename T::key_type>(key), static_cast<typename T::mapped_type>(default_range_elem));
                        else result.emplace(static_cast<typename T::key_type>(key), std::move(*val));
                    }
                    return result;
                }
            }
            if constexpr ( convertible_array<Json, T, D> ) {
                if (type() == Type::eArray) {
                    T result{};
                    for (auto& value : std::get<Array>(m_data)) {
                        auto val = value.template move_if<typename T::value_type>();
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
        requires convertible<Json, T> || convertible_map<Json, T, D> || convertible_array<Json, T, D>
        [[nodiscard]]
        T  move( D default_range_elem = D{} ) {
            auto opt = move_if<T, D>( std::move(default_range_elem) );
            if (!opt) throw std::runtime_error("Cast fail.");
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
        requires convertible<Json, T> || convertible_map<Json, T, D> || convertible_array<Json, T, D>
        [[nodiscard]]
        T  move_or( T default_result, D default_range_elem = D{} ) {
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
        bool operator==(const Json& other) const noexcept {
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
        requires (!std::is_same_v<T, Json>)
        [[nodiscard]]
        bool operator==(const T& other) const {
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
            } else if constexpr (std::equality_comparable<T> && std::is_constructible_v<T, Json>) {
                return other == static_cast<T>(*this);     // Use T's operator==
            } else if constexpr (std::is_constructible_v<Json, T>) {
                return static_cast<Json>(other) == *this; // Use Value's operator==
            }
            return false;
        }

    };

}

export namespace vct::tools {
    /**
     * @brief Alias for the Json class with default template parameters.
     *
     * Use `std::allocator` for memory allocation,
     * Use `std::map` for Object, `std::vector` for Array, `std::string` for String.
     */
    using Json = ::vct::tools::json::Json<>;
}
