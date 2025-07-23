# **json::Value::dumpf**

```cpp
std::optional<String> dumpf(
    const std::uint16_t space_num = 2, 
    const std::uint16_t depth = 0, 
    const std::uint32_t max_space = 512
) const noexcept;
```


Serialize Json value with format and return as a string.

## Parameters

- `space_num` \- the number of spaces to use for indentation (default is 2).
- `depth` \- the current depth of the JSON value (default is 0).
- `max_space` \- the maximum number of spaces to use for indentation (default is 512).

if `depth*space_num > max_space`, it will stop writing and quickly return `false`.

## Version

From v1.0.0 to now.
