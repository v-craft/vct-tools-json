# **json::Value::writef**

```cpp
Bool writef(
    String& out,
    std::uint16_t space_num = 2,
    std::uint16_t depth = 0,
    std::uint32_t max_space = 512
) const noexcept;

Bool writef(
    std::ostream& out, 
    std::uint16_t space_num = 2,
    std::uint16_t depth = 0,
    std::uint32_t max_space = 512
) const noexcept;
```


Serialize Json value with format to a string back or ostream.

## Parameters

- `space_num` \- the number of spaces to use for indentation (default is 2).
- `depth` \- the current depth of the JSON value (default is 0).
- `max_space` \- the maximum number of spaces to use for indentation (default is 512).

if `depth*space_num > max_space`, it will stop writing and quickly return `false`.

## Version

From v1.0.0 to now.
