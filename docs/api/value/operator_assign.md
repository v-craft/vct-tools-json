# **json::Value::operator=**

```cpp
// 1
Value& operator=(const Value&) = default;
// 2
Value& operator=(Value&& other) noexcept;
// 3
template<typename T>
requires constructible<std::remove_cvref_t<T>>
Value& operator=(T&& other) noexcept(
    std::is_rvalue_reference_v<T&&> ||
    std::is_arithmetic_v<std::remove_cvref_t<T>> ||
    std::is_enum_v<std::remove_cvref_t<T>> ||
    std::is_same_v<std::remove_cvref_t<T>, Null>
);
```

## Complexity

1. Linear.
2. Constant.
3. Constant for simple types or move, linear for complex types with copy.

## Version

From v1.0.0 to now.
