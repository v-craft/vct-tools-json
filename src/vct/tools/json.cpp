/**
 * @file tools_json.cpp
 * @brief JSON parsing and serialization implementation
 */

module vct.tools.json;

import std;


namespace vct::tools::json{

/**
 * @brief Escape special characters in JSON string
 * @param str String to escape
 * @return Escaped string with quotes
 */
static String escape(const std::string_view str) noexcept {
    String result;
    result.reserve(str.size() + (str.size() >> 1) + 3);
    
    result.push_back('"');
    for (const char c : str) {
        switch (c) {
            case '\\': result.append(R"(\\)"); break;
            case '"':  result.append(R"(\")"); break;
            case '\r': result.append(R"(\r)"); break;
            case '\n': result.append(R"(\n)"); break;
            case '\f': result.append(R"(\f)"); break;
            case '\t': result.append(R"(\t)"); break;
            case '\b': result.append(R"(\b)"); break;
            default: {
                if (static_cast<unsigned char>(c) < 0x20) {
                    // Handle control characters (0x00-0x1F except already handled ones)
                    result.append(std::format("\\u{:04x}", static_cast<unsigned char>(c)));
                } else result.push_back(c);
            } break;
        }
    }
    result.push_back('"');
    return result;
}

/**
 * @brief Lookup table for hexadecimal character to digit conversion
 */
static constexpr std::array<std::uint8_t, 256> hex_table = []() {
    std::array<std::uint8_t, 256> table{};
    for (int i = 0; i < 256; ++i) table[i] = 255; // Invalid marker
    for (int i = 0; i <= 9; ++i) table['0' + i] = i;
    for (int i = 0; i <= 5; ++i) table['A' + i] = 10 + i;
    for (int i = 0; i <= 5; ++i) table['a' + i] = 10 + i;
    return table;
}();

/**
 * @brief Concept for character iterators used in JSON parsing
 */
template<typename T>
concept char_iterator =  std::disjunction_v<
    std::is_same<T, std::string_view::const_iterator>,
    std::is_same<T, std::istreambuf_iterator<char>>
>;

/**
 * @brief Parse 4 hexadecimal characters into a 32-bit value
 * @param it Iterator positioned at first hex character
 * @param end_ptr End iterator
 * @return Parsed value or max uint32_t on error
 */
static std::uint32_t hex4_next(char_iterator auto& it_ref, const char_iterator auto end_ptr) noexcept {
    auto it = it_ref;
    // `it` wat in `\uABCD`'s A position, and not be end_ptr
    std::uint32_t result{ 0 };
    
    const std::uint8_t d1 = hex_table[static_cast<unsigned char>(*it)];
    if (d1 == 255) return std::numeric_limits<std::uint32_t>::max(); // Invalid if not a hex digit
    result = (result << 4) | d1;

    ++it; // External `++it` may reduce some instructions
    if(it == end_ptr) return std::numeric_limits<std::uint32_t>::max(); // Invalid if not enough characters
    const std::uint8_t d2 = hex_table[static_cast<unsigned char>(*it)];
    if (d2 == 255) return std::numeric_limits<std::uint32_t>::max(); // Invalid if not a hex digit
    result = (result << 4) | d2;

    ++it;
    if(it == end_ptr) return std::numeric_limits<std::uint32_t>::max(); // Invalid if not enough characters
    const std::uint8_t d3 = hex_table[static_cast<unsigned char>(*it)];
    if (d3 == 255) return std::numeric_limits<std::uint32_t>::max(); // Invalid if not a hex digit
    result = (result << 4) | d3;

    ++it;
    if(it == end_ptr) return std::numeric_limits<std::uint32_t>::max(); // Invalid if not enough characters
    const std::uint8_t d4 = hex_table[static_cast<unsigned char>(*it)];
    if (d4 == 255) return std::numeric_limits<std::uint32_t>::max(); // Invalid if not a hex digit
    result = (result << 4) | d4;

    // `it` wat in `\uABCD`'s D position
    it_ref = it; // Update the original iterator
    return result;
}

/**
 * @brief Convert Unicode escape sequence to UTF-8 string
 * @param it Iterator positioned at 'u' in \uXXXX
 * @param end_ptr End iterator
 * @return UTF-8 encoded string or nullopt on error
 */
static std::optional<String> unescape_unicode_next(
    char_iterator auto& it_ref, 
    const char_iterator auto end_ptr
) noexcept {
    auto it = it_ref;
    // it was in `\uABCD`'s `u` position
    String out;
    out.reserve( 4 );

    ++it;
    if (it == end_ptr) return std::nullopt;
    // `it` was in `\uXXXX`'s A position

    // move to \uABCD's D position and get hex4 value
    std::uint32_t code_point = hex4_next( it , end_ptr);
    if(code_point > 0xFFFF) return std::nullopt;
    // `it` was in `\uXXXX`'s D position and not be `end_ptr`, if hex4_next successful

    // [0xD800 , 0xE000) is agent pair, which is two consecutive \u encoding
    if (code_point >= 0xD800 && code_point <= 0xDFFF) {
        // agent pair, must be high agent + low agent
        // high agent [\uD800, \uDBFF]
        // low agent [\uDC00, \uDFFF]

        // first char must be high agent
        if (code_point >= 0xDC00) return std::nullopt;

        // second char must be low agent
        ++it;
        if(it == end_ptr || *it != '\\') return std::nullopt;
        ++it;
        if(it == end_ptr || *it != 'u') return std::nullopt;
        ++it;
        if(it == end_ptr) return std::nullopt;

        // `it` was in `\uXXXX`'s A position, and note be end_ptr

        // move to \uABCD's D position and get hex4 value
        const std::uint32_t low_code_point = hex4_next( it, end_ptr );
        if( 0xDFFF < low_code_point ||  low_code_point < 0xDC00 ) return std::nullopt;
        // `it` was in `\uXXXX`'s D position and not be `end_ptr`, if hex4_next successful

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

    it_ref = it; // Update the original iterator
    return out;
}

/**
 * @brief Parse and unescape JSON string
 * @param it Iterator positioned at opening quote
 * @param end_ptr End iterator
 * @return Unescaped string or ParseError
 */
static std::expected<std::string, ParseError> unescape_next(
    char_iterator auto& it_ref, 
    const char_iterator auto end_ptr
) noexcept {
    auto it = it_ref; // Make a copy of the iterator, Accelerate operation speed
    std::string res;

    for (++it; it != end_ptr && *it != '\"'; ++it) {
        switch (*it) {
            case '\t': case '\n': case '\f': case '\b': case '\r':
                return std::unexpected( ParseError::eIllegalEscape );
            case '\\': {
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
                    case 'u': case 'U': {
                        const auto str = unescape_unicode_next(it, end_ptr);
                        if (!str) return std::unexpected( ParseError::eIllegalEscape );
                        res.append( *str );
                    } break;
                    default: return std::unexpected( ParseError::eIllegalEscape );
                }
            } break;
            default: res.push_back( *it ); break;
        }
    }
    if(it == end_ptr) return std::unexpected( ParseError::eUnclosedString );
    ++it;
    it_ref = it; // Update the original iterator
    return res;
}

/**
 * @brief Recursive JSON value parser
 * @param it Current iterator position
 * @param end_ptr End iterator
 * @param max_depth Maximum recursion depth
 * @return Parsed JSON value or ParseError
 */
static std::expected<Value, ParseError> reader(
    char_iterator auto& it_ref, 
    const char_iterator auto end_ptr, 
    const std::int32_t max_depth
) noexcept {
    // Check for maximum depth
    if(max_depth < 0) return std::unexpected( ParseError::eDepthExceeded );
    auto it = it_ref; // Make a copy of the iterator, Accelerate operation speed
    // Skip spaces
    while(it != end_ptr && std::isspace(*it)) ++it;
    if(it == end_ptr) return std::unexpected( ParseError::eEmptyData );
    // return value
    Value json; 
    // Check the first character to determine the type
    switch (*it) {
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
                if(it == end_ptr) break;
                if(*it == ',') ++it;
                else if(*it != ']') return std::unexpected( ParseError::eUnknownFormat );
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
            if(* it == 'e' || *it == 'E' ) {
                return std::unexpected( ParseError::eUnknownFormat );
            } // begin with e/E is invalid, other invalid type will be handled after

            // number
            std::uint8_t buffer_len = 0;
            std::array<char, 24> buffer; // Reserve enough space for typical numbers

            while(buffer_len < 24 && it != end_ptr && 
                (std::isdigit(*it)  || *it=='-' || *it=='.' || *it=='e' || *it=='E' || *it=='+')
            ) buffer[buffer_len++] = *(it++);
            
            if( buffer_len == 0  ) return std::unexpected( ParseError::eInvalidNumber );
            if( buffer_len == 24 ) return std::unexpected( ParseError::eInvalidNumber );
            if( it != end_ptr && !std::isspace(*it) && *it != '}' && *it != ']' && *it != ',' ) return std::unexpected( ParseError::eInvalidNumber );

            Number value;
            const auto [ptr, ec] = std::from_chars(buffer.begin(), buffer.begin()+buffer_len, value);
            if(ec != std::errc{} || ptr != buffer.begin() + buffer_len) {
                return std::unexpected( ParseError::eInvalidNumber );
            }
            json = value;
        } break;
    }
    it_ref = it; // Update the original iterator
    return json;
}

/**
 * @brief Parse JSON from string view
 * @param text JSON text to parse
 * @param max_depth Maximum nesting depth
 * @return Parsed JSON value or ParseError
 */
std::expected<Value, ParseError> deserialize(const std::string_view text, const std::int32_t max_depth) noexcept{
    auto it = text.begin();
    const auto end_ptr = text.end();
    const auto result = reader(it, end_ptr, max_depth-1);
    if(!result) return std::unexpected( result.error() );
    while(it != end_ptr && std::isspace(*it)) ++it;
    if(it != end_ptr) return std::unexpected( ParseError::eRedundentText );
    return result;
}

/**
 * @brief Parse JSON from input stream
 * @param is_test Input stream containing JSON
 * @param max_depth Maximum nesting depth
 * @return Parsed JSON value or ParseError
 */
std::expected<Value, ParseError> deserialize(std::istream& is_test, const std::int32_t max_depth) noexcept{
    auto it = std::istreambuf_iterator<char>(is_test);
    const auto end_ptr = std::istreambuf_iterator<char>();
    const auto result = reader(it, end_ptr, max_depth-1);
    if(!result) return std::unexpected( result.error() );
    while(it != end_ptr && std::isspace(*it)) ++it;
    if(it != end_ptr) return std::unexpected( ParseError::eRedundentText );
    return result;
}

/**
 * @brief Serialize JSON value to compact string format
 * @param out Output string to append to
 */
void Value::serialize_to(String& out) const noexcept {
    switch (m_type) {
        case Type::eObject: {
            out.push_back('{');
            for (const auto& [key, val] : std::get<Object>(m_data)) {
                out.append(escape(key));
                out.push_back(':');
                val.serialize_to(out);
                out.push_back(',');
            }
            if (*out.rbegin() == ',') *out.rbegin() = '}';
            else out.push_back('}');
        } break;
        case Type::eArray: {
            out.push_back('[');
            for (const auto& val : std::get<Array>(m_data)) {
                val.serialize_to(out);
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
            out.append(escape(std::get<String>(m_data)));
            break;
        case Type::eNumber:
            out.append(std::format("{:.17}",std::get<Number>(m_data)));
            break;
    }
}

/**
 * @brief Serialize JSON value to compact stream format
 * @param out Output stream to write to
 */
void Value::serialize_to(std::ostream& out) const noexcept {
    if(out.fail()) return;
    switch (m_type) {
        case Type::eObject: {
            out.put('{');
            for(bool first = true;
                const auto& [key, val] : std::get<Object>(m_data)
            ) {
                if(!first) out.put(',');
                else first = false;
                out << escape(key);
                out.put(':');
                val.serialize_to(out);
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
                val.serialize_to(out);
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
            out << escape(std::get<String>(m_data));
            break;
        case Type::eNumber:
            out << std::format("{:.17}",std::get<Number>(m_data));
            break;
    }
    if (out.fail()) return;
}

/**
 * @brief Serialize JSON value to pretty-printed string format
 * @param out Output string to append to
 * @param space_num Number of spaces per indentation level
 * @param depth Current nesting depth
 * @param max_space Maximum allowed spaces (depth protection)
 * @return true on success, false if max_space exceeded
 */
Bool Value::serialize_pretty_to(
    String& out, 
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
                if(!val.serialize_pretty_to(out, space_num, depth + 1, max_space)) return false;
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
                if(!val.serialize_pretty_to(out, space_num, depth + 1, max_space)) return false;
                out.push_back(',');
            }
            if (*out.rbegin() == ',') *out.rbegin() = '\n';
            if(!std::get<Array>(m_data).empty()){
                out.append(tabs - space_num, ' ');
                out.push_back(']');
            } else out.append(" ]");
        } break;
        case Type::eBool:
            out.append(std::get<Bool>(m_data) ? "true" : "false");
            break;
        case Type::eNull:
            out.append("null");
            break;
        case Type::eString:
            out.append(escape(std::get<String>(m_data)));
            break;
        case Type::eNumber:
            out.append(std::format("{:.17}",std::get<Number>(m_data)));
            break;
    }
    return true;
}

/**
 * @brief Serialize JSON value to pretty-printed stream format
 * @param out Output stream to write to
 * @param space_num Number of spaces per indentation level
 * @param depth Current nesting depth
 * @param max_space Maximum allowed spaces (depth protection)
 * @return true on success, false if max_space exceeded or stream error
 */
Bool Value::serialize_pretty_to(
    std::ostream& out, 
    const std::uint16_t space_num, 
    const std::uint16_t depth, 
    const std::uint32_t max_space
) const noexcept {
    if(out.fail()) return false;
    const std::uint32_t tabs  = depth * space_num + space_num;
    if(tabs > max_space) return false;

    switch (m_type) {
        case Type::eObject: {
            out.put('{');
            bool first = true;
            for(const auto& [key, val] : std::get<Object>(m_data)) {
                if(!first) out.put(',');
                else first = false;
                out.put('\n');
                out << std::setfill(' ') << std::setw(tabs) << "";
                out << escape(key);
                out.put(':');
                out.put(' ');
                if(!val.serialize_pretty_to(out, space_num, depth + 1, max_space)) return false;
            }
            if(!first) out.put('\n');
            if(!std::get<Object>(m_data).empty()){
                out << std::setfill(' ') << std::setw(tabs - space_num) << "";
                out.put('}');
            } else {
                out.put(' ');
                out.put('}');
            }
        } break;
        case Type::eArray: {
            out.put('[');
            bool first = true;
            for (const auto& val : std::get<Array>(m_data)) {
                if(!first) out.put(',');
                else first = false;
                out.put('\n');
                out << std::setfill(' ') << std::setw(tabs) << "";
                if(!val.serialize_pretty_to(out, space_num, depth + 1, max_space)) return false;
            }
            if(!first) out.put('\n');
            if(!std::get<Array>(m_data).empty()){
                out << std::setfill(' ') << std::setw(tabs - space_num) << "";
                out.put(']');
            } else {
                out.put(' ');
                out.put(']');
            }
        } break;
        case Type::eBool:
            out << (std::get<Bool>(m_data) ? "true" : "false");
            break;
        case Type::eNull:
            out << "null";
            break;
        case Type::eString:
            out << escape(std::get<String>(m_data));
            break;
        case Type::eNumber:
            out << std::format("{:.17}",std::get<Number>(m_data));
            break;
    }
    if(out.fail()) return false;
    return true;
}


}

