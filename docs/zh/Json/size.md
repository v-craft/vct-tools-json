# **Json.size**

```cpp
std::size_t size() const noexcept;
```

如果内部数据类型是 `Object` 或 `Array`，则返回其元素个数；否则返回 0 （包括 `String`）。

## 异常

无异常。

## 复杂度

常数。

## 版本

v1.0.0 至今。

