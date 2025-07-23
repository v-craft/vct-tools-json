# **json::Value::operator=**

```cpp
bool operator==(const Value& other) const noexcept;
```

Compares two `Value` objects for equality.

??? note "Implementation"
    ```cpp
    bool operator==(const Value& other) const noexcept {
        if (type() != other.type()) return false; // Different types cannot be equal
        switch (type()) {
            case Type::eNull: return true; // Both are null
            case Type::eBool: return std::get<Bool>(m_data) == std::get<Bool>(other.m_data);
            case Type::eNumber: return std::get<Number>(m_data) == std::get<Number>(other.m_data);
            case Type::eString: return std::get<String>(m_data) == std::get<String>(other.m_data);
            case Type::eObject: return std::get<Object>(m_data) == std::get<Object>(other.m_data);
            case Type::eArray: return std::get<Array>(m_data) == std::get<Array>(other.m_data);
        }
        return false; // Should never reach here, but added for safety
    }
    ```

## Complexity

Linear.

## Version

From v1.0.0 to now.
