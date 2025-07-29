# **Json.push_back**

```cpp
template<typename V>
requires std::convertible_to<V, Json>
bool push_back(V&& value);
```

如果内部数据类型是 `Array`，则在尾部插入指定元素并返回 `true`。如果类型不匹配则返回 `false` （不会抛出异常）。

## 异常

通常无异常。

## 复杂度

常数。

## 版本

v1.0.0 至今。

