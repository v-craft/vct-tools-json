# **json::constructible_array**

```cpp
template<typename T>
concept constructible_array = std::ranges::range<T> && requires {
    typename T::value_type;
    requires std::is_constructible_v<Value, typename T::value_type>;
};
```

Can be used as an explicit constructor Array-like type.


## Version

From v1.0.0 to now.

