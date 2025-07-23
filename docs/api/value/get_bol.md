# **json::Value::bol**

```cpp
constexpr Bool& bol() & ;
constexpr Bool&& bol() && ;
constexpr const Bool& bol() const & ;
constexpr const Bool&& bol() const && ;
```

Get the reference of inner `Bool` value of this `Value`.

## Exception

throw `std::bad_variant_access` if this Value is not Bool.

## Version

From v1.0.0 to now.
