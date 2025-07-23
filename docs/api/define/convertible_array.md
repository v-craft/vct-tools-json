# **json::convertible_array**

```cpp
template<typename T, typename D>
concept convertible_array =  std::ranges::range<T> && requires {
    typename T::value_type;
    requires std::is_constructible_v<typename T::value_type,D>;
    requires convertible<typename T::value_type>;
    requires std::is_default_constructible_v<T>;
    requires std::is_default_constructible_v<typename T::value_type>;
    requires std::is_copy_constructible_v<typename T::value_type>;
} && requires (T t, typename T::value_type v) {
    t.emplace_back(std::move(v));
};
```

Array-like types that can be directly or **indirectly** converted, which can also be used in `to/move` member functions of [`Value`](../value/Value.md).

The key requirements include the following:

- Type `T` must be a range.
- `value_type` is [`json::convertible`](./convertible.md) .
- `D` usually refers to `value_type`, but can also be other types that can be converted.

This concept is important as it allows converting [`json::Value`](../value/Value.md) to types such as `std::vector<int>` or `std::list<std::string>`.

## Version

From v1.0.0 to now.
