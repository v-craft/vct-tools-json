# **Json.operator[]**

```cpp
// 1
Json& operator[](const String& key) { return std::get<Object>(m_data)[key]; }

// 2
const Json& operator[](const String& key) const { return std::get<Object>(m_data).at(key); }

// 3
Json& operator[](const std::size_t index) { return std::get<Array>(m_data)[index]; }

// 4
const Json& operator[](const std::size_t index) const { return std::get<Array>(m_data).at(index); }
```

1. 访问对象指定键值对的值，返回可变引用，越界时自动创建一个默认值对象，不抛出异常。
2. 访问对象指定键值对的值，返回不可变引用，越界时抛出异常。
3. 访问数组指定索引的值，返回不变引用，越界时程序崩溃。
4. 访问数组指定索引的值，返回的不可变引用，越界时抛出 `out_of_range` 异常。

## 异常

尝试获取对象引用但内部数据不是 `Object` 时抛出 `std::bad_variant_access` 异常，获取数组应用时同理。

`const` 访问越界时抛出 `std::out_of_range` 异常。

## 复杂度

数组访问为常数，对象访问默认为对数\(logN\)，使用哈希映射时平均复杂度为常数。

## 版本

v0.8.0 至今。
