# **convertible_map**

```cpp
template<typename J, typename T, typename D>
concept convertible_map = std::ranges::range<T> && requires {
    requires std::is_constructible_v<typename T::mapped_type, D>;
    requires std::is_convertible_v<typename J::String, typename T::key_type>;
    requires convertible<J, typename T::mapped_type>;
    requires std::is_default_constructible_v<T>;
    requires std::is_default_constructible_v<typename T::mapped_type>;
    requires std::is_copy_constructible_v<typename T::mapped_type>;
} && requires (T t, typename J::String s, typename T::mapped_type m) {
    t.emplace(static_cast<typename T::key_type>(s), std::move(m));
};
```

位于 `vct::tools::json` 命名空间中，用于表示某个 `Json` 容器类型 `J` 可以尝试使用 `to/move` 成员函数将内部数据转换为键值对类型 `T`。

满足此概念仅代表 `T` 可以作为 `to/move` 的模板形参，但转换是否成功取决于 JSON 容器对象的内部数据情况。

模板形参 `D` 用于表示键值对类型 `T` 的默认值类型，可以不是 `mapped_type` 类型，但需要能够转换成 `mapped_type` 。
这用于在转换函数中提供子对象的默认指。

## 版本

v0.8.0 至今。



