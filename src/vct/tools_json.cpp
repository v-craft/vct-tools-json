module vct.tools.json;

import std;


namespace vct::tools::json{

/**
 * @brief Escape JSON string with proper quote wrapping
 * @param str String view to escape
 * @return Escaped string with surrounding quotes
 * 
 * Converts special characters to their JSON escape sequences:
 * - Backslash (\) -> \\
 * - Quote (") -> \"
 * - Control characters (newline, tab, etc.) -> \n, \t, etc.
 */
static std::string escape(const std::string_view str) noexcept {
    std::string result;
    result.reserve(str.size() + (str.size() >> 1) + 2);
    
    result.push_back('"');
    for (char c : str) {
        switch (c) {
            case '\\': result.append(R"(\\)"); break;
            case '"':  result.append(R"(\")"); break;
            case '\r': result.append(R"(\r)"); break;
            case '\n': result.append(R"(\n)"); break;
            case '\f': result.append(R"(\f)"); break;
            case '\t': result.append(R"(\t)"); break;
            case '\b': result.append(R"(\b)"); break;
            default:   result.push_back(c); break;
        }
    }
    result.push_back('"');
    return result;
}

/**
 * @brief Parse 4-digit hexadecimal value from string
 * @param p Pointer to start of hex digits
 * @return Parsed hex value, or UINT32_MAX if invalid
 * 
 * Parses exactly 4 hexadecimal characters into a 32-bit unsigned integer.
 * Used for Unicode escape sequence processing (\uXXXX).
 */
static std::uint32_t parse_hex4(const char* p) noexcept {
    static constexpr std::array<std::uint8_t, 256> hex_table = []() {
        std::array<std::uint8_t, 256> table{};
        for (int i = 0; i < 256; ++i) table[i] = 255; // Invalid marker
        for (int i = 0; i <= 9; ++i) table['0' + i] = i;
        for (int i = 0; i <= 5; ++i) table['A' + i] = 10 + i;
        for (int i = 0; i <= 5; ++i) table['a' + i] = 10 + i;
        return table;
    }();
    
    std::uint32_t result = 0;
    for (int i = 0; i < 4; ++i) {
        std::uint8_t digit = hex_table[static_cast<unsigned char>(p[i])];
        if (digit == 255) return std::numeric_limits<std::uint32_t>::max();
        result = (result << 4) | digit;
    }
    return result;
}

/**
 * @brief Parse Unicode escape sequence and convert to UTF-8
 * @param it Iterator reference, positioned at 'u' in \uXXXX
 * @param end_ptr End iterator for bounds checking
 * @return UTF-8 encoded string or nullopt if invalid
 * 
 * Handles both basic Unicode (\uXXXX) and surrogate pairs (\uD800-\uDBFF \uDC00-\uDFFF).
 * Converts Unicode code points to UTF-8 encoding:
 * - U+0000-U+007F: 1 byte
 * - U+0080-U+07FF: 2 bytes  
 * - U+0800-U+FFFF: 3 bytes
 * - U+10000-U+10FFFF: 4 bytes (via surrogate pairs)
 * 
 * @note Iterator is advanced to the last character of the escape sequence
 */
static std::optional<std::string> unescape_unicode_next(
    std::string_view::const_iterator& it, 
    const std::string_view::const_iterator end_ptr
) noexcept {
    std::string out;
    out.reserve( 4 );

    // `it` was in \uXXXX's u position, so we need at least 4 more characters.
    if (end_ptr - it <= 4) return std::nullopt; 
    ++it;

    std::uint32_t code_point = parse_hex4( &(*it) );
    if(code_point > 0xFFFF) return std::nullopt;

    // when this function returns, `it` should be at the D position of \uABCD, so we can only add 3
    it += 3;

    // [0xD800 , 0xE000) is agent pair, which is two consecutive \u encoding
    if (code_point >= 0xD800 && code_point <= 0xDFFF) {
        // agent pair, must be high agent + low agent
        // high agent [\uD800, \uDBFF]
        // low agent [\uDC00, \uDFFF]

        // first char must be high agent
        if (code_point >= 0xDC00) return std::nullopt;

        // second char must be low agent
        if (end_ptr - it < 7 || *(it+1) != '\\' || *(it+2) != 'u') return std::nullopt;

        it += 3; // move to \uABCD's A position

        std::uint32_t low_code_point = parse_hex4( &(*it) );
        if( low_code_point < 0xDC00 || low_code_point > 0xDFFF  ) return std::nullopt;
    
        it += 3; // move to \uABCD's D position

        // combine the agent pair into a single code point
        code_point = 0x10000 + ((code_point - 0xD800) << 10) + (low_code_point - 0xDC00);
    }

    // encode the code point to UTF-8
    if (code_point <= 0x7F) {
        out += static_cast<char>(code_point);
    } else if (code_point <= 0x7FF) {
        out += static_cast<char>(0xC0 | (code_point >> 6));
        out += static_cast<char>(0x80 | (code_point & 0x3F));
    } else if (code_point <= 0xFFFF) {
        out += static_cast<char>(0xE0 | (code_point >> 12));
        out += static_cast<char>(0x80 | ((code_point >> 6) & 0x3F));
        out += static_cast<char>(0x80 | (code_point & 0x3F));
    } else if (code_point <= 0x10FFFF) {
        out += static_cast<char>(0xF0 | (code_point >> 18));
        out += static_cast<char>(0x80 | ((code_point >> 12) & 0x3F));
        out += static_cast<char>(0x80 | ((code_point >> 6) & 0x3F));
        out += static_cast<char>(0x80 | (code_point & 0x3F));
    } else return std::nullopt;

    return out;
}

/**
 * @brief Parse and unescape JSON string content
 * @param it Iterator reference, positioned after opening quote
 * @param end_ptr End iterator for bounds checking
 * @return Unescaped string or ParseError
 * 
 * Processes JSON string escape sequences:
 * - Basic escapes: \", \\, \/, \n, \r, \t, \f, \b
 * - Unicode escapes: \uXXXX and surrogate pairs
 * - Validates string is properly closed with ending quote
 * 
 * Performance optimized with direct character processing and minimal allocations.
 */
static std::expected<std::string, ParseError> unescape_next(
    std::string_view::const_iterator& it, 
    const std::string_view::const_iterator end_ptr
) noexcept {
    std::string res;

    for (++it; it != end_ptr && *it != '\"'; ++it) {
        switch (*it) {
            case '\t': case '\n': case '\f': case '\b': case '\r':
                return std::unexpected( ParseError::eIllegalEscape );
            case '\\': {
                ++it;
                if (it == end_ptr) return std::unexpected( ParseError::eUnclosedString );
                switch (*it) {
                    case '\"': res += '\"'; break;
                    case '\\': res += '\\'; break;
                    case '/':  res += '/';  break;
                    case 'n':  res += '\n'; break;
                    case 'r':  res += '\r'; break;
                    case 't':  res += '\t'; break;
                    case 'f':  res += '\f'; break;
                    case 'b':  res += '\b'; break;
                    case 'u': case 'U': {
                        const auto str = unescape_unicode_next(it, end_ptr);
                        if (!str) return std::unexpected( ParseError::eIllegalEscape );
                        res += *str;
                    } break;
                    default: return std::unexpected( ParseError::eIllegalEscape );
                }
            } break;
            default: res += *it; break;
        }
    }
    if(it == end_ptr) return std::unexpected( ParseError::eUnclosedString );
    ++it;
    return res;
}

/**
 * @brief Recursive JSON value parser
 * @param it Iterator reference for current parsing position
 * @param end_ptr End iterator for bounds checking
 * @param max_depth Maximum recursion depth allowed
 * @return Parsed JSON Value or ParseError
 * 
 * Recursively parses JSON values with support for:
 * - Objects: { "key": value, ... }
 * - Arrays: [ value, value, ... ]
 * - Strings: "escaped content"
 * - Numbers: integers, floats, scientific notation
 * - Booleans: true, false
 * - Null: null
 */
static std::expected<Value, ParseError> reader(
    std::string_view::const_iterator& it, 
    const std::string_view::const_iterator end_ptr, 
    const std::int32_t max_depth
) noexcept {
    // Check for maximum depth
    if(max_depth < 0) return std::unexpected( ParseError::eDepthExceeded );
    // Skip spaces
    while(it != end_ptr && std::isspace(*it)) ++it;
    if(it == end_ptr) return std::unexpected( ParseError::eEmptyData );
    // return value
    Value json; 
    // Check the first character to determine the type
    switch (*it){
        case '{': {
            // Object type
            ++it;
            json = Object{};
            auto& object = json.get_ref<Object>(); 
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
                auto value = reader(it, end_ptr, max_depth - 1);
                if(!value) return std::unexpected( value.error() );
                // add to object
                object.emplace(std::move(*key), std::move(*value));

                while(it != end_ptr && std::isspace(*it)) ++it;
                if(it != end_ptr && *it == ',') ++it;
            }
            if(it == end_ptr) return std::unexpected( ParseError::eUnclosedObject );
            ++it;
        } break;
        case '[': {
            // Array type
            ++it;
            json = Array{};
            auto& array = json.get_ref<Array>();
            while(it != end_ptr){
                // Skip spaces
                while (it != end_ptr && std::isspace(*it)) ++it;
                if(it == end_ptr || *it == ']') break;
                // find value
                auto value = reader(it, end_ptr, max_depth - 1);
                if(!value) return std::unexpected( value.error() );
                // add to array
                array.emplace_back(std::move(*value));

                while(it != end_ptr && std::isspace(*it)) ++it;
                if(it != end_ptr && *it == ',') ++it;
            }
            if(it == end_ptr) return std::unexpected( ParseError::eUnclosedArray );
            ++it;
        } break;
        case '\"': {
            // String type
            auto str = unescape_next(it, end_ptr);
            if(!str) return std::unexpected( str.error() );
            json = std::move(*str);
        } break;
        case 't': {
            // true
            if (it + 3 <= end_ptr && *it=='t' && *(it+1)=='r' && *(it+2)=='u' && *(it+3)=='e') {
                json = true;
                it += 4;
            } else return std::unexpected( ParseError::eUnknownFormat );
        } break;
        case 'f': {
            // false
            if (it + 4 <= end_ptr &&  *it=='f' && *(it+1)=='a' && *(it+2)=='l' && *(it+3)=='s' && *(it+4)=='e' ) {
                json = false;
                it += 5;
            } else return std::unexpected( ParseError::eUnknownFormat );
        } break;
        case 'n': {
            // null
            if (it + 3 <= end_ptr && *it=='n' && *(it+1)=='u' && *(it+2)=='l' && *(it+3)=='l') {
                json = nullptr;
                it += 4;
            } else return std::unexpected( ParseError::eUnknownFormat );
        } break;
        default: {
            // number
            if(!std::isdigit(*it) && *it != '-' && *it != '.') return std::unexpected( ParseError::eUnknownFormat );
            bool have_not_point = true;
            bool have_not_e = true;
            auto left = it;
            if(*it == '-') ++it;

            if(it == end_ptr || !std::isdigit(*it)) return std::unexpected( ParseError::eInvalidNumber );
            while (it != end_ptr) {
                if (std::isdigit(*it)) {
                    ++it;
                } else if (*it == '.' && have_not_point && have_not_e) {
                    have_not_point = false;
                    ++it;
                } else if ((*it == 'e' || *it == 'E') && have_not_e) {
                    have_not_e = false;
                    ++it;
                    if (it != end_ptr && (*it == '-' || *it == '+')) ++it;
                } else break;
            }
            if(it == left || (*left=='-' && it==left+1)) return std::unexpected( ParseError::eInvalidNumber );
            double value;
            auto [ptr, ec] = std::from_chars(left, it, value);
            if(ec != std::errc{} || ptr != it) {
                return std::unexpected( ParseError::eInvalidNumber );
            }
            json = value;
        } break;
    }
    return json;
}

/**
 * @brief Deserialize JSON string into Value object
 * @param text JSON string to parse
 * @param max_depth Maximum recursion depth (default: reasonable limit)
 * @return Parsed JSON Value or ParseError
 * 
 * Main entry point for JSON deserialization. Parses complete JSON document
 * and validates that no trailing content remains after parsing.
 * 
 * @throws ParseError Various parsing errors (see reader() documentation)
 * @throws ParseError::eUnknownFormat Trailing content after valid JSON
 * 
 * Usage example:
 * @code
 * auto result = json::deserialize(R"({"key": "value"})");
 * if (result) {
 *     auto& json = *result;
 *     // or: json::Value json = *result;
 *     // Use json object
 * }
 * @endcode
 */
std::expected<Value, ParseError> deserialize(const std::string_view text, const std::int32_t max_depth) noexcept{
    auto it = text.begin();
    const auto result = reader(it, text.end(), max_depth-1);
    if(!result) return std::unexpected( result.error() );
    while(it != text.end() && std::isspace(*it)) ++it;
    if(it != text.end()) return std::unexpected( ParseError::eUnknownFormat );
    return result;
}


/**
 * @brief Get string representation of current value type
 * @return Type name as string
 * 
 * Returns human-readable type names:
 * - "Object" for JSON objects
 * - "Array" for JSON arrays  
 * - "String" for JSON strings
 * - "Number" for JSON numbers
 * - "Bool" for JSON booleans
 * - "Null" for JSON null
 */
std::string Value::type_name() const noexcept {
    switch (m_type) {
        case Type::eObject: return "Object";
        case Type::eArray: return "Array";
        case Type::eString: return "String";
        case Type::eNumber: return "Number";
        case Type::eBool: return "Bool";
        case Type::eNull: return "Null";
    }
}

/**
 * @brief Reset value data to default state for current type
 * 
 * Initializes the variant data to type-appropriate default values:
 * - Object: empty map
 * - Array: empty vector
 * - String: empty string
 * - Number: 0.0
 * - Bool: false
 * - Null: nullptr
 * 
 * Used internally for type changes and initialization.
 */
void Value::clear_data() noexcept {
    switch (m_type) {
    case Type::eObject:
        m_data = Object();
        break;
    case Type::eArray:
        m_data = Array();
        break;
    case Type::eString:
        m_data = std::string();
        break;
    case Type::eNumber:
        m_data = 0.0;
        break;
    case Type::eBool:
        m_data = false;
        break;
    case Type::eNull:
        m_data = nullptr;
        break;
    }
}


/**
 * @brief Serialize JSON value to compact string format
 * @param out Output string buffer to append to
 * @return true if successful, false on error
 * 
 * Converts JSON value to compact string representation without formatting:
 * - Objects: {"key":value,"key2":value2}
 * - Arrays: [value,value,value]
 * - Strings: "escaped content"
 * - Numbers: decimal representation
 * - Booleans: true/false
 * - Null: null
 * 
 * Performance optimized with direct string appending and minimal allocations.
 */
bool Value::write_out(std::string& out) const noexcept {
    switch (m_type) {
        case Type::eObject: {
            out.push_back('{');
            for (const auto& [key, val] : std::get<Object>(m_data)) {
                out.append(escape(key));
                out.push_back(':');
                if(!val.write_out(out)) return false;
                out.push_back(',');
            }
            if (*out.rbegin() == ',') *out.rbegin() = '}';
            else out.push_back('}');
        } break;
        case Type::eArray: {
            out.push_back('[');
            for (const auto& val : std::get<Array>(m_data)) {
                if(!val.write_out(out)) return false;
                out.push_back(',');
            }
            if (*out.rbegin() == ',') *out.rbegin() = ']';
            else out.push_back(']');
        } break;
        case Type::eBool:
            out.append(std::get<bool>(m_data) ? "true" : "false");
            break;
        case Type::eNull:
            out.append("null");
            break;
        case Type::eString:
            out.append(escape(std::get<std::string>(m_data)));
            break;
        case Type::eNumber:
            out.append(std::format("{:.15}",std::get<double>(m_data)));
            break;
    }
    return true;
}

/**
 * @brief Serialize JSON value to pretty-printed string format
 * @param out Output string buffer to append to
 * @param space_num Number of spaces per indentation level
 * @param depth Current nesting depth
 * @param max_space Maximum allowed total spaces (overflow protection)
 * @return true if successful, false if max_space exceeded
 * @note Fails safely if indentation would exceed max_space limit
 */
bool Value::write_out_pretty(
    std::string& out, 
    const std::uint16_t space_num, 
    const std::uint16_t depth, 
    const std::uint32_t max_space
) const noexcept {
    const std::uint32_t tabs  = depth * space_num + space_num;
    if(tabs > max_space) return false;

    switch (m_type) {
        case Type::eObject: {
            out.push_back('{');
            for (const auto& [key, val] : std::get<Object>(m_data)) {
                out.push_back('\n');
                out.append(tabs, ' ');
                out.append(escape(key));
                out.append(": ");
                if(!val.write_out_pretty(out, space_num, depth + 1, max_space)) return false;
                out.push_back(',');
            }
            if (*out.rbegin() == ',') *out.rbegin() = '\n';
            if(!std::get<Object>(m_data).empty()){
                out.append(tabs - space_num, ' ');
                out.push_back('}');
            } else out.append(" }");
        } break;
        case Type::eArray: {
            out.push_back('[');
            for (const auto& val : std::get<Array>(m_data)) {
                out.push_back('\n');
                out.append(tabs, ' ');
                if(!val.write_out_pretty(out, space_num, depth + 1, max_space)) return false;
                out.push_back(',');
            }
            if (*out.rbegin() == ',') *out.rbegin() = '\n';
            if(!std::get<Array>(m_data).empty()){
                out.append(tabs - space_num, ' ');
                out.push_back(']');
            } else out.append(" ]");
        } break;
        case Type::eBool:
            out.append(std::get<bool>(m_data) ? "true" : "false");
            break;
        case Type::eNull:
            out.append("null");
            break;
        case Type::eString:
            out.append(escape(std::get<std::string>(m_data)));
            break;
        case Type::eNumber:
            out.append(std::format("{:.15}",std::get<double>(m_data)));
            break;
    }
    return true;
}

/**
 * @brief Serialize JSON value to compact string
 * @return JSON string representation
 * 
 * Convenience wrapper around write_out() that returns a new string.
 * For performance-critical code, consider using write_out() directly
 * to append to an existing buffer.
 */
std::string Value::serialize() const noexcept {
    std::string res;
    write_out(res);
    return res;
}

/**
 * @brief Serialize JSON value to pretty-printed string
 * @param space_num Number of spaces per indentation level
 * @param depth Starting nesting depth
 * @param max_space Maximum allowed total spaces
 * @return Formatted JSON string or nullopt if space limit exceeded
 * 
 * Convenience wrapper around write_out_pretty() that returns a new string.
 * Returns nullopt if formatting would exceed the space limit.
 * 
 * Typical usage:
 * @code
 * auto pretty = value.serialize_pretty(2, 0, 10000);
 * if (pretty) {
 *     std::cout << *pretty << std::endl;
 * }
 * @endcode
 */
std::optional<std::string> Value::serialize_pretty(
    const std::uint16_t space_num, 
    const std::uint16_t depth, 
    const std::uint32_t max_space 
) const noexcept {
    std::string res;
    if(!write_out_pretty(res, space_num, depth, max_space)) return std::nullopt;
    return res;
}

}

