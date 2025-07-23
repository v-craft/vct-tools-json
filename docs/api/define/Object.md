# **json::Object**

```cpp
using Object = std::map<String,Value>;
```

This type is used to represent the `object` type in JSON, corresponding to the `eObject` in [`json::Type`](../Type.md).

`Value` is [`json::Value`](../value/Value.md) , which can store any type of json data.

## Note

Choosing to use `std::map` is to ensure that the serialization results of the same data are the same.

## Version

From v1.0.0 to now.
