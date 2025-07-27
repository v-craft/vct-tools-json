# **constructible_map**

```cpp
template<typename J, typename T>
concept constructible_map = std::ranges::range<T> &&
    std::is_convertible_v<typename T::key_type, typename J::String> &&
    std::is_constructible_v<J, typename T::mapped_type>;
```

位于 `vct::tools::json` 命名空间中，用于表示某个键值对形式的类型，可以作为 JSON 容器类型 `J` 的构造函数参数。

如果已经满足 [`constructible`](./constructible.md) 概念，则可作为隐式构造或赋值运算符的参数。
如果不满足 [`constructible`](./constructible.md) 概念，但满足此概念，则只能作为**显式构造**的参数，不能直接用于赋值。

如果同时满足此概念和 [`constructible_array`](./constructible_array.md) 概念，则优先视作 `map` 而不是 `array`。

## 版本

v0.8.0 至今。


