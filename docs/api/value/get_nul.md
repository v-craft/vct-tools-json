# **json::Value::nul**

```cpp
constexpr Null& nul() & ;
constexpr Null&& nul() && ;
constexpr const Null& nul() const & ;
constexpr const Null&& nul() const && ;
```

Get the reference of inner `Null` value of this `Value`.

## Exception

throw `std::bad_variant_access` if this Value is not Null.

## Version

From v1.0.0 to now.
