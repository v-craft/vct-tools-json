# **json::convertible_map**

```cpp
template<typename T, typename D>
concept convertible_map = std::ranges::range<T> && requires {
    typename T::key_type;
    typename T::mapped_type;
    requires std::is_constructible_v<typename T::mapped_type, D>;
    requires std::is_convertible_v<String, typename T::key_type>;
    requires convertible<typename T::mapped_type>;
    requires std::is_default_constructible_v<T>;
    requires std::is_default_constructible_v<typename T::mapped_type>;
    requires std::is_copy_constructible_v<typename T::mapped_type>;
} && requires (T t, String s, typename T::mapped_type m) {
    t.emplace(static_cast<typename T::key_type>(s), std::move(m));
};
```

Mapping types that can be directly or **indirectly** converted, which can also be used in `to/move` member functions of [`Value`](../value/Value.md).

The key requirements include the following:

- Type `T` must be a range and key-value constructure.
- `key_type` must be convertible from `json::String (std::string)`.
- `mapped_type` is [`json::convertible`](./convertible.md) .
- `D` usually refers to `mapped_type`, but can also be other types that can be converted.

This concept is important as it allows converting [`json::Value`](../value/Value.md) to types such as `std::map<std::string,int>`.

## Version

From v1.0.0 to now.
