# **Json.reset**

```cpp
template<typename T = Null>
requires json_type<Json, T>
void reset() noexcept;
```

重置内部数据为指定类型，默认为 `Null` 类型，可以指定模板参数从而重载为不同类型。

## 内部实现

??? note "内部实现"
    ```cpp
    template<typename T = Null>
    requires json_type<Json, T>
    void reset() noexcept {
        if constexpr(std::is_same_v<T, Null>) {
            m_data = Null{};
        } else if constexpr(std::is_same_v<T, Bool>) {
            m_data = Bool{};
        } else if constexpr(std::is_same_v<T, Number>) {
            m_data = Number{};
        } else if constexpr(std::is_same_v<T, String>) {
            m_data = String{};
        } else if constexpr(std::is_same_v<T, Array>) {
            m_data = Array{};
        } else if constexpr(std::is_same_v<T, Object>) {
            m_data = Object{};
        }
    }
    ```
## 异常

无异常。

## 复杂度

常数。

## 版本

v0.8.0 至今。
