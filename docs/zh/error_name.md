# **error_name**


```cpp
constexpr const char* error_name(const ParseError error) noexcept;
```

位于 `vct::tools::json` 命名空间中，用于将 [`ParseError`](./ParseError.md) 枚举值转换为对应的字符串。

建议仅用于调试或日志输出。


## 内部实现

??? note "内部实现"
    ```cpp
    constexpr const char* error_name(const ParseError error) noexcept {
        switch (error) {
            case ParseError::eNone: return "None";
            case ParseError::eEmptyData: return "EmptyData";
            case ParseError::eDepthExceeded: return "DepthExceeded";
            case ParseError::eIllegalEscape: return "IllegalEscape";
            case ParseError::eInvalidNumber: return "InvalidNumber";
            case ParseError::eRedundantText: return "RedundantText";
            case ParseError::eUnclosedString: return "UnclosedString";
            case ParseError::eUnclosedObject: return "UnclosedObject";
            case ParseError::eUnclosedArray: return "UnclosedArray";
            case ParseError::eUnknownFormat: return "UnknownFormat";
            case ParseError::eUnknownError: return "UnknownError";
            default: return "Unknown Enum Value";
        }
    }
    ```

## 异常

无异常。

## 时间复杂度

常数。

## 版本

v0.8.0 至今。



