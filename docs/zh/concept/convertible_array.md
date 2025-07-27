# **convertible_array**

```cpp
template<typename J, typename T, typename D>
concept convertible_array =  std::ranges::range<T> && requires {
    requires std::is_constructible_v<typename T::value_type,D>;
    requires convertible<J, typename T::value_type>;
    requires std::is_default_constructible_v<T>;
    requires std::is_default_constructible_v<typename T::value_type>;
    requires std::is_copy_constructible_v<typename T::value_type>;
} && requires (T t, typename T::value_type v) {
    t.emplace_back(std::move(v));
};
```

位于 `vct::tools::json` 命名空间中，用于表示某个 `Json` 容器类型 `J` 可以尝试使用 `to/move` 成员函数将内部数据转换为数组类型 `T`。

满足此概念仅代表 `T` 可以作为 `to/move` 的模板形参，但转换是否成功取决于 JSON 容器对象的内部数据情况。

模板形参 `D` 用于表示数组类型 `T` 的默认值类型，可以不是 `value_type` 类型，但需要能够转换成 `value_type` 。
这用于在转换函数中提供子对象的默认指。

## 版本

v0.8.0 至今。





