# **json::Value::num**

```cpp
constexpr Number& num() & ;
constexpr Number&& num() && ;
constexpr const Number& num() const & ;
constexpr const Number&& num() const && ;
```

Get the reference of inner `Number` value of this `Value`.

## Exception

throw `std::bad_variant_access` if this Value is not Number.

## Version

From v1.0.0 to now.
