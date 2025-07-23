# **json::json_type**

```cpp
template<typename T>
concept json_type = std::disjunction_v<
    std::is_same<T, Null>,
    std::is_same<T, Bool>,
    std::is_same<T, Number>,
    std::is_same<T, String>,
    std::is_same<T, Array>,
    std::is_same<T, Object>
>;
```

This concept is used for constraint types, and only includes the six JSON data types used in this library.

## Version

From v1.0.0 to now.
