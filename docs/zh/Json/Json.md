# **Json**

```cpp
template<
    bool UseOrderedMap = true,
    template<typename U> class AllocatorType = std::allocator
>
requires requires{
    typename std::basic_string<char, std::char_traits<char>, AllocatorType<char>>;
    typename std::vector<int, AllocatorType<int>>;
    typename std::map<std::string, int, std::less<std::string>, AllocatorType<std::pair<const std::string, int>>>;
    typename std::unordered_map<std::string, int, std::hash<std::string>, std::equal_to<std::string>, AllocatorType<std::pair<const std::string, int>>>;
}
class Json;
```

位于 `vct::tools::json` 命名空间中，提供了一个通用的 JSON 容器类模板，内部提供各种 `JSON` 相关数据操作，比如增删改查、序列化与反序列化等。

在 `vct::tools` 中提供了默认的 `Json` 类别名，简化使用：

```cpp
namespace vct::tools {
    using Json = ::vct::tools::json::Json<>;
}
```

## 模板形参

- `UseOrderedMap` 参数用于表示在记录键值对时使用有序映射（`std::map`）还是无序映射（`std::unordered_map`）。默认为 `true`，表示使用有序映射。

- `AllocatorType` 参数用于指定容器需要使用的内存分配器，默认为 `std::allocator`。可以自定义分配器来满足特定的内存管理需求。

只提供内存分配器的自定义，以及有序或哈希映射的选择，无法指定字符串、数组和映射的具体实现类型。

## 内部类型

每个 `Json` 类内部存在六种类型，对于 JSON 的六种数据格式，它们的定义如下：

```cpp
// public
using Null = std::nullptr_t;
using Bool = bool;
using Number = double;
using String = std::basic_string<char, std::char_traits<char>, AllocatorType<char>>;
using Array = std::vector<Json, AllocatorType<Json>>;
using Object = std::conditional_t<UseOrderedMap,
    std::map<String, Json, std::less<String>, AllocatorType<std::pair<const String, Json>>>,
    std::unordered_map<String, Json, std::hash<String>, std::equal_to<String>, AllocatorType<std::pair<const String, Json>>>
>;
```

其中 `Null`、`Bool` 和 `Number` 的类型是完全固定的。`String`、`Array` 和 `Object` 的类型可以通过模板参数 `AllocatorType` 自定义内存分配器，`Object` 可以选择使用有序或哈希实现，但类模板依然固定。

在默认情况下，`String` 等于 `std::string`，`Array` 等于 `std::vector<Json>`，`Object` 等于 `std::map<String, Json>`。

## 成员变量

类内仅有一个成员，使用 `std::variant` 类型，存储六种类型中的任意一种值，默认为 `Null` ：

```cpp
// protected
std::variant<
    Null,
    Bool,
    Number,
    String,
    Array,
    Object
> m_data { Null{} };
```

## 成员函数

### 1. 构造相关

- [constructor](constructor.md)：构造函数，支持多种类型的初始化。
- [deconstructor](destructor.md)：析构函数，释放内部资源。
- [operator=](operator_assign.md)：赋值运算符，支持多种类型的赋值。

### 2. 基础操作

- [type](type.md)：获取当前 JSON 数据的类型。
- [is_nul](is_nul.md)：检查当前 JSON 数据是否为 `Null`。
- [is_bol](is_bol.md)：检查当前 JSON 数据是否为 `Bool`。
- [is_num](is_num.md)：检查当前 JSON 数据是否为 `Number`。
- [is_str](is_str.md)：检查当前 JSON 数据是否为 `String`。
- [is_arr](is_arr.md)：检查当前 JSON 数据是否为 `Array`。
- [is_obj](is_obj.md)：检查当前 JSON 数据是否为 `Object`。
- [nul](get_nul.md)：获取内部 `Null` 数据的引用。
- [bol](get_bol.md)：获取内部 `Bool` 数据的引用。
- [num](get_num.md)：获取内部 `Number` 数据的引用。
- [str](get_str.md)：获取内部 `String` 数据的引用。
- [arr](get_arr.md)：获取内部 `Array` 数据的引用。
- [obj](get_obj.md)：获取内部 `Object` 数据的引用。

### 3. 数据操作

- ['[]'](operator_bracket.md)：下标运算符，用于访问或修改数组和对象中的元素。
- [at](at.md)：访问数组或对象中的元素，支持越界检查。
- [to](to.md)：将当前 JSON 内部数据拷贝并转换为指定类型，失败时抛出异常。
- [to_if](to_if.md)：尝试将当前 JSON 内部数据拷贝并转换为指定类型，使用 `optional` 返回值。
- [to_or](to_or.md)：尝试将当前 JSON 内部数据拷贝并转换为指定类型，失败时返回默认值。
- [move](move.md)：将当前 JSON 内部数据移动并转换为指定类型，失败时抛出异常。
- [move_if](move_if.md)：尝试将当前 JSON 内部数据移动并转换为指定类型，使用 `optional` 返回值。
- [move_or](move_or.md)：将当前 JSON 内部数据移动并转换为指定类型，失败时返回默认值。
- [operator==](operator_eq.md)：比较两个 JSON 对象是否相等。

### 4. 序列化与反序列化

- [parse](parse.md)：静态成员函数，将字符串或输入流中的 JSON 文本解析为 `Json` 对象。
- [dump](dump.md)：将当前 JSON 对象序列化为字符串，去除无效字符。
- [dumpf](dumpf.md)：将当前 JSON 对象序列化为字符串，可指定缩进。
- [write](write.md)：将当前 JSON 对象序列化写入字符串或输出流，去除无效字符。
- [writef](writef.md)：将当前 JSON 对象序列化写入字符串或输出流，可指定缩进。

### 5. 容器操作简化函数

- [reset](reset.md)：重置当前 JSON 数据。
- [size](size.md)：获取内部子元素个数。
- [contains](contains.md)：检查当前 JSON 是否包含指定键或索引。
- [empty](empty.md)：检查当前 JSON 是否存在子元素。
- [erase](erase.md)：删除指定键或索引的元素。
- [insert](insert.md)：插入新的键值对或元素到 JSON 中。
- [push_back](push_back.md)：向数组中添加新元素。
- [pop_back](pop_back.md)：从数组中移除最后一个元素。

## 版本

v0.8.0 至今。
