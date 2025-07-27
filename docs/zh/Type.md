# **Type**

```cpp
enum class Type{
    eNull = 0,
    eBool,    
    eNumber,  
    eString,  
    eArray,   
    eObject   
};
```

位于 `vct::tools::json` 命名空间中，用于表示六种 JSON 类型。

由于任何 `Json` 容器类型的内部只可能存放此六种类型的数据，因此使用通用枚举表示。

## 版本

v0.8.0 至今。
