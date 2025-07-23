# **json::parse**

```cpp
std::expected<Value, ParseError> parse(
    std::string_view text,      // from string_view
    std::int32_t max_depth = 256 
) noexcept;

std::expected<Value, ParseError> parse(
    std::istream& is_text,      // from input stream
    std::int32_t max_depth = 256 
) noexcept;
```

This function is used to parse a JSON string or stream into a `json::Value` object.

## Parameters

- `text`: A `std::string_view` containing the JSON text to be parsed.

- `is_text`: An input stream from which the JSON text will be read.

- `max_depth`: The maximum depth of nested structures allowed during parsing.
If the JSON exceeds this depth, a `ParseError::eDepthExceeded` error will be returned. The default value is 256.


## Return value

- **On success**: Returns a [`Value`](./value/Value.md) object containing the parsed JSON data.

- **On failure**: Returns a [`ParseError`](./ParseError.md) enum value indicating the reason for the parsing failure. 

## Exception

Noexcept.

## Notes

JSON file parsing failures are common and unpredictable in real applications, as there can easily be format errors or garbage data.
Therefore, this library's deserialization functions return `std::expected<>`, avoiding the exception mechanism and reducing overhead.

This function also has an optional parameter `max_depth` (default is 256) to limit the maximum (nesting) depth of parsing.
Although this library guarantees that total parsing complexity is `O(n)` (strict single traversal), it uses recursion to handle nested structures.
So this parameter is needed to avoid issues with certain garbage data (such as overly long `[[[[[[]]]]]]` which might cause stack overflow during recursion).

Streaming file parsing efficiency is almost equivalent to first reading the entire file into a string and then parsing the string, but memory usage may be less.

## Examples

??? note "Example"
    ```cpp
    // Parsing from a string
    std::string str = R"( [1, true, null, "string", {"key": false }] )";
    json::Value json = json::parse(pretty_str).value_or( nullptr );
    
    // Parsing from an input stream
    std::ifstream file( "xxxxx.json" );
    auto json = json::parse(file).value_or( nullptr );
    ```

## Complexity

Linear in the length of the input.

## Version

From v1.0.0 to now.


