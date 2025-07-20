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

作为 README ，这里只进行粗略介绍，详细说明请参考 `examples` 下的 [文档](./examples/docs.md) 和示例代码。

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
target_link_libraries(test_json PRIVATE vct::tools-json)
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

注意到这里有一个特殊类型 `Value` ，它也由本库定义，内部包含一个 `Type` 枚举值和一个 `std::variant` ，后者可以存储上述任一类型的值。

> 选用 `map` 而非 `unordered_map` 是为了保证序列化结果的一致性和时间的稳定性。
> 但你可以参考 [文档](./examples/docs.md) ，通过宏定义显式选用 `unordered_map` 。

## **基础操作**

### 1. 初始化

`Value` 的默认构造函数会创建 `Null` 类型的值，但你可以指定类型或者直接传值初始化：

```cpp
json::Value null_val;                // 默认构造，类型为 Null
json::Value bool_val(3);             // 整数初始化，类型为 Number
json::Value obj_val = json::Type::eObject; // 使用枚举初始化，类型为 Object
```

`json::Value` 在上述六种类型和 `json::Type` 外，还支持基本算术类型、枚举类型（视为整数）和 `const char*` 的隐式构造函数和赋值函数。

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

**注意**，空数组初始化请使用 `Array{}` ，非空初始化请使用 `Array{ { ... } }` 双重花括号，否则在特定情况下会被认为是拷贝构造/扩容构造而非初始化列表。

你可以用 `type()` 或 `is()` 方法检查类型，或者使用 `type_name()` 获取字符串形式的类型名（后者应该仅用于调试）：

```cpp
smp_val.type();        // 返回 Type::eObject
smp_val.type_name();  // 返回 "Object" ,没有前缀 `e`
smp_val.is(json::Type::eArray); // 返回 false
```

可以用 `reset()` 将内容重置为 `Null` ，或者使用 `clear_data()` 仅清理内部数据而不改变类型（比如数组将变成空数组）。 

### 2. 访问和修改

本库提供了 `get<>()` 成员函数以获取内部数据的引用，参数必须是六种JSON类型之一（否则模板无法实例化）。

```cpp
json::Value vi_42 = smp_val.get<json::Object>()["key1"]; // 注意 Object 的 mapped 值依然是 Value 类型
double i_42 = vi_42.get<json::Number>(); // 获取内部值, json::Number 等价于 double
 // vi_42.get<int>(); // 编译时错误，Number 数据使用 double 存储，不可能获得 int& ，模板无法实例化
 // vi_42.get<json::String>(); // 运行时错误，内部类型不匹配，抛出 runtime_error 异常
```

`Value` 还提供了 `[]` 和 `at` 运算符，区别在于 `at` 禁止索引越界（抛出异常），而 `[]` 不检查越界（所以`Object`可以用`[]`创建新键值对，但`Array`越界是未定义行为，可能直接断言崩溃）。

```cpp
smp_val["arr"][2].get<json::Number>(); // 返回 3.14
smp_val.at("obj").at("nested_k") = nullptr; // 修改对象，变为 Null 类型
smp_val["obj"].at("nested_k").is(json::Type::eNull); // 返回true, [] 和 at 可以随意混合使用
```

`get` 只能获取六种类型的引用，这很不方便。所以本库还提供了 `to` 和 `move` 模板来获取内部的值，并强制转换类型。前者必然是拷贝，后者是移动或拷贝（简单类型，或者无需移动时进行拷贝）。

```cpp
auto str_view = smp_val["key2"].to<std::string_view>(); // 返回内部字符串的视图
auto str = smp_val["key2"].move<std::string>(); // 将内部字符串移动了出来，现在内部变为空字符串，之前的视图不再可用
int int_42 = smp_val["key1"].to<int>(); // 返回内部整数的拷贝
```

特别注意，`Number` 数据使用 `double` 存储，因此在转换成整数时（枚举类型，或符合整数模板要求的类型），会**四舍五入**，避免精度问题。

注意，`to` 和 `move` 支持很多类型（要求类型**有可能**转换成功），转换失败会抛出 `std::runtime_error` 异常。
为此，我们还提供了 `xx_if` 和 `xx_or` 版本，前者返回 `optional<T>` ，后者则是在失败时返回指定的默认值。

```cpp
auto opt_str = smp_val["key1"].to_if<std::string>(); // opt_str 是 std::optional<std::string> ，转换失败为空，但不抛出异常
if(opt_str) std::cout << *opt_str << std::endl; // 如果转换成功，输出字符串
std::string or_str = smp_val["key1"].to_or<std::string>("default"); // 如果转换失败，返回 "default"
```

转换具有非常准确的规则与测试顺序，详细内容请参考 [文档](./examples/docs.md) 。

### 3. 序列化与反序列化

本库的序列化和反序列化非常高效且容易使用。

首先是反序列化，即将字符串转换为 `Value` 对象，使用 `deserialize()` 或 `parse()` 函数，它们完全等价。

```cpp
std::string json_str1 = R"( [ 1, false, null, { "Hello": "World" } ] )";
std::string json_str2 = R"( false )"; // 允许顶层类型是任一 JSON 类型
json::Value val1 = json::deserialize(json_str1).value_or( nullptr ); // 解析 JSON 字符串
std::cout << val1[1].to<bool>() << std::endl; // 输出 0 （没有指定 boolaplha）
```

这里需要要说明三件事。

首先，JSON 文件解析失败在实际应用中很常见且难以预料，因为很容易有一些垃圾数据或格式错误问题。
因此本库的反序列化函数返回 `std::expected<Value, ParseError>` ，从而避免使用异常机制，减小开销。后者是一个描述错误类型的枚举，可以在 [文档](./examples/docs.md) 中找到相关信息。

其次，此函数还具有一个可选参数 `max_depth`（默认是 256），用于限制解析的最大深度。
本库虽然保证总解析耗时是 `O(n)` 的（严格单次遍历），但使用了递归来处理嵌套结构，因此需要用它避免某些垃圾数据的问题（比如过长的 `[[[[[[[[]]]]]]]]` 在递归时可能导致栈溢出）。

最后，此函数除了 `std::string` 外，还能传入 `std::istream` 进行流式解析。

> 流式解析文件的效率几乎等同于先将文件全部读入 string 再用 string 解析，但是前者的内存占用可能更少。

然后是序列化，使用 `Value` 对象的 `serialize()` 系列成员函数。

```cpp
std::string str_ser = val1.serialize(); // 不含无效空白字符的高效序列化，返回 std::string
std::string str_back;
val1.serialize_to( str_back ); // 将序列化结果写入 std::string 的末尾
val1.serialize_to( std::cout ); // 将序列化结果直接输出到 `ostream` 中
```

值得一提的是，现在 `str_ser` 和 `str_back` 的内容完全一样，因为 `serialize` 就是像用 `serialize_to` 实现的。

由于 `Value` 必然是有效的 JSON 数据，因此 `serialize` 必然成功。
`serialize_to` 的流操作不一定成功（比如文件突然关闭）、函数检测到流的状态为 `fail()` 后会立即返回，但不会抛出异常，需要你自行检查流的状态。

上面的三个序列化函数都是高效的紧凑序列化，不含无效空白字符。
但你可以使用 `serialize_pretty()` 系列函数来获得更易读的格式化输出，它同样包含三种形式：

```cpp
std::string pretty_str = val1.serialize_pretty().value_or( "fail" );
bool res = val1.serialize_pretty_to( std::cout ); // 输出到 `ostream`，返回是否成功
if (!res){ std::cerr << "fail"; }   // 还有 to 字符串，此处省略
```

`pretty` 系列有三个可选参数，依次是 “单次缩进空格数（默认 2）”，“初始缩进次数（默认 0）”和“最大单行缩进空格数（默认 512）”。

前两个参数控制缩进的样式，很好理解。第三个参数依然是为了避免 `[[[[]]]]` 这种垃圾数据，此类数据自身文本不长，但是带缩进美化后会变得非常大，因此使用此参数加以限制。
这就是为什么 `pretty` 系列的返回值有所不同， `serialize_pretty` 返回 `std::optional<std::string>` ，后二者返回 `bool` ，表示是否成功。

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

更加特殊的是， `Value` 还通过模板函数实现了和其他任意类型的 `==` 比较，可以在 [文档](./examples/docs.md) 中找到详细说明。

简单的说，不兼容的类型直接返回 `false` ，如果目标是六种 JSON 类型之一，则先测试类型是否匹配，然后比较具体值。
否则，尝试将对象转换为 `Value` ，或者将 `Value` 转换为目标类型然后比较。都不匹配则返回 `false` 。

此比较保证成功，不会抛出异常。

## **自定义类型序列化**

任何自定义类型，只要提供针对 `Value` 的构造函数和类型转换函数，就能通过 `to/move` 或者类型转换等方式与 JSON 数据交互，从而实现快速的序列化和反序列化。

这些转换函数的实现并不轻松，因此本库提供了一个仅包含宏定义的头文件，让你可以轻松实现自定义类型与 JSON 的交互，甚至支持移动语义。

```cpp
#define M_VCT_JSON_SIMPLIFY_MACROS // 定义宏，以启用简化的宏函数名
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
        M_JSON_CS_MEM_OR( active, true, nullptr ) // 默认值是 `true`, nullptr 表示这个类型不需要子元素默认值
        M_JSON_CS_MAP_OR( value, m_value, 64.0, nullptr ) // nullptr 表示这个类型不需要子元素默认值
    )
};
```

`CV` 的是指 `Conversion` 转换函数，而 `CS` 是指 `Constructor` 构造函数。它们的第一个参数都是类型名，后面需要一个 `,` 分隔符。
然后通过对应的宏定义指定 JSON 转换中需要的字段，`MEM` 是指成员变量名与 JSON 键名相同，`MAP` 是指成员变量名与 JSON 键名不同（比如键是 `name` ，而成员变量名是 `m_name`）。

> 你可以查看头文件，然后在项目中自己定义一些宏，比如 `JCSM` `JCSP` 等等，高度简化书写。

转换函数是必然成功的，因为需要的数据都是成员变量。
但是构造函数可能会失败，因为 `json::Value` 中可能不存在对应的键（甚至 `Value` 根本不是 `Object` 类型），因此需要指定默认值。

你会看到构造函数（`CS`）的宏，部分带有 `OR` 后者，它们多了两个参数，第一个参数就是默认值。
而没有 `OR` 的宏并非没有默认值，而是将对应类型的默认构造作为默认值，即 `decltype(name){}` 。

作者建议是，字段的默认值请和成员变量的默认值保持一致，因为我们希望从 `json::Value` 中转换失败的结果，应该等于默认构造函数的效果。
（上面 `active` 的默认值就和 `CS` 中指定的不一样，不推荐这种写法）

然后你就可以像下面这样，让 `json::Value` 和 `MyData` 互相转换了：

```cpp
json::Value v_null; 
MyData d_null{ v_null }; // 什么都没有，因此全部字段都是 CS 中的默认值
d_null.active; // true，因为 CS 函数指定了默认值为 true

json::Value v_object{ json::Type::eObject };
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
json::Value v_data2{ data2 };
std::println("");
v_data2.serialize_pretty_to( std::cout );
std::println("");
v_data2["data"]["id"] = 8848;
v_data2["data"]["name"] = "Mount Everest";
v_data2["data"]["active"] = true;
v_data2["data_list"].get<json::Array>().emplace_back( v_data2["data"] );
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
但是，像 `array<int>` 这种标准库提到的类型怎么办呢？它很常用，但无法为其提供转换函数和构造函数。

因此，本库为 `json::Value` 提供了四个模板，分别对应 `数组类型->json::Value` 和 `json::Value->数组类型` 以及 `映射类型->json::Value` 和 `json::Value->映射类型` 的转换。

什么类型能够用这些模板呢？首先映射类型的键必须是 `std::string` 或者可以转换为 `std::string` 的类型。
最重要的是内部的值类型，要求是 “六种JSON类型 或 基本算术类型 或 提供了转换函数和构造函数的自定义类型” ，因为这些类型能够直接转换。

`数组/映射->json::Value`是不会遗漏任何元素的，因为所有元素都能被 `json::Value` 接受。
但是反之则不然，`json::Value->数组/映射` 可能会丢失一些元素，因为 `json::Value` 可能有各种奇怪的数据和格式。

因此，如果你的数组和映射不是基本类型里的 `json::Array` 和 `json::Object` ，那么在转换时必须提供两个默认值。

1. 完全不匹配时返回的默认结果。比如需要转换成数组，但是 `json::Value` 内部不是数组，则直接返回此默认值，。
2. 能够匹配类型，但是局部元素不匹配时填充的子元素默认值。比如需要 `vector<int>` ，但是 `json::Value` 中是 `[1, 2, [], 3]` ，你需要指定遇到 `[]` 这些不匹配元素时填充的默认整数。

这也就是为什么 `M_JSON_CS_MEM_OR` 和 `M_JSON_CS_MAP_OR` 宏定义需要两个默认值。
不过，如果你转换的类型不是数组或者映射，最后这个子元素默认值可以任意填写，上面我们就使用过 `nullptr` 作为默认值。

这些内容在代码中实际对应 `to/move` 系列函数。

对于基本类型或者自定义类型的数据，可以像之前一样直接转换：

```cpp
xxx = val.to<MyData>(); // 或者 move<MyData>()
```

但如果需要转换成数组，就需要显式指定子元素默认值：

```cpp
// 实际模板有两个参数，第一个是目标类型，第二个是填充子元素的类型
xxx = val.to<std::vector<MyData>, MyData>( MyData{} );
// 第二个模板参数是可选的，默认为 nullptr_t，非数组/映射类型用不到它，即使用到也可以自动推导。 
xxx = val.to<std::vector<MyData>>( MyData{} ); // 根据参数类型自动推导第二个参数
```

注意，使用 `to` 在完全不匹配时直接抛出异常，所以我们只指定了子元素默认值。
而宏定义实际由 `to_or` 和 `move_or` 实现，因此才是两个默认值。

```cpp
xxx = val.to_or<std::vector<MyData>>( std::vector<MyData>{} , MyData{} ); 
```

## **最后**

以上就是本库的基本使用，虽然自定义类型序列化的部分比较复杂，但你可以自行阅读 [文档](./examples/docs.md) 和源码，本库的源码的有效行数其实非常少（不足 2000 行）。
重点观察 `to` 和 `move` 的实现，以及头文件中的宏定义，你应该能很快上手。

如果你发现的本库的任何问题，或者可优化的地方，欢迎提交 issue 或 PR。

</div>

---

<div id="ENGLISH">

> Translated by Copilot

## **Introduction**

vct-tools-json is a C++23 JSON library providing concise, efficient JSON parsing, manipulation, and serialization.

This README is a brief overview. For detailed documentation and examples, see docs in the examples folder.

## **Importing the Library**

This library uses CMake and requires C++23 standard library module support. You can download the source and add it to your project, or use CMake's `FetchContent` to fetch it automatically:

```cmake
include(FetchContent)   # Include FetchContent module
FetchContent_Declare(
    vct-tools-json      # Declare vct-tools-json library, add SHA and tag as needed
    URL  https://github.com/v-craft/vct-tools-json/archive/refs/tags/xxxx.tar.gz
)
FetchContent_MakeAvailable(vct-tools-json) # Download and add to your project
```

If you use the source directly, remember to add it with `add_subdirectory` in your CMake project. `FetchContent` will handle dependencies automatically.

> vcpkg is not supported yet, as standard library modules are still experimental and UUIDs depend on CMake version.

Link the library simply:

```cmake
target_link_libraries(test_json PRIVATE vct::tools-json)
```

Finally, import the JSON module in your code:

```cpp
import std; // Enable C++23 std module support
import vct.tools.json; // Import vct-tools-json
using namespace vct::tools; // Use the namespace for convenience
```

## **Basic Types**

The JSON standard defines six types, mapped in this library to C++ types in the `vht::tools::json` namespace:

| JSON Type | Library Type | C++ Type | Enum Value |
|-----------|-------------|----------|------------|
| `null`    | `Null`      | `std::nullptr_t`               | `Type::eNull`   |
| `bool`    | `Bool`      | `bool`                         | `Type::eBool`   |
| `number`  | `Double`    | `double`                       | `Type::eNumber` |
| `string`  | `String`    | `std::string`                  | `Type::eString` |
| `array`   | `Array`     | `std::vector<Value>`           | `Type::eArray`  |
| `object`  | `Object`    | `std::map<std::string, Value>` | `Type::eObject` |

Note the special type `Value`, defined by this library, which contains a `Type` enum and a `std::variant` holding any of the above types.

> `map` is used instead of `unordered_map` to ensure consistent serialization and stable timing. You can use macros to select `unordered_map` if needed (see docs).

## **Basic Operations**

### 1. Initialization

The default constructor for `Value` creates a `Null` type, but you can specify the type or initialize with a value:

```cpp
json::Value null_val;                // Default constructor, Null type
json::Value bool_val(3);             // Integer initializes as Number
json::Value obj_val = json::Type::eObject; // Enum initializes as Object
```

`json::Value` supports implicit constructors and assignment for the six JSON types, `json::Type`, arithmetic types, enums (as integers), and `const char*`.

Although `Value` does not support initializer lists directly, you can quickly create objects using initializer lists for `Array` and `Object`:

```cpp
json::Value smp_val = json::Object{
    { "key1", 42 },
    {"key2", "value2"},
    {"key3", true },
    {"arr", json::Array{ { 2, 3.14, nullptr } } },
    {"obj", json::Object{ {"nested_k", "nested_v"} } }
};
```

**Note:** Use `Array{}` for empty arrays, and `Array{ { ... } }` (double braces) for non-empty arrays. Otherwise, it may be treated as copy/expansion constructor, not an initializer list.

Check types with `type()` or `is()`, or get the type name string (for debugging) with `type_name()`:

```cpp
smp_val.type();        // Returns Type::eObject
smp_val.type_name();   // Returns "Object" (no `e` prefix)
smp_val.is(json::Type::eArray); // Returns false
```

Use `reset()` to reset to Null, or `clear_data()` to clear internal data without changing type (e.g., array becomes empty).

### 2. Access and Modification

Use the `get<>()` member to get a reference to the internal data. The parameter must be one of the six JSON types:

```cpp
json::Value vi_42 = smp_val.get<json::Object>()["key1"];
double i_42 = vi_42.get<json::Number>();
// vi_42.get<int>(); // Compile error: Number is stored as double
// vi_42.get<json::String>(); // Runtime error: type mismatch, throws runtime_error
```

`Value` also provides `[]` and `at` operators. `at` checks bounds (throws), `[]` does not (undefined for out-of-bounds in Array, but creates new key for Object):

```cpp
smp_val["arr"][2].get<json::Number>(); // Returns 3.14
smp_val.at("obj").at("nested_k") = nullptr; // Modify object to Null
smp_val["obj"].at("nested_k").is(json::Type::eNull); // true, [] and at can be mixed
```

`get` only works for the six types, which can be inconvenient. The library also provides `to` and `move` templates to get or convert the value. `to` always copies, `move` moves or copies as needed:

```cpp
auto str_view = smp_val["key2"].to<std::string_view>(); // Get string view
auto str = smp_val["key2"].move<std::string>(); // Move out string, now empty inside
int int_42 = smp_val["key1"].to<int>(); // Copy as int
```

Note: `Number` is stored as double, so converting to int (enum or integer types) will **round** to avoid precision issues.

`to` and `move` support many types (if conversion is possible). Failed conversions throw `std::runtime_error`.
There are also `xx_if` and `xx_or` versions: the former returns `optional<T>`, the latter returns a default value on failure.

```cpp
auto opt_str = smp_val["key1"].to_if<std::string>(); // optional<string>, empty if fails, no exception
if(opt_str) std::cout << *opt_str << std::endl;
std::string or_str = smp_val["key1"].to_or<std::string>("default"); // Returns "default" if fails
```

Conversion rules and order are precise; see docs for details.

### 3. Serialization and Deserialization

Serialization and deserialization are efficient and easy to use.

Deserialization (string to `Value`) uses `deserialize()` or `parse()` (identical):

```cpp
std::string json_str1 = R"( [ 1, false, null, { "Hello": "World" } ] )";
std::string json_str2 = R"( false )";
json::Value val1 = json::deserialize(json_str1).value_or( nullptr );
std::cout << val1[1].to<bool>() << std::endl; // Outputs 0
```

Three notes:
- Parsing failures are common and unpredictable, so deserialization returns `std::expected<Value, ParseError>` (see docs).
- The function has an optional `max_depth` parameter (default 256) to limit nesting and avoid stack overflow.
- The function also accepts `std::istream` for stream parsing.

> Stream parsing is as efficient as reading the whole file into a string, but may use less memory.

Serialization uses the `serialize()` family of member functions:

```cpp
std::string str_ser = val1.serialize(); // Compact, no extra whitespace
std::string str_back;
val1.serialize_to( str_back ); // Append to string
val1.serialize_to( std::cout ); // Output to ostream
```

`serialize` always succeeds. `serialize_to` may fail if the stream fails, but does not throw; check the stream state yourself.

All three produce compact output. For pretty formatting, use `serialize_pretty()`:

```cpp
std::string pretty_str = val1.serialize_pretty().value_or( "fail" );
bool res = val1.serialize_pretty_to( std::cout );
if (!res){ std::cerr << "fail"; }
```

`pretty` functions have three optional parameters: indent size (default 2), initial indent (default 0), and max single-line indent (default 512).

The third parameter limits pretty output for deeply nested data. `serialize_pretty` returns `optional<string>`, the others return `bool` for success.

### 4. Equality Operator

`Value` provides `==` for comparing with another `Value`. It first checks type, then compares contents recursively (using `std::map` and `std::vector` element comparison).

```cpp
json::Value val_arr_1 = json::Array{{ 1, 2, 3 }};
json::Value val_arr_2 = json::Array{{ 1, 2, 3 }};
json::Value val_arr_3 = json::Array{{ 1, true, 3 }};
val_arr_1 == val_arr_2; // true
val_arr_1 == val_arr_3; // false
```

`Value` also provides template `==` for other types. See docs for details.

In short, incompatible types return false. If the target is a JSON type, it checks type and value. Otherwise, it tries to convert the object to `Value`, or convert `Value` to the target type for comparison. If neither matches, returns false.

Comparison always succeeds, never throws.

## **Custom Type Serialization**

Any custom type can interact with JSON by providing a constructor and conversion function for `Value`. The library provides macros to simplify this, even supporting move semantics.

```cpp
#define M_VCT_JSON_SIMPLIFY_MACROS
#include <vct/tools/json_macros.hpp>
import std;
import vct.tools.json;
using namespace vct::tools;
```

Suppose you have:

```cpp
struct MyData{
    int id{};
    std::string m_name{};
    bool active{};
    double m_value{};
};
```

Add macros for conversion and construction (default constructor required):

```cpp
struct MyData{
    int id{};
    std::string m_name{};
    bool active{false};
    double m_value{};

    MyData() = default;

    M_JSON_CV_FUN( MyData,
        M_JSON_CV_MEM( id );
        M_JSON_CV_MAP( name, m_name )
        M_JSON_CV_MEM( active )
        M_JSON_CV_MAP( value, m_value )
    )
    M_JSON_CS_FUN( MyData,
        M_JSON_CS_MEM( id )
        M_JSON_CS_MAP( name, m_name )
        M_JSON_CS_MEM_OR( active, true, nullptr )
        M_JSON_CS_MAP_OR( value, m_value, 64.0, nullptr )
    )
};
```

`CV` is for conversion, `CS` for construction. The first argument is the type name, separated by a comma. Use `MEM` for matching member/JSON key names, `MAP` for different names.

> See the header for more macros; you can define your own for brevity.

Conversion always succeeds. Construction may fail if the JSON lacks keys or is not an object, so defaults are needed.

Defaults in `CS` macros should match member defaults for consistency. If not, the result of failed conversion may differ from default construction.

Now you can convert between `json::Value` and `MyData`:

```cpp
json::Value v_null; 
MyData d_null{ v_null }; // All fields are defaults
json::Value v_object{ json::Type::eObject };
v_object["id"] = 42;
v_object["name"] = "Test User";
v_object["active"] = false;
v_object["value"] = 128.0;
MyData d_object{ v_object };
d_object.m_name == "Test User"; // true
json::Value v_data{ d_object };
v_data["id"] == 42; // true
```

Members must support conversion to/from `json::Value`:

1. Arithmetic types, enums, JSON types, and `json::Value` itself always work.
2. Other custom types need conversion and constructor macros.
3. Lists of supported types (e.g. `std::vector`, `std::list`) work directly.
4. Maps with `std::string` keys and supported value types work directly.

For example, you can nest custom types:

```cpp
struct MyData2 {
    std::string name;
    MyData my_data;
    std::vector<MyData> data_list;
    MyData2() = default;
    M_JSON_CV_FUN( MyData2,
        M_JSON_CV_MEM( name )
        M_JSON_CV_MAP( data, my_data )
        M_JSON_CV_MEM( data_list )
    )
    M_JSON_CS_FUN( MyData2,
        M_JSON_CS_MEM( name )
        M_JSON_CS_MAP( data, my_data )
        M_JSON_CS_MEM_OR( data_list, std::vector<MyData>{}, MyData{} )
    )
};
```

The fourth argument in `OR` macros is only needed for arrays/maps (not for `json::Array/Object`).
If the JSON is not an array, the third argument is used as the default. If some elements can't be converted, the fourth argument is used for those elements.

You can convert back and forth:

```cpp
json::Value v_data2{ data2 };
v_data2.serialize_pretty_to( std::cout );
v_data2["data"]["id"] = 8848;
v_data2["data"]["name"] = "Mount Everest";
v_data2["data"]["active"] = true;
v_data2["data_list"].get<json::Array>().emplace_back( v_data2["data"] );
v_data2["name"] = "name_name";
MyData2 d_data2{ v_data2 };
M_EXPECT_TRUE( d_data2.my_data.id == 8848 );
M_EXPECT_TRUE( d_data2.my_data.m_name == "Mount Everest" );
M_EXPECT_TRUE( d_data2.data_list.size() == 1 );
M_EXPECT_TRUE( d_data2.data_list[0].id == 8848 );
M_EXPECT_TRUE( d_data2.data_list[0].m_name == "Mount Everest" );
M_EXPECT_TRUE( d_data2.name == "name_name" );
```

> `M_EXPECT_TRUE` uses the vct-test-unit library; you can ignore it.

## **List and Map Extensions**

For more complex cases, the library provides templates for converting between array/map types and `json::Value`.

Map keys must be `std::string` or convertible. Value types must be JSON types, arithmetic types, or custom types with conversion macros.

Conversion from array/map to `json::Value` never loses elements. Conversion from `json::Value` to array/map may lose elements if the data is not compatible.

If your array/map is not the built-in `json::Array/Object`, you must provide two defaults:

1. Default result if the type does not match (e.g. not an array).
2. Default for elements that can't be converted (e.g. `[1, 2, [], 3]` to `vector<int>`).

This is why `M_JSON_CS_MEM_OR` and `M_JSON_CS_MAP_OR` need two defaults. For non-array/map types, the second default can be anything (often `nullptr`).

These templates are used in the `to/move` family:

For basic or custom types, convert directly:

```cpp
xxx = val.to<MyData>(); // or move<MyData>()
```

For arrays, specify the element default:

```cpp
xxx = val.to<std::vector<MyData>, MyData>( MyData{} );
xxx = val.to<std::vector<MyData>>( MyData{} ); // Type deduction
```

`to` throws if the type does not match, so only the element default is needed. Macros use `to_or` and `move_or`, so two defaults are required:

```cpp
xxx = val.to_or<std::vector<MyData>>( std::vector<MyData>{} , MyData{} ); 
```

## **Conclusion**

This covers the basics. Custom type serialization is complex, but you can read the docs and source code (less than 2000 lines). Focus on `to`/`move` and the macros for quick learning.

If you find any issues or improvements, feel free to submit an issue or PR.


</div>

---


