# **Json.contains**

```cpp
bool contains(const String& key) const;
```

如果内部数据类型是 `Object`，则调用内部类型的 `contains()` 函数；否则返回 `false`。

## 异常

通常无异常。

## 复杂度

有序映射：对数复杂度；哈希映射：平均常数复杂度。

## 版本

v1.0.0 至今。

