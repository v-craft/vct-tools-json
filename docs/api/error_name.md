# **json::error_name**

```cpp
constexpr const char* error_name(const ParseError error) noexcept;
```

Returns a string representation of the given [`json::ParseError`](./ParseError.md) enumeration value.

> This function is recommended for debugging or logging purposes only.

??? note "implementation"
    ```cpp
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
            default: return "Unknown Enum Value";
        }
    }
    ```

## Version

From v1.0.0 to now.
