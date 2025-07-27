# **convertible**

```cpp
template<typename J, typename T>
concept convertible = std::disjunction_v<
    std::is_arithmetic<T>,
    std::is_enum<T>,
    std::is_convertible<typename J::Array, T>,
    std::is_convertible<typename J::Object, T>,
    std::is_convertible<typename J::String, T>,
    std::is_convertible<typename J::Number, T>,
    std::is_convertible<typename J::Bool, T>,
    std::is_convertible<typename J::Null, T>,
    std::is_constructible<T, J>
>;
```

位于 `vct::tools::json` 命名空间中，用于表示某个 `Json` 容器类型 `J` 可以尝试使用 `to/move` 成员函数将内部数据转换为类型 `T`。

满足此概念仅代表 `T` 可以作为 `to/move` 的模板形参，但转换是否成功取决于 JSON 容器对象的内部数据情况。

## 版本

v0.8.0 至今。

