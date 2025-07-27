# **Json.writef**

```cpp
Bool writef(
    String& out,
    std::uint16_t space_num = 2,
    std::uint16_t depth = 0,
    std::uint32_t max_space = 512
) const;

Bool writef(
    std::ostream& out, 
    std::uint16_t space_num = 2,
    std::uint16_t depth = 0,
    std::uint32_t max_space = 512
) const;
```


将 Json 数据带缩进地序列化到字符串末尾或输出流。

## 参数

- `space_num` 单次缩减空格数
- `depth` 需要缩进的初始次数
- `max_space` 最大单行缩进次数

## 返回值

返回布尔值，表示序列化是否成功。
在缩减长度超出 `max_space` 限制，或者输出流状态错误时返回 `false`，否则返回 `true`。

注意，如果出现异常（比如内存不足），函数并不会正常返回，而是由异常跳转处理，因此返回值与异常无关。

## 异常

函数本身不会抛出任何异常，但是由于字符串末尾增加内容涉及内存分配，以及输出流本身可能状态错误，可能会抛出异常。

函数本身不处理任何异常，且内存分配通常不会出现异常。
对于输出流，函数会检查 `fail()` 状态，如果流已经出错，则函数会立即返回不再序列化，你需要在调用前后检查流状态。

## 复杂度

线性。

## 版本

v0.8.0 至今。
