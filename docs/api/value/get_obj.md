# **json::Value::obj**

```cpp
constexpr Object& obj() & ;
constexpr Object&& obj() && ;
constexpr const Object& obj() const & ;
constexpr const Object&& obj() const && ;
```

Get the reference of inner `Object` value of this `Value`.

## Exception

throw `std::bad_variant_access` if this Value is not Object.

## Version

From v1.0.0 to now.
