# **constructible**

```cpp
template<typename J, typename T>
concept constructible = std::disjunction_v<
    std::is_arithmetic<T>,
    std::is_enum<T>,
    std::is_convertible<T, typename J::Array>,
    std::is_convertible<T, typename J::Object>,
    std::is_convertible<T, typename J::String>,
    std::is_convertible<T, typename J::Number>,
    std::is_convertible<T, typename J::Bool>,
    std::is_convertible<T, typename J::Null>
>;
```

位于 `vct::tools::json` 命名空间中，用于表示某个 `Json` 容器类型 `J` 可以**直接**通过类型 `T` 构造。

`J` 必须是 `json::Json` 类模板的实例化类型。
满足此概念的类型可以作为 `J` 类型**隐式构造**的参数，或者用于 `J` 类型的赋值运算符。

## 版本

v0.8.0 至今。
