# **json::Value::type_name**


```cpp
constexpr const char* type_name() const noexcept;
```

Returns the type name of the inner data. Should be used for debugging or logging purposes only.

## Return value

A `const char*` representing the type name of the inner data.

```cpp
switch ( this->type() ) {
    case Type::eObject: return "Object";
    case Type::eArray:  return "Array";
    case Type::eString: return "String";
    case Type::eNumber: return "Number";
    case Type::eBool:   return "Bool";
    case Type::eNull:   return "Null";
    default: return "Unknown Enum Value"; // should never happen
}
```

## Exception

Noexcept.

## Complexity

Constant.

## Version

From v1.0.0 to now.


