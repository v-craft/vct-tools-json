# **json::Value::constructor**

```cpp
template<json_type T = Null>
void reset() noexcept;
```

Reset the JSON value to a specific type, defaulting to `Null`.

??? note "implementation"
    ```cpp
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
    ```

## Complexity

Constant.

## Version

From v1.0.0 to now.


