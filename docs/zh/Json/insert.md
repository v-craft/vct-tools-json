# **Json.insert**

```cpp
// 1
template<typename  K, typename V>
requires std::convertible_to<K, String> && std::convertible_to<V, Json>
bool insert(K&& key, V&& value);

// 2
template<typename V>
requires std::convertible_to<V, Json>
bool insert(const std::size_t index, V&& value);
```

1. 如果内部数据类型是 `Object`，则插入指定键值对（已存在则覆盖）并返回 `true`。如果类型不匹配则返回 `false` （不会抛出异常）。

2. 如果内部数据类型是 `Array`，且索引在范围内（允许使用结束索引）则插入指定元素并返回 `true`。如果类型不匹配或索引越界则返回 `false` （不会抛出异常）。

## 异常

通常无异常。

## 复杂度

1. 有序映射：对数复杂度；哈希映射：平均常数复杂度。

2. 线性复杂度，取决于尾部元素数量。

## 版本

v1.0.0 至今。

