# **json::Value::constructor**

```cpp
// 1
Value() noexcept = default;

//  2
Value(const Value&) = default; 

// 3
Value(Value&& other) noexcept;

// 4
template<typename T>
requires constructible<std::remove_cvref_t<T>>
Value(T&& other) noexcept (
    std::is_rvalue_reference_v<T&&> ||
    std::is_arithmetic_v<std::remove_cvref_t<T>> ||
    std::is_enum_v<std::remove_cvref_t<T>> ||
    std::is_same_v<std::remove_cvref_t<T>, Null>
);

// 5
template<typename T>
requires !constructible<std::remove_cvref_t<T>> && !constructible_map<std::remove_cvref_t<T>> && constructible_array<std::remove_cvref_t<T>>
explicit Value(T&& other);

// 6
template<typename T>
requires !constructible<std::remove_cvref_t<T>> && constructible_map<std::remove_cvref_t<T>>
explicit Value(T&& other);
```


1. Create a default `Value` object, which is initialized to `Null`.

2. Creates a copy of a given JSON value.

3. Move constructor, and change target state to `Null`.

4. Implicit constructor that accepts any type that can be converted to a JSON value, such as `int`, `double`, `std::string`, etc. 
If the type is an arithmetic type or an enum, it will be converted to a number; if it is a `Null`, it will be converted to a `Null` JSON value.

5. Explicit constructor that accepts a type that can be converted to a JSON array, such as `std::vector`, `std::list`, etc.

6. Explicit constructor that accepts a type that can be converted to a JSON object, such as `std::map`, `std::unordered_map`, etc.

## Complexity

1. Constant.
2. Linear.
3. Constant.
4. Constant for simple types or move, linear for complex types with copy.
5. Linear.
6. Linear.

## Version

From v1.0.0 to now.
