<div align="center">

<h1>vct-tools-json</h1>

<p>
   <a href="#ENGLISH"><img src="https://img.shields.io/badge/English-blue?style=for-the-badge" alt="English" /></a>
   &nbsp;&nbsp;
   <a href="#中文"><img src="https://img.shields.io/badge/中文-red?style=for-the-badge" alt="中文" /></a>
</p>
</div>

---

<div id="中文">

## **介绍**

vct-tools-json 是一个 C++23 的 JSON 库，它提供简洁、高效的 JSON 解析、操作和序列化功能。

你可以在本仓库的 [github-pages](https://v-craft.github.io/vct-tools-json/) 页面找到详细的文档。

## **导入库**

本库使用 CMake 构建，需要 C++23 标准库模块支持。你可以直接下载源代码并将其添加到你的项目中，或者使用 CMake 的 `FetchContent` 模块来自动下载。

```cmake
include(FetchContent)   # 引入 FetchContent 模块
FetchContent_Declare(
    vct-tools-json      # 声明 vct-tools-json 库，SHA 验证请自行添加，tags 版本号请自行选择
    URL  https://github.com/v-craft/vct-tools-json/archive/refs/tags/xxxx.tar.gz
)
FetchContent_MakeAvailable(vct-tools-json) # 下载并添加到项目中
```

如果你选用源码的方式，别忘了通过 `add_subdirectory` 将其显式添加到你的 CMake 项目中。`FetchContent` 会自动处理依赖关系。

> 目前暂不支持 vcpkg ，因为标准库模块仍属于实验性支持，实验性 UUID 与 CMake 版本相关，无法通用。

然后可以使用简单的链接库，这会自动处理所有依赖：

```cmake
target_link_libraries(main PRIVATE vct::tools-json)
```

最后，在你的文件中导入 JSON 模块：

```cpp
import std; // 建议启用 C++23 的 std 模块支持
import vct.tools.json; // 导入 vct-tools-json 库
using namespace vct::tools; // 使用命名空间，简化代码书写
```

## **基础类型**

JSON 标准中定义了六种类型，本库中将它们映射为了 C++ 中类型，都位于 `vht::tools::json` 命名空间中：

| JSON 类型  | 本库中的类型   | 对应的 C++ 类型                     | 对应的枚举值          |
|----------|----------|--------------------------------|-----------------|
| `null`   | `Null`   | `std::nullptr_t`               | `Type::eNull`   |
| `bool`   | `Bool`   | `bool`                         | `Type::eBool`   |
| `number` | `Double` | `double`                       | `Type::eNumber` |
| `string` | `String` | `std::string`                  | `Type::eString` |
| `array`  | `Array`  | `std::vector<Value>`           | `Type::eArray`  |
| `object` | `Object` | `std::map<std::string, Value>` | `Type::eObject` |

> 选用 `map` 而非 `unordered_map` 是为了保证序列化结果的一致性和时间的稳定性。

本库还提供了一个概念\(concept\) `json::json_type` ，仅有上述六种类型满足此概念。

注意到这里有一个特殊类型 `Value` ，它也由本库定义，内部只有一个 `std::variant` ，可以存储上述任一类型的值并附带类型索引。

## **基础操作**

### 1. 初始化

`Value` 的默认构造函数会创建 `Null` 类型的值，但你可以直接通过上述六种类型进行初始化：

```cpp
json::Value null_val;                  // 默认构造，类型为 Null
json::Value bool_val(3.3);             // 浮点初始化，类型为 Number
json::Value obj_val = json::Object{};  // 直接使用 Object 初始化
```

`json::Value` 在上述六种类型外，还支持基本算术类型、枚举类型和 `const char*` 的隐式构造函数和赋值函数。

**枚举会被视为整数**，不要试图使用 `json::Type` 枚举值进行指定初始化，这只会生成 `Number` 类型的值：

```cpp
json::Value enum_val{ json::Type::eObject }; // 危险
// 这会生成一个 Number 类型的值，具体值取决于枚举值的整数表示。
```

虽然 `Value` 不支持初始化列表，但由于隐式构造的存在，可以通过 `Array` 和 `Object` 的初始化列表快速创建对象：

```cpp
json::Value smp_val = json::Object{
    { "key1", 42 },
    {"key2", "value2"},
    {"key3", true },
    {"arr", json::Array{ { 2, 3.14, nullptr } } },
    {"obj", json::Object{ {"nested_k", "nested_v"} } }
};
```

**注意：**空数组初始化请使用 `Array{}` ，非空初始化请使用 `Array{ { ... } }` 双重花括号，否则在特定情况下会被认为是拷贝构造/扩容构造而非初始化列表。

你可以用 `type()` 或 `is_xxx()` 函数检查类型，或者使用 `type_name()` 获取字符串形式的类型名：

```cpp
smp_val.type();        // 返回 Type::eObject
smp_val.type_name();  // 返回 "Object" ,没有前缀 `e`
smp_val.is_arr();     // 返回 false
```
`is` 共有六个，分别是 `arr`、`obj`、`str`、`num`、`bol` 和 `nul` ，对应六种 JSON 类型。

你可以通过赋值语句重置内容，也可以使用 `reset()` 成员函数。
这是个模板函数，默认重置回 `Null` 类型，但你可以显示指定重置类型，比如使用 `reset<json::Object>()` 将内容重置为一个空的 `Object` 。

`reset` 的模板形参只能是六种 JSON 类型之一，否则无法通过编译。

### 2. 访问和修改

本库提供了 `xxx()` 成员函数以获取内部数据的**引用**，`xxx` 和上面的 `is_xxx` 相同。

```cpp
 // 注意 Object 的 mapped 依然是 Value 类型
json::Value& vi_42 = smp_val.obj()["key1"];

// 虽然返回引用，但也可以用于赋值
double i_42 = vi_42.num(); 

 // vi_42.str(); // 类型不匹配，抛出 std::bad_varient_access 异常
```

`Value` 还提供了 `[]` 和 `at` 运算符，区别在于 `at` 禁止索引越界（抛出异常），而 `[]` 不检查越界（所以`Object`可以用`[]`创建新键值对，但`Array`越界是未定义行为，可能直接断言崩溃）。

> `const` 的 `[]` 较为特殊，等价于 `const` 的 `at` ，无法创建新键值对或越界。

```cpp
smp_val["arr"][1].num(); // 返回 3.14
smp_val.at("obj").at("nested_k") = nullptr; // 修改对象，变为 Null 类型
smp_val["obj"].at("nested_k").is_nul(); // 返回true, [] 和 at 可以随意混合使用
```

我们还提供了一些辅助函数用于简化数组和对象的操作，避免过多的 `get<>` 调用。
这些函数仅对数组和对象有效，但它们进行了类型与范围检查，不会抛出异常，可以“随意”使用。

| 函数名                     | 作用          | 返回值      | 描述                                 |
|-------------------------|-------------|----------|------------------------------------|
| `size()`                | 获取元素数量      | `size_t` | 数组或对象时返回子元素数量，其他类型（含字符串）返回 0       |
| `empty()`               | 检查是否为空      | `bool`   | 数组或对象时返回是否为空，其他类型（含字符串）返回 **true** |
| `push_back( val )`      | 向数组末尾添加元素   | `bool`   | 仅是数组且添加成功时返回 true                  |
| `pop_back()`            | 移除数组末尾元素    | `bool`   | 仅是数组且移除成功时返回 true （空数组也 false )    |
| `insert(idx, val)`      | 在指定位置插入元素   | `bool`   | 仅数组指定位置插入成功时范围 true（越界也 false）     |
| `insert(idx, cnt, val)` | 在指定位置插入多次元素 | `bool`   | 仅数组指定位置插入成功时范围 true（越界也 false）     |
| `erase(idx)`            | 移除指定位置元素    | `bool`   | 仅数组指定位置移除成功时返回 true（越界也 false）     |
| `contains( key )`       | 检查对象是否包含指定键 | `bool`   | 仅是对象且包含指定键时返回 true                 |
| `insert( key, val )`    | 向对象添加键值对    | `bool`   | 仅是对象且添加成功时返回 true （已存在也 false）     |
| `erase( key )`          | 移除对象指定键值队   | `bool`   | 仅是对象且移除成功时返回 true （不存在也 false）     |

上述函数还支持 `Value` 参数的移动传值。

`get` 只能获取六种类型的引用，所以本库还提供了 `to` 和 `move` 模板来获取内部的值，并强制转换类型。前者必然是拷贝，后者是移动或拷贝（简单类型，或者无需移动时进行拷贝）。

```cpp
auto str_view = smp_val["key2"].to<std::string_view>(); // 返回内部字符串的视图
auto str = smp_val["key2"].move<std::string>(); // 将内部字符串移动了出来，现在内部变为空字符串，之前的视图不再可用
int int_42 = smp_val["key1"].to<int>(); // 返回内部整数的拷贝
```

特别注意，`Number` 数据使用 `double` 存储，因此在转换成整数时（枚举类型，或符合整数模板要求的类型），会**四舍五入**，避免精度问题。

注意，`to` 和 `move` 支持很多类型，转换失败会抛出 `std::runtime_error` 异常。
为此，我们还提供了 `xx_if` 和 `xx_or` 版本，前者返回 `optional<T>` ，后者则是在失败时返回指定的默认值。

```cpp
auto opt_str = smp_val["key1"].to_if<std::string>(); // opt_str 是 std::optional<std::string> ，转换失败为空，但不抛出异常
if(opt_str) std::cout << *opt_str << std::endl; // 如果转换成功，输出字符串
std::string or_str = smp_val["key1"].to_or<std::string>("default"); // 如果转换失败，返回 "default"
```

转换具有非常准确的规则与测试顺序，详细内容请参考 [github-pages](https://v-craft.github.io/vct-tools-json/) 文档，或源码注释。

本库提供了三个概念用于查询类型是否可能被转换：（都不满足的无法使用`to/move`模板，无法通过编译）

1. `json::convertible<T>` 可能**直接**转换成功的类型 `T`，这包含了满足 `json::json_type<T>` 的类型。
2. `json::convertible_map<T,D>` 可能**间接**转换成功的映射类型 `T`，键必须是字符串，值\(mapped\)是 `D` 类型且满足条件 1 。
3. `json::convertible_array<T,D>` 可能**间接**转换成功的数组类型 `T`，内部值是 `D` 类型且满足条件 1 。

> 实际上，能直接转换成功的数组或映射同时满足条件 1 & ( 2 | 3 ) ，但这不重要。

只要类型满足三种概念之一，就可以使用 `to` 和 `move` 系列函数进行转换。我们会在后续的“自定义类型序列化”部分详细介绍。

### 3. 序列化与反序列化

本库的序列化和反序列化非常高效且容易使用。

首先是反序列化，将字符串转换为 `Value` 对象，使用 `json::read()` 函数。

```cpp
std::string json_str1 = R"( [ 1, false, null, { "Hello": "World" } ] )";
std::string json_str2 = R"( false )"; // 允许顶层类型是任一 JSON 类型
json::Value val1 = json::read(json_str1).value_or( nullptr ); // 解析 JSON 字符串
std::cout << val1[1].to<bool>() << std::endl; // 输出 0 （没有指定 boolaplha）
```

这里需要要说明三件事：

1. JSON 文件解析失败在实际应用中很常见且难以预料，因为很容易有一些格式错误或垃圾数据。
因此本库的反序列化函数返回 `std::expected<Value, ParseError>` ，从而避免使用异常机制，减小开销。后者是一个描述错误类型的枚举。

2. 此函数还具有一个可选参数 `max_depth`（默认是 256），用于限制解析的最大（嵌套）深度。
本库虽然保证总解析复杂度是 `O(n)` 的（严格单次遍历），但使用了递归来处理嵌套结构，因此需要用它避免某些垃圾数据的问题（比如过长的 `[[[[[[[[]]]]]]]]` 在递归时可能导致栈溢出）。

3. 此函数除了 `std::string` 外，还能传入 `std::istream` 进行流式解析。
流式解析文件的效率几乎等同于先将文件全部读入 string 再用 string 解析，但内存占用可能更少。

然后是序列化，使用 `Value` 对象的 `dump/write` 成员函数。

```cpp
std::string str_ser = val1.dump(); // 不含无效空白字符的高效序列化，返回 std::string
std::string str_back;
val1.write( str_back ); // 将序列化结果写入 std::string 的末尾
val1.write( std::cout ); // 将序列化结果直接输出到 `ostream` 中
```

现在 `str_ser` 和 `str_back` 的内容完全一样，因为 `dump` 就是用 `write` 实现的。

由于 `Value` 必然是有效的 JSON 数据，因此 `dump` 必然成功。
不过 `write` 的流操作不一定成功（比如文件突然关闭）、函数检测到流的状态为 `fail()` 后会立即返回，但不会抛出异常，需要你自行检查流的状态。

上面的三个序列化函数都是高效的紧凑序列化，不含无效空白字符。
但你可以使用 `dumpf/writef` 系列函数来获得更易读的格式化输出，`f` 指 `format` 它同样包含三种形式：

```cpp
std::string pretty_str = val1.dumpf().value_or( "fail" );
bool res = val1.writef( std::cout ); // 输出到 `ostream`，返回是否成功
if (!res){ std::cerr << "fail"; }   // 还有写入字符串，此处省略
```

`f` 系列有三个可选参数，依次是 “单次缩进空格数（默认 2）”，“初始缩进次数（默认 0）”和“最大**单行**缩进空格数（默认 512）”。

前两个参数控制缩进的样式，很好理解。第三个参数依然是为了避免 `[[[[]]]]` 这种垃圾数据，此类数据自身文本不长，但是带缩进后会变得非常大，因此使用此参数加以限制。
这就是为什么美化系列的返回值有所不同， `dumpf()` 返回 `std::optional<std::string>` ，另外二者返回 `bool` ，表示是否成功。

美化序列化失败通常是因为缩进长度溢出，但流处理返回 `false` 也可能是因为流本身 `fail` 了。

### 4. 等于运算符

`Value` 类型提供了和 `Value` 进行比较的 `==` 运算符，它首先判断类型是否相同，然后调用内部的 `==` 进行比较（ `std::map` 和 `std::vector` 的比较基于子元素内容，从而实现递归比较）。

```cpp
json::Value val_arr_1 = json::Array{{ 1, 2, 3 }};
json::Value val_arr_2 = json::Array{{ 1, 2, 3 }};
json::Value val_arr_3 = json::Array{{ 1, true, 3 }};
val_arr_1 == val_arr_2; // true
val_arr_1 == val_arr_3; // false
```

更加特殊的是， `Value` 还通过模板函数实现了和其他任意类型的 `==` 比较。

不兼容的类型直接返回 `false` ，如果目标是六种 JSON 类型之一，则先测试类型是否匹配，然后比较具体值。
否则，尝试将对象转换为 `Value` ，或者将 `Value` 转换为目标类型然后比较。都不匹配则返回 `false` 。

`==` 操作必然成功，不会抛出异常。

## **自定义类型序列化**

任何自定义类型，只要提供针对 `Value` 的构造函数和类型转换函数，就能通过 `to/move` 或者类型转换等方式与 JSON 数据交互，从而实现快速的序列化和反序列化。

> 提供了针对 `json::Value` 构造函数和类型转换函数，就满足了 `json::convertible<T>` 概念。

这些函数还有一些细节要求，它们的实现并不轻松，因此本库提供了一个仅包含宏定义的头文件，让你可以轻松实现自定义类型与 JSON 的交互，它甚至支持移动语义。

> 你可以自行浏览此头文件，它的内容很少，但可以让你了解什么类型能够满足转换条件。

```cpp
#define M_VCT_TOOLS_JSON_SIMPLIFY_MACROS // 定义宏，以启用简化的宏函数名
#include <vct/tools/json_macros.hpp>
// 建议将所有头文件放在所有 import 之前，虽然此文件仅含宏定义
import std;
import vct.tools.json;
using namespace vct::tools;
```

假设你现在有这样一个类型：

```cpp
struct MyData{
    int id{};
    std::string m_name{};
    bool active{};
    double m_value{};
};
```

然后你可以像下面这样，通过宏定义为其添加构造函数和转换函数，但需要显式启用默认构造：

```cpp
struct MyData{
    int id{};
    std::string m_name{};
    bool active{false};
    double m_value{};

    MyData() = default; // 必须存在默认构造，内容可以自定义

    M_JSON_CV_FUN( MyData,  // 转换函数，必须在 public 作用域
        M_JSON_CV_MEM( id );    // 注意，MyData 后面必须有 `,` 
        M_JSON_CV_MAP( name, m_name )   // 但是剩余的字段后面不能有逗号 `,` ，分号 `;` 则是可选的 
        M_JSON_CV_MEM( active )
        M_JSON_CV_MAP( value, m_value )
    )
    M_JSON_CS_FUN( MyData,  // 构造函数，必须在 public 作用域
        M_JSON_CS_MEM( id )
        M_JSON_CS_MAP( name, m_name )
        M_JSON_CS_MEM_OR( active, true, nullptr ) // 默认值是 `true`
        M_JSON_CS_MAP_OR( value, m_value, 64.0, nullptr ) // nullptr 表示此类型不需要子元素默认值
    )
};
```

`CV` 的是指 `Conversion` 转换函数，而 `CS` 是指 `Constructor` 构造函数。它们的第一个参数都是类型名，后面需要一个 `,` 分隔符。
然后通过对应的宏定义指定 JSON 转换中需要的字段，`MEM` 是指成员变量名与 JSON 键名相同，`MAP` 是指成员变量名与 JSON 键名不同（比如键是 `name` ，而成员变量名是 `m_name`）。

> 你可以选择自行定义一些简化宏，比如 `JCSM` `JCSP` 等等，高度简化书写。

转换函数是必然成功的，因为需要的数据都是成员变量。
但是构造函数中的成员赋值可能会失败，因为 `json::Value` 中可能不存在对应的键（甚至 `Value` 根本不是 `Object` 类型），因此需要指定成员默认值。

你会看到构造函数（`CS`）的宏，部分带有 `OR` 后者，它们多了两个参数，第一个参数就是默认值。
而没有 `OR` 的宏并非没有默认值，而是将对应类型的默认构造作为默认值，即 `decltype(name){}` 。

作者建议是，字段的默认值请和成员变量的默认值保持一致，因为我们希望从 `json::Value` 转换失败的结果等于默认构造函数的效果。
（上面 `active` 的默认值就和 `CS` 中指定的不一样，不推荐这种写法）

然后你就可以像下面这样，让 `json::Value` 和 `MyData` 互相转换了：

```cpp
json::Value v_null; 
MyData d_null{ v_null }; // 什么都没有，因此全部字段都是 CS 中的默认值
d_null.active; // true，因为 CS 函数指定了默认值为 true

json::Value v_object{ json::Object{} };
v_object["id"] = 42;
v_object["name"] = "Test User";
v_object["active"] = false;
v_object["value"] = 128.0;
MyData d_object{ v_object };    // 必须显式转换，不能用 `=` 构造
d_object.m_name == "Test User"; // true

json::Value v_data{ d_object }; // 将 MyData 转换为 JSON 对象
v_data["id"] == 42; // true
```

使用这两个宏有一个非常重要的要求，即需要转换的**成员变量**必须支持与 `json::Value` 类型的转换。

1. 对于基本算术类型、枚举类型、六种 JSON 类型和 `json::Value` 自身，必然满足要求。
2. 对于其他自定义类类型，需要像上面一样提供转换函数和构造函数。
3. 对于满足条件 1 或 2 的类型构成的列表（如 `std::vector`,`std::list` 等），可以直接使用。
4. 对于满足条件 1 或 2 的类型构成的映射（如 `std::map`,`unordered_map` 等），在键为 `std::string` 时也可以直接使用。

条件 1 和 2 指的是概念 `json::convertible<T>` ，而条件 3 和 4 指的是概念 `json::convertible_array<T,D>` 和 `json::convertible_map<T,D>` 。

比如，现在的 `MyData` 类型已经通过宏定义提供了转换函数和构造函数，满足条件 2 。
因此你可以在其他类型中直接使用它，然后实现嵌套的 JSON 对象：

```cpp
struct MyData2 {
    std::string name;   // std::string 等于 json::String，因此可以直接使用
    MyData my_data;     // MyData 已经有转换函数和构造函数，因此可以直接使用
    std::vector<MyData> data_list;  // 能够直接使用的类型构成的列表也能直接使用（但再套一层列表就不行了）
    MyData2() = default;
    M_JSON_CV_FUN( MyData2,
        M_JSON_CV_MEM( name )
        M_JSON_CV_MAP( data, my_data )
        M_JSON_CV_MEM( data_list )
    )
    M_JSON_CS_FUN( MyData2,
        M_JSON_CS_MEM( name )
        M_JSON_CS_MAP( data, my_data )
        M_JSON_CS_MEM_OR( data_list, std::vector<MyData>{}, MyData{} ) // 变量名，默认值，内部子元素的默认值
    )
};
```

可以看到我们用到了 `OR` 宏的第四个参数。第三个参数是字段本身的默认值，第四个参数是子元素的默认值。
第四个参数仅在目标是数组或者映射类型（且非 `json::Array/Object` ）时才有用，其他时候可以随意填写，通常用 `nullptr` 。

比如你需要数组，但是 `json::Value` 内部不是数组，就会返回第三个字段的默认值。
`json::Value` 也是数组，但是内部只有部分元素能够转成你需要的类型，那么其他元素会用第四个参数的默认值填充，保证数组长度一致。

然后你可以像下面这样在两种类型之间来回切换：

```cpp
json::Value v_data2{ MyData2{} };
std::println("");
v_data2.writef( std::cout );
std::println("");
v_data2["data"]["id"] = 8848;
v_data2["data"]["name"] = "Mount Everest";
v_data2["data"]["active"] = true;
v_data2["data_list"].push_back( v_data2["data"] );
v_data2["name"] = "name_name";
MyData2 d_data2{ v_data2 };
M_EXPECT_TRUE( d_data2.my_data.id == 8848 ); // true
M_EXPECT_TRUE( d_data2.my_data.m_name == "Mount Everest" ); // true
M_EXPECT_TRUE( d_data2.data_list.size() == 1 ); // true
M_EXPECT_TRUE( d_data2.data_list[0].id == 8848 ); // true
M_EXPECT_TRUE( d_data2.data_list[0].m_name == "Mount Everest" ); // true
M_EXPECT_TRUE( d_data2.name == "name_name" ); // true
```

> 这里的 `M_EXPECT_TRUE` 使用的是 vct-test-unit 库，你可以不用在意。

## **列表与映射扩展**

内容变得越来越复杂了，这里作为最后一部分，将介绍列表和映射的实现细节。

我们之前提到，原先只有六种 JSON 类型和基本算术类型是能够直接和 `json::Value` 转换的，而自定义类型需提供转换函数和构造函数。
也就是只有满足 `json::convertible<T>` 概念的类型才能直接转换。

但是，像 `array<int>` 这种标准库提供的类型怎么办呢？它很常用，内部的 `int` 满足了转换条件，但整体并不满足，又无法让它提供转换函数和构造函数。

因此，本库为 `json::Value` 提供了四个模板函数，分别对应 `数组类型->json::Value` 和 `json::Value->数组类型` 以及 `映射类型->json::Value` 和 `json::Value->映射类型` 的转换。

什么类型能够用这些模板呢？首先映射类型的键必须是 `std::string` 或者可以转换为 `std::string` 的类型。
最重要的是内部的值类型，要求是 `json::convertible<T>` ，因为这些类型能够直接转换。

这就是为什么会有两个独立的概念 `json::convertible_map<T,D>` 和 `json::convertible_array<T,D>` 。

`数组/映射->json::Value`是不会遗漏任何元素的，因为所有元素都能被 `json::Value` 接受。
但是反之则不然，`json::Value->数组/映射` 可能会丢失一些元素，因为 `json::Value` 可能有各种奇怪的数据和格式。

因此，如果你的数组和映射不是基本类型里的 `json::Array` 和 `json::Object` ，那么在转换时必须提供两个默认值：

1. 完全不匹配时返回的默认结果。比如需要转换成数组，但是 `json::Value` 内部不是数组，则直接返回此默认值。

2. 能够匹配类型，但是局部元素不匹配时填充的子元素默认值。比如需要 `vector<int>` ，但是 `json::Value` 中是 `[1, 2, [], 3]` ，你需要指定遇到 `[]` 这些不匹配元素时填充的默认整数。

这也就是为什么 `M_JSON_CS_MEM_OR` 和 `M_JSON_CS_MAP_OR` 宏定义需要两个默认值。
不过，如果你转换的类型不是数组或者映射，最后这个子元素默认值可以任意填写，上面我们就使用过 `nullptr` 作为默认值。

此内容在代码中实际对应 `to/move` 系列函数。

对于基本类型或者自定义类型的数据，可以像之前一样直接转换：

```cpp
xxx = val.to<MyData>(); // 或者 move<MyData>()
```

但如果需要转换成数组，就需要显式指定子元素默认值：

```cpp
// 实际模板有两个参数，第一个是目标类型，第二个是填充子元素的类型
xxx = val.to<std::vector<MyData>, MyData>( MyData{} );
// 可以根据函数参数自动推导第二个模板参数
xxx = val.to<std::vector<MyData>>( MyData{} ); 
```

第二个模板参数默认是 `json::Null` ，即 `std:::nullptr_t` 。如果转换目标不是数组或对象，完全不需要添加它。

注意， `to/move` 在完全不匹配时直接抛出异常，所以我们只指定了子元素默认值。
而宏定义实际由 `to_or` 和 `move_or` 实现，因此需要两个默认值：

```cpp
// 第二个模板参数使用自动推导
xxx = val.to_or<std::vector<MyData>>( std::vector<MyData>{} , MyData{} ); 
```

## **最后**

以上就是本库的基本使用，虽然自定义类型序列化的部分比较复杂，但你可以自行阅读文档和源码，本库的源码的有效行数其实非常少（不足 2000 行）。
重点观察 `to` 和 `move` 的实现，以及头文件中的宏定义，你应该能很快上手。

如果你发现的本库的任何问题，或者可优化的地方，欢迎提交 issue 或 PR。

</div>

---

<div id="ENGLISH">

## **Introduction**

vct-tools-json is a C++23 JSON library that provides concise and efficient JSON parsing, manipulation, and serialization capabilities.

You can find detailed documentation on the [github-pages](https://v-craft.github.io/vct-tools-json/) page of this repository.

## **Importing the Library**

This library is built with CMake and requires C++23 standard library module support. You can directly download the source code and add it to your project, or use CMake's `FetchContent` module to automatically download it.

```cmake
include(FetchContent)   # Include FetchContent module
FetchContent_Declare(
    vct-tools-json      # Declare vct-tools-json library, please add SHA verification yourself and choose tags version number
    URL  https://github.com/v-craft/vct-tools-json/archive/refs/tags/xxxx.tar.gz
)
FetchContent_MakeAvailable(vct-tools-json) # Download and add to project
```

If you choose the source code approach, don't forget to explicitly add it to your CMake project via `add_subdirectory`. `FetchContent` will automatically handle dependencies.

> Currently vcpkg is not supported because standard library modules are still experimental, and experimental UUIDs are CMake version dependent and cannot be universal.

Then you can use simple library linking, which will automatically handle all dependencies:

```cmake
target_link_libraries(main PRIVATE vct::tools-json)
```

Finally, import the JSON module in your files:

```cpp
import std; // Recommend enabling C++23 std module support
import vct.tools.json; // Import vct-tools-json library
using namespace vct::tools; // Use namespace to simplify code writing
```

## **Basic Types**

The JSON standard defines six types, which this library maps to C++ types, all located in the `vht::tools::json` namespace:

| JSON Type | Type in This Library | Corresponding C++ Type             | Corresponding Enum Value |
|-----------|----------------------|------------------------------------|--------------------------|
| `null`    | `Null`               | `std::nullptr_t`                   | `Type::eNull`            |
| `bool`    | `Bool`               | `bool`                             | `Type::eBool`            |
| `number`  | `Double`             | `double`                           | `Type::eNumber`          |
| `string`  | `String`             | `std::string`                      | `Type::eString`          |
| `array`   | `Array`              | `std::vector<Value>`               | `Type::eArray`           |
| `object`  | `Object`             | `std::map<std::string, Value>`     | `Type::eObject`          |

> We choose `map` over `unordered_map` to ensure consistency and temporal stability of serialization results.

This library also provides a concept `json::json_type`, which only the above six types satisfy.

Note that there is a special type `Value` defined by this library, which internally contains only a `std::variant` that can store values of any of the above types along with type indices.

## **Basic Operations**

### 1. Initialization

The default constructor of `Value` creates a `Null` type value, but you can directly initialize with any of the above six types:

```cpp
json::Value null_val;                  // Default construction, type is Null
json::Value bool_val(3.3);             // Float initialization, type is Number
json::Value obj_val = json::Object{};  // Direct initialization with Object
```

`json::Value` supports implicit constructors and assignment functions for basic arithmetic types, enum types, and `const char*` in addition to the above six types.

**Enums are treated as integers**. Don't try to use `json::Type` enum values for designated initialization, as this will only generate `Number` type values:

```cpp
json::Value enum_val{ json::Type::eObject }; // Dangerous
// This will generate a Number type value, with the specific value depending on the integer representation of the enum value.
```

Although `Value` doesn't support initializer lists, due to the existence of implicit construction, you can quickly create objects through initializer lists of `Array` and `Object`:

```cpp
json::Value smp_val = json::Object{
    { "key1", 42 },
    {"key2", "value2"},
    {"key3", true },
    {"arr", json::Array{ { 2, 3.14, nullptr } } },
    {"obj", json::Object{ {"nested_k", "nested_v"} } }
};
```

**Note:** Use `Array{}` for empty array initialization, and `Array{ { ... } }` with double braces for non-empty initialization, otherwise in specific cases it will be considered copy construction/expansion construction rather than initializer list.

You can check types with `type()` or `is_xxx()` functions, or use `type_name()` to get the type name in string form:

```cpp
smp_val.type();        // Returns Type::eObject
smp_val.type_name();  // Returns "Object", without the `e` prefix
smp_val.is_arr();     // Returns false
```

There are six `is` functions: `arr`, `obj`, `str`, `num`, `bol`, and `nul`, corresponding to the six JSON types.

You can reset content through assignment statements, or use the `reset()` member function.
This is a template function that defaults to resetting to `Null` type, but you can explicitly specify the reset type, such as using `reset<json::Object>()` to reset content to an empty `Object`.

The template parameter of `reset` can only be one of the six JSON types, otherwise it won't compile.

### 2. Access and Modification

This library provides `xxx()` member functions to get **references** to internal data, where `xxx` is the same as the `is_xxx` above.

```cpp
 // Note that Object's mapped is still Value type
json::Value& vi_42 = smp_val.obj()["key1"];

// Although it returns a reference, it can also be used for assignment
double i_42 = vi_42.num(); 

 // vi_42.str(); // Type mismatch, throws std::bad_varient_access exception
```

`Value` also provides `[]` and `at` operators. The difference is that `at` prohibits index out-of-bounds (throws exception), while `[]` doesn't check bounds (so `Object` can use `[]` to create new key-value pairs, but `Array` out-of-bounds is undefined behavior and may crash directly).

> The `const` version of `[]` is special, equivalent to `const` `at`, and cannot create new key-value pairs or go out of bounds.

```cpp
smp_val["arr"][1].num(); // Returns 3.14
smp_val.at("obj").at("nested_k") = nullptr; // Modify object, becomes Null type
smp_val["obj"].at("nested_k").is_nul(); // Returns true, [] and at can be mixed freely
```

We also provide some helper functions to simplify array and object operations, avoiding too many `get<>` calls.
These functions are only valid for arrays and objects, but they perform type and range checks and won't throw exceptions, so they can be used "freely".

| Function Name           | Purpose                      | Return Value | Description                                                                                     |
|-------------------------|------------------------------|--------------|-------------------------------------------------------------------------------------------------|
| `size()`                | Get element count            | `size_t`     | Returns sub-element count for arrays or objects, 0 for other types (including strings)          |
| `empty()`               | Check if empty               | `bool`       | Returns whether empty for arrays or objects, **true** for other types (including strings)       |
| `push_back( val )`      | Add element to array end     | `bool`       | Returns true only when it's an array and addition succeeds                                      |
| `pop_back()`            | Remove last array element    | `bool`       | Returns true only when it's an array and removal succeeds (false for empty array)               |
| `insert(idx, val)`      | Insert element at position   | `bool`       | Returns true only when array insertion at specified position succeeds (false for out-of-bounds) |
| `insert(idx, cnt, val)` | Insert multiple elements     | `bool`       | Returns true only when array insertion at specified position succeeds (false for out-of-bounds) |
| `erase(idx)`            | Remove element at position   | `bool`       | Returns true only when array removal at specified position succeeds (false for out-of-bounds)   |
| `contains( key )`       | Check if object contains key | `bool`       | Returns true only when it's an object and contains the specified key                            |
| `insert( key, val )`    | Add key-value pair to object | `bool`       | Returns true only when it's an object and addition succeeds (false if already exists)           |
| `erase( key )`          | Remove object key-value pair | `bool`       | Returns true only when it's an object and removal succeeds (false if doesn't exist)             |

The above functions also support move semantics for `Value` parameters.

`get` can only get references to the six types, so this library also provides `to` and `move` templates to get internal values and force type conversion. The former is always a copy, the latter is move or copy (simple types, or copy when move is not needed).

```cpp
auto str_view = smp_val["key2"].to<std::string_view>(); // Returns a view of the internal string
auto str = smp_val["key2"].move<std::string>(); // Moves the internal string out, now internal becomes empty string, previous view is no longer usable
int int_42 = smp_val["key1"].to<int>(); // Returns a copy of the internal integer
```

Note that `Number` data is stored as `double`, so when converting to integers (enum types, or types that meet integer template requirements), it will **round** to avoid precision issues.

Note that `to` and `move` support many types, and conversion failure will throw `std::runtime_error` exception.
For this, we also provide `xx_if` and `xx_or` versions, the former returns `optional<T>`, the latter returns a specified default value on failure.

```cpp
auto opt_str = smp_val["key1"].to_if<std::string>(); // opt_str is std::optional<std::string>, empty on conversion failure but doesn't throw exception
if(opt_str) std::cout << *opt_str << std::endl; // If conversion succeeds, output the string
std::string or_str = smp_val["key1"].to_or<std::string>("default"); // If conversion fails, return "default"
```

Conversion has very precise rules and test order. For detailed content, please refer to [github-pages](https://v-craft.github.io/vct-tools-json/) documentation or source code comments.

This library provides three concepts for querying whether types can possibly be converted: (types that don't satisfy any cannot use `to/move` templates and won't compile)

1. `json::convertible<T>` types `T` that can possibly **directly** convert successfully, including types that satisfy `json::json_type<T>`.
2. `json::convertible_map<T,D>` mapping types `T` that can possibly **indirectly** convert successfully, where keys must be strings and values (mapped) are type `D` satisfying condition 1.
3. `json::convertible_array<T,D>` array types `T` that can possibly **indirectly** convert successfully, where internal values are type `D` satisfying condition 1.

> Actually, arrays or mappings that can directly convert successfully satisfy both condition 1 & (2 | 3), but this isn't important.

As long as a type satisfies any of the three concepts, you can use the `to` and `move` series functions for conversion. We'll detail this in the upcoming "Custom Type Serialization" section.

### 3. Serialization and Deserialization

Serialization and deserialization in this library are very efficient and easy to use.

First is deserialization, converting strings to `Value` objects, using `json::read()` functions.

```cpp
std::string json_str1 = R"( [ 1, false, null, { "Hello": "World" } ] )";
std::string json_str2 = R"( false )"; // Top-level type can be any JSON type
json::Value val1 = json::read(json_str1).value_or( nullptr ); // Parse JSON string
std::cout << val1[1].to<bool>() << std::endl; // Output 0 (no boolaplha specified)
```

Here, three things need explanation:

1. JSON file parsing failures are common and unpredictable in real applications, as there can easily be format errors or garbage data.
Therefore, this library's deserialization functions return `std::expected<Value, ParseError>`, avoiding the exception mechanism and reducing overhead. The latter is an enum describing error types.

2. This function also has an optional parameter `max_depth` (default is 256) to limit the maximum (nesting) depth of parsing.
Although this library guarantees that total parsing complexity is `O(n)` (strict single traversal), it uses recursion to handle nested structures, so this parameter is needed to avoid issues with certain garbage data (such as overly long `[[[[[[[[]]]]]]]]` which might cause stack overflow during recursion).

3. Besides `std::string`, this function can also take `std::istream` for streaming parsing.
Streaming file parsing efficiency is almost equivalent to first reading the entire file into a string and then parsing the string, but memory usage may be less.

Next is serialization, using the `dump/write` series member functions of `Value` objects.

```cpp
std::string str_ser = val1.dump(); // Efficient serialization without invalid whitespace, returns std::string
std::string str_back;
val1.write( str_back ); // Write serialization result to the end of std::string
val1.write( std::cout ); // Output serialization result directly to `ostream`
```

Now `str_ser` and `str_back` have exactly the same content, because `dump` is implemented using `write`.

Since `Value` is always valid JSON data, `dump` always succeeds.
However, `write` stream operations may not always succeed (such as file suddenly closing). The function will return immediately when it detects the stream state is `fail()`, but won't throw exceptions. You need to check the stream state yourself.

The above three serialization functions are all efficient compact serialization without invalid whitespace.
But you can use the `dumpf/writef` series functions to get more readable formatted output, `f` is `format`, which also comes in three forms:

```cpp
std::string pretty_str = val1.dumpf().value_or( "fail" );
bool res = val1.writef( std::cout ); // Output to `ostream`, returns whether successful
if (!res){ std::cerr << "fail"; }   // There's also to string, omitted here
```

The `f` series has three optional parameters: "spaces per indent (default 2)", "initial indent count (default 0)", and "maximum **single-line** indent spaces (default 512)".

The first two parameters control indent style, which is easy to understand. The third parameter is still to avoid garbage data like `[[[[]]]]`. Such data isn't long in text itself, but becomes very large with indentation, so this parameter is used to limit it.
This is why the return values of the prettify series are different. `dumpf()` returns `std::optional<std::string>`, while the other two return `bool` indicating success.

Prettify serialization failure is usually due to indent length overflow, but stream processing returning `false` might also be because the stream itself `fail`ed.

### 4. Equality Operator

The `Value` type provides an `==` operator for comparison with `Value`. It first checks if types are the same, then calls the internal `==` for comparison (`std::map` and `std::vector` comparison is based on sub-element content, thus implementing recursive comparison).

```cpp
json::Value val_arr_1 = json::Array{{ 1, 2, 3 }};
json::Value val_arr_2 = json::Array{{ 1, 2, 3 }};
json::Value val_arr_3 = json::Array{{ 1, true, 3 }};
val_arr_1 == val_arr_2; // true
val_arr_1 == val_arr_3; // false
```

More specially, `Value` also implements `==` comparison with any other type through template functions.

Incompatible types directly return `false`. If the target is one of the six JSON types, it first tests if types match, then compares specific values.
Otherwise, it tries to convert the object to `Value`, or convert `Value` to the target type and then compare. If neither matches, it returns `false`.

The `==` operation always succeeds and won't throw exceptions.

## **Custom Type Serialization**

Any custom type that provides a constructor for `Value` and type conversion functions can interact with JSON data through `to/move` or type conversion, thus achieving rapid serialization and deserialization.

> Providing constructor and type conversion functions for `json::Value` satisfies the `json::convertible<T>` concept.

These functions have some detailed requirements, and their implementation is not easy, so this library provides a header file containing only macro definitions, allowing you to easily implement interaction between custom types and JSON, even supporting move semantics.

> You can browse this header file yourself. It has very little content, but can help you understand what types can satisfy conversion conditions.

```cpp
#define M_VCT_TOOLS_JSON_SIMPLIFY_MACROS // Define macro to enable simplified macro function names
#include <vct/tools/json_macros.hpp>
// Recommend placing all header files before all imports, although this file only contains macro definitions
import std;
import vct.tools.json;
using namespace vct::tools;
```

Suppose you now have such a type:

```cpp
struct MyData{
    int id{};
    std::string m_name{};
    bool active{};
    double m_value{};
};
```

Then you can add constructor and conversion functions for it through macro definitions as shown below, but you need to explicitly enable default construction:

```cpp
struct MyData{
    int id{};
    std::string m_name{};
    bool active{false};
    double m_value{};

    MyData() = default; // Must have default constructor, content can be customized

    M_JSON_CV_FUN( MyData,  // Conversion function, must be in public scope
        M_JSON_CV_MEM( id );    // Note, there must be a `,` after MyData
        M_JSON_CV_MAP( name, m_name )   // But remaining fields cannot have commas `,`, semicolons `;` are optional
        M_JSON_CV_MEM( active )
        M_JSON_CV_MAP( value, m_value )
    )
    M_JSON_CS_FUN( MyData,  // Constructor, must be in public scope
        M_JSON_CS_MEM( id )
        M_JSON_CS_MAP( name, m_name )
        M_JSON_CS_MEM_OR( active, true, nullptr ) // Default value is `true`
        M_JSON_CS_MAP_OR( value, m_value, 64.0, nullptr ) // nullptr means this type doesn't need sub-element default value
    )
};
```

`CV` refers to `Conversion` functions, while `CS` refers to `Constructor` functions. Their first parameter is the type name, followed by a `,` separator.
Then specify the fields needed in JSON conversion through corresponding macro definitions. `MEM` means member variable name is the same as JSON key name, `MAP` means member variable name differs from JSON key name (e.g., key is `name`, but member variable name is `m_name`).

> You can choose to define some simplified macros yourself, such as `JCSM` `JCSP` etc., to highly simplify writing.

Conversion functions always succeed because the needed data are all member variables.
But member assignment in constructor functions might fail because corresponding keys might not exist in `json::Value` (or `Value` might not even be `Object` type), so member default values need to be specified.

You'll see constructor (`CS`) macros, some with `OR` suffix, which have two additional parameters, the first being the default value.
Macros without `OR` don't lack default values, but use the default construction of the corresponding type as default value, i.e., `decltype(name){}`.

The author recommends that field default values should be consistent with member variable default values, because we want the result of failed conversion from `json::Value` to equal the effect of the default constructor.
(The `active` default value above differs from what's specified in `CS`, which is not recommended)

Then you can make `json::Value` and `MyData` convert to each other as shown below:

```cpp
json::Value v_null; 
MyData d_null{ v_null }; // Nothing there, so all fields are default values in CS
d_null.active; // true, because CS function specified default value as true

json::Value v_object{ json::Object{} };
v_object["id"] = 42;
v_object["name"] = "Test User";
v_object["active"] = false;
v_object["value"] = 128.0;
MyData d_object{ v_object };    // Must explicitly convert, cannot use `=` construction
d_object.m_name == "Test User"; // true

json::Value v_data{ d_object }; // Convert MyData to JSON object
v_data["id"] == 42; // true
```

When using these two macros, it is very important that the **member variables** to be converted must support conversion with the `json::Value` type.

1. For basic arithmetic types, enum types, the six JSON types, and `json::Value` itself, this requirement is always satisfied.
2. For other custom class types, you need to provide conversion functions and constructors as shown above.
3. For lists composed of types that satisfy condition 1 or 2 (such as `std::vector`, `std::list`, etc.), you can use them directly.
4. For maps composed of types that satisfy condition 1 or 2 (such as `std::map`, `unordered_map`, etc.), you can use them directly when the key is `std::string`.

Conditions 1 and 2 refer to the concept `json::convertible<T>`, while conditions 3 and 4 refer to the concepts `json::convertible_array<T,D>` and `json::convertible_map<T,D>`.

For example, the current `MyData` type already provides conversion functions and constructors via macros, satisfying condition 2.
Therefore, you can use it directly in other types to implement nested JSON objects:

```cpp
struct MyData2 {
    std::string name;   // std::string is equivalent to json::String, so it can be used directly
    MyData my_data;     // MyData already has conversion functions and constructors, so it can be used directly
    std::vector<MyData> data_list;  // Lists composed of directly usable types can also be used directly (but nesting another list is not supported)
    MyData2() = default;
    M_JSON_CV_FUN( MyData2,
        M_JSON_CV_MEM( name )
        M_JSON_CV_MAP( data, my_data )
        M_JSON_CV_MEM( data_list )
    )
    M_JSON_CS_FUN( MyData2,
        M_JSON_CS_MEM( name )
        M_JSON_CS_MAP( data, my_data )
        M_JSON_CS_MEM_OR( data_list, std::vector<MyData>{}, MyData{} ) // field name, default value, default value for internal elements
    )
};
```

As you can see, we use the fourth parameter of the `OR` macro. The third parameter is the default value for the field itself, and the fourth is the default value for the sub-elements.
The fourth parameter is only useful when the target is an array or map type (and not `json::Array/Object`); otherwise, you can fill it with anything, usually `nullptr`.

For example, if you need an array but the internal `json::Value` is not an array, the third parameter's default value will be returned.
If `json::Value` is an array but only some elements can be converted to your type, the other elements will be filled with the fourth parameter's default value to ensure the array length is consistent.

You can then switch between the two types as shown below:

```cpp
json::Value v_data2{ MyData2{} };
std::println("");
v_data2.writef( std::cout );
std::println("");
v_data2["data"]["id"] = 8848;
v_data2["data"]["name"] = "Mount Everest";
v_data2["data"]["active"] = true;
v_data2["data_list"].push_back( v_data2["data"] );
v_data2["name"] = "name_name";
MyData2 d_data2{ v_data2 };
M_EXPECT_TRUE( d_data2.my_data.id == 8848 ); // true
M_EXPECT_TRUE( d_data2.my_data.m_name == "Mount Everest" ); // true
M_EXPECT_TRUE( d_data2.data_list.size() == 1 ); // true
M_EXPECT_TRUE( d_data2.data_list[0].id == 8848 ); // true
M_EXPECT_TRUE( d_data2.data_list[0].m_name == "Mount Everest" ); // true
M_EXPECT_TRUE( d_data2.name == "name_name" ); // true
```

> The `M_EXPECT_TRUE` here uses the vct-test-unit library, which you can ignore.

## **List and Map Extensions**

As things get more complex, this final section introduces the implementation details of lists and maps.

As mentioned earlier, only the six JSON types and basic arithmetic types can be directly converted with `json::Value`, while custom types need to provide conversion functions and constructors.
That is, only types that satisfy the `json::convertible<T>` concept can be directly converted.

But what about standard library types like `array<int>`? They are commonly used, and their internal `int` satisfies the conversion condition, but the overall type does not, and you cannot provide conversion functions and constructors for them.

Therefore, this library provides four template functions for `json::Value`, corresponding to conversions between `array type <-> json::Value` and `map type <-> json::Value`.

What types can use these templates? First, the key of the map type must be `std::string` or convertible to `std::string`.
Most importantly, the internal value type must be `json::convertible<T>`, because these types can be directly converted.

This is why there are two separate concepts: `json::convertible_map<T,D>` and `json::convertible_array<T,D>`.

Converting from array/map to `json::Value` will not lose any elements, because all elements can be accepted by `json::Value`.
But the reverse is not true; converting from `json::Value` to array/map may lose some elements, because `json::Value` may contain various strange data and formats.

Therefore, if your array or map is not the basic type `json::Array` or `json::Object`, you must provide two default values when converting:

1. The default result returned when there is a complete mismatch. For example, if you want to convert to an array but the internal `json::Value` is not an array, this default value is returned directly.

2. The default value for sub-elements when there is a partial match but some elements do not match. For example, if you need a `vector<int>` but `json::Value` contains `[1, 2, [], 3]`, you need to specify the default integer to fill in for unmatched elements like `[]`.

This is why the macros `M_JSON_CS_MEM_OR` and `M_JSON_CS_MAP_OR` require two default values.
However, if the type you are converting to is not an array or map, the last sub-element default value can be anything; above, we used `nullptr` as the default value.

This content in the code actually corresponds to the `to/move` series of functions.

For basic types or custom type data, you can convert directly as before:

```cpp
xxx = val.to<MyData>(); // or move<MyData>()
```

But if you need to convert to an array, you need to explicitly specify the default value for sub-elements:

```cpp
// The actual template has two parameters: the target type and the type for filling sub-elements
xxx = val.to<std::vector<MyData>, MyData>( MyData{} );
// The second template parameter can be deduced automatically from the function argument
xxx = val.to<std::vector<MyData>>( MyData{} ); 
```

The second template parameter defaults to `json::Null`, i.e., `std:::nullptr_t`. If the conversion target is not an array or object, you do not need to add it.

Note that `to/move` will throw an exception when there is a complete mismatch, so we only specify the default value for sub-elements.
The macros are actually implemented by `to_or` and `move_or`, so two default values are required:

```cpp
// The second template parameter is deduced automatically
xxx = val.to_or<std::vector<MyData>>( std::vector<MyData>{} , MyData{} ); 
```

## **Conclusion**

This covers the basic usage of the library. Although the custom type serialization part is relatively complex, you can read the documentation and source code yourself. The effective lines of code in this library are actually very few (less than 2000 lines).
Focus on the implementation of `to` and `move`, as well as the macro definitions in the header file, and you should be able to get started quickly.

If you find any issues or optimizations for this library, feel free to submit an issue or PR.

</div>
