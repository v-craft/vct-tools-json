# **Json.dumpf**

```cpp
std::optional<String> dumpf(
    const std::uint16_t space_num = 2, 
    const std::uint16_t depth = 0, 
    const std::uint32_t max_space = 512
) const;
```

将 JSON 数据序列化为字符串并返回。

## 参数

- `space_num` 单次缩减空格数
- `depth` 需要缩进的初始次数
- `max_space` 最大单行缩进次数

## 返回值

如果单行缩进长度超出限制，则返回 `std::nullopt`，否则返回一个包含序列化结果的 `String` 对象。

注意，如果出现异常（比如内存不足），函数并不会正常返回，而是由异常跳转处理，因此返回值与异常无关。

## 异常

函数本身不会抛出任何异常，但需要将数据写入字符串对象，中间涉及的内存分配可能会抛出异常。

## 复杂度

线性。

等价于创建一个空 `String`，然后使用 `Json.writef` 函数将数据写入该字符串并返回。

## 版本

v0.8.0 至今。