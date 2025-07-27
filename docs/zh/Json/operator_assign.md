# **Json.operator=**

```cpp
// 1
Json& operator=(const Json&) = default;

// 2
Json& operator=(Json&& other) noexcept;

// 3
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
);
```

赋值运算符重载。

## 复杂度

1. 线性。
2. 常数。
3. 简单类型或直接移动时为常数，其他类型为线性。

## 版本

v0.8.0 至今。

