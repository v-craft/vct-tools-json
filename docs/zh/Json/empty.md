# **Json.empty**

```cpp
bool empty() const noexcept;
```

如果内部数据类型是 `Object` 或 `Array`，则调用内部类型的 `empty()` 函数；否则返回 `true` （包括 `String`）。

## 异常

无异常。

## 复杂度

常数。

## 版本

v1.0.0 至今。



