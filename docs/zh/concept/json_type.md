# **json_type**

```cpp
template<typename J, typename T>
concept json_type = std::disjunction_v<
    std::is_same<T, typename J::Null>,
    std::is_same<T, typename J::Bool>,
    std::is_same<T, typename J::Number>,
    std::is_same<T, typename J::String>,
    std::is_same<T, typename J::Array>,
    std::is_same<T, typename J::Object>
>;
```

位于 `vct::tools::json` 命名空间中，用于判断某类型 `T` 是否是 `J` 内部的子类型，`J` 必须是 `json::Json` 类模板的实例化类型。

由于 `json::Json` 是一个类模板，模板形参不同时内部子类型也不同，因此概念处理需要判断的类型 `T` ，还需要输入容器类型 `J`。

此模板仅包含六种 JSON 数据类型，不包含 JSON 容器类型 `J` 本身。

## 使用示例

```cpp
using namespace vct::tools;

...

if constexpr (json_type<Json, T>) {
    std::cout << "T is a JSON object." << std::endl;
} else {
    std::cout << "T is not a JSON object." << std::endl;
}
```

## 版本

v0.8.0 至今。


