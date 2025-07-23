# **json::Value::arr**

```cpp
constexpr Array& arr() & ;
constexpr Array&& arr() && ;
constexpr const Array& arr() const & ;
constexpr const Array&& arr() const && ;
```

Get the reference of inner `Array` value of this `Value`.

## Exception

throw `std::bad_variant_access` if this Value is not Array.

## Version

From v1.0.0 to now.
