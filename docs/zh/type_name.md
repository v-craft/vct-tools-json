# **type_name**

```cpp
constexpr const char* type_name(const Type type) noexcept;
```

位于 `vct::tools::json` 命名空间中，用于将 [`Type`](./Type.md) 枚举值转换为对应的字符串。

建议仅用于调试或日志输出。

## 内部实现

??? note "内部实现"
    ```cpp
    constexpr const char* type_name(const Type type) noexcept{
        switch ( type ) {
            case Type::eObject: return "Object";
            case Type::eArray:  return "Array";
            case Type::eString: return "String";
            case Type::eNumber: return "Number";
            case Type::eBool:   return "Bool";
            case Type::eNull:   return "Null";
            default: return "Unknown Enum Value"; // should never happen
        }
    }
    ```

## 异常

无异常。

## 时间复杂度

常数。

## 版本

v0.8.0 至今。

