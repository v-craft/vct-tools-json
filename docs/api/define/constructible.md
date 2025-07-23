# **json::constructible**

```cpp
template<typename T>
concept constructible = std::disjunction_v<
    std::is_arithmetic<T>,
    std::is_enum<T>,
    std::is_convertible<T, Array>,
    std::is_convertible<T, Object>,
    std::is_convertible<T, String>,
    std::is_convertible<T, Number>,
    std::is_convertible<T, Bool>,
    std::is_convertible<T, Null>
>;
```

Can be used as an implicit constructor type.

## Note

`enum` types are treated as their underlying integer values.

## Version

From v1.0.0 to now.

