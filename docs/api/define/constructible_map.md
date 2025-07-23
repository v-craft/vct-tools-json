# **json::constructible_map**

```cpp
template<typename T>
concept constructible_map = std::ranges::range<T> && requires {
    typename T::key_type;
    typename T::mapped_type;
    requires std::is_convertible_v<typename T::key_type, String>;
    requires std::is_constructible_v<Value, typename T::mapped_type>;
};
```

Can be used as an explicit constructor key-val type.

## Version

From v1.0.0 to now.

