# **ParseError**

```cpp
enum class ParseError {
    eNone = 0,      
    eEmptyData,     
    eRedundantText, 
    eDepthExceeded, 
    eIllegalEscape, 
    eInvalidNumber, 
    eUnclosedString,
    eUnclosedObject,
    eUnclosedArray, 
    eUnknownFormat, 
    eUnknownError   
};
```

位于 `vct::tools::json` 命名空间中，用于 `parse` 函数反序列化字符串失败的原因。

虽然 `Json` 容器的使用的类型可能不同，但是反序列化算法本质相同，可能存在的错误原因也一致，因此使用通用枚举表示。

## 注意

此枚举值并不准确，并不能指出错误的具体位置，且很多错误会被归类为 `eUnknownFormat` ，建议仅用于粗略调试。

## 版本

v0.8.0 至今。
