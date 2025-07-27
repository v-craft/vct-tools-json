# **Json.constructor**

```cpp
// 1
constexpr Json() noexcept = default;

//  2
Json(const Json&) = default;

// 3
Json(Json&& other) noexcept;

// 4
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
);

// 5
template<typename T>
requires !constructible<Json, std::remove_cvref_t<T>> && !constructible_map<Json, std::remove_cvref_t<T>> && constructible_array<Json, std::remove_cvref_t<T>>
explicit Json(T&& other);

// 6
template<typename T>
requires !constructible<Json, std::remove_cvref_t<T>> && constructible_map<Json, std::remove_cvref_t<T>>
explicit Json(T&& other);
```

1. 默认构造函数，初始化为 `Null`。

2. 拷贝构造函数，创建一个新的 `Json` 对象，复制给定的 JSON 值。

3. 移动构造函数，将给定的 JSON 值移动到新的 `Json` 对象中，并将源对象的状态更改为 `Null`，无异常。

4. 隐式构造函数，接受任何可以转换为 JSON 值的类型，如基础算术类型与枚举（视为整数）、六种内部 JSON 数据类型以及提供了转换函数的类型。
仅当数据是简单类型，或右值的复杂类型时才保证无异常。

5. 显式构造函数，接受可以转换为 JSON 数组的类型，如 `std::vector`、`std::list` 等，内部元素也需要可构造。

6. 显式构造函数，接受可以转换为 JSON 对象的类型，如 `std::map`、`std::unordered_map` 等，内部元素也需要可构造。

1. Create a default `Value` object, which is initialized to `Null`.

## 异常

可通过编译时一般无异常发生。

在少数情况下，但由于内存分配问题（比如内存不足），在运行时可能出现 `bad_alloc` 等异常。

## 复杂度

1. 常数。
2. 线性。
3. 常数。
4. 简单类型为常数，复制类型为线性，能够直接移动时为常数。
5. 线性。
6. 线性。

## 版本

v0.8.0 至今。

