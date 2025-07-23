# **json::Value::str**

```cpp
constexpr String& str() & ;
constexpr String&& str() && ;
constexpr const String& str() const & ;
constexpr const String&& str() const && ;
```

Get the reference of inner `String` value of this `Value`.

## Exception

throw `std::bad_variant_access` if this Value is not String.

## Version

From v1.0.0 to now.
