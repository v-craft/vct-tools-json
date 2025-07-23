# **json::Value::to_or**

```cpp
template<typename T, typename D = Null>
requires convertible<T> || convertible_map<T, D> || convertible_array<T, D>
[[nodiscard]]
T  to_or( T default_result, D default_range_elem = D{} ) const noexcept;
```

type conversion, copy inner value to specified type.

Attempt sequence of conversions:

1. Null -> Null
2. Object -> Object
3. Array -> Array
4. String -> String
5. Bool -> Bool
6. Number -> enum types (us llround, round to nearest)
7. Number -> integral types (us llround, round to nearest)
8. Number -> floating_point types
9. Any -> T is constructible from json::Value
10. Object -> implicit convertible types
11. Array -> implicit convertible types
12. String -> implicit convertible types
13. Number -> implicit convertible types
14. Bool -> implicit convertible types
15. Null -> implicit convertible types (Null is not convertible to bool !!!!!)
16. Object -> Try copy to `range && String->key_type && Value->mapped_type types && have default_range_value`
17. Array -> Try copy to `range && Value->value_type types && have default_range_value`
18: return default_result

## Version

From v1.0.0 to now.

