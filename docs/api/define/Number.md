# **json::Number**

```cpp
using Number = double;
```

This type is used to represent the `number` type in JSON, corresponding to the `eNumber` in [`json::Type`](../Type.md).

`Number{} == 0.0` must be `true`.

## Note

- The most significant digit of the output is 17 decimal digits.

- Round off when converting to integer type.

## Version

From v1.0.0 to now.

