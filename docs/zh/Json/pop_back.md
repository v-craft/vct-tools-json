# **Json.pop_back**

```cpp
bool pop_back();
```

如果内部数据类型是 `Array`，且存在子元素，则删除尾部元素并返回 `true`。如果类型不匹配或数组为空则返回 `false` （不会抛出异常）。

## 异常

通常无异常。

## 复杂度

常数。

## 版本

v1.0.0 至今。

