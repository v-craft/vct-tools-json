# **json::convertible**

```cpp
template<typename T>
concept convertible = std::disjunction_v<
    std::is_arithmetic<T>,
    std::is_enum<T>,
    std::is_convertible<Array, T>,
    std::is_convertible<Object, T>,
    std::is_convertible<String, T>,
    std::is_convertible<Number, T>,
    std::is_convertible<Bool, T>,
    std::is_convertible<Null, T>,
    std::is_constructible<T, Value>
>;
```

The **directly** available types for 'to/move' member function in [`Value`](../value/Value.md), including [`json_type`](./json_type.md) .

This concept includes:

1. Primitive Types \(`int`, `float`, etc.\)
2. Enumeration Types \(Treated as their underlying integer values\)
3. JSON Native Types \(`json::Array`, `json::Object`, etc.\)
4. Implicitly Convertible Types from JSON Native Types \( `std::string_view` \)
5. Custom Constructible Types which  providing a constructor for `Value`

## Version

From v1.0.0 to now.
