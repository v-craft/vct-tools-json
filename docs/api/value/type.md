# **json::Value::type**

```cpp
constexpr Type type() const noexcept;
```

Returns the type of the inner data.

## Return value

A [`json::Type`](../Type.md) value representing the type of the inner data.

??? note "implementation"
    ```cpp
    [[nodiscard]]
    constexpr Type type() const noexcept { return static_cast<Type>(m_data.index()); }
    ```

## Exception

Noexcept.

## Complexity

Constant.

## Version

From v1.0.0 to now.
