# **json::Value::at**

```cpp
// 1
Value& at(const String& key);

// 2
const Value& at(const String& key) const;

// 3
Value& at(const std::size_t index);

// 4
const Value& at(const std::size_t index) const;
```

1. return a reference to the value associated with the given key in an object. If the key does not exist, it will throw `out_of_range` exception.

2. return a const reference to the value associated with the given key in an object. If the key does not exist, it will throw `out_of_range` exception.

3. return a reference to the value at the given index in an array. If the index is out of range, it will throw `out_of_range` exception.

4. return a const reference to the value at the given index in an array. If the index is out of range, it will throw `out_of_range` exception.

## Version

From v1.0.0 to now.


