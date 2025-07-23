# **json::ParseError**

```cpp
enum class ParseError {
    eNone = 0,          // No error
    eEmptyData,         // The input data is empty
    eRedundantText,     // There is redundant un-space text after the JSON data.
    eDepthExceeded,     // The maximum depth of nested structures has been exceeded
    eIllegalEscape,     // An illegal escape sequence was encountered
    eInvalidNumber,     // An invalid number format was encountered
    eUnclosedString,    // A string was not properly closed
    eUnclosedObject,    // An object was not properly closed
    eUnclosedArray,     // An array was not properly closed
    eUnknownFormat      // Other format error
};
```

This enumeration is only returned by the 'parse' function and is used to indicate the reason for parsing failure.

> You can use the [`error_name()`](./error_name.md) function to obtain the string name.

## Note

The reason for the failure is not accurate, and specific enumeration values are recommended for debugging or logging purposes only.

## Version

From v1.0.0 to now.

