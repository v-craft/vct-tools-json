# C++23 JSON解析库

## 警告
- 本库使用C++23标准库默认，依然是试验性支持
- 库处于开发中，随时可能变更API/内容/模块名等

## 概要
- C++23
- 标准库模块
- 充分测试
- 性能优秀
- 操作简单
- 增删改查支持
- 移动语义支持
- 异常处理支持
- 初始化列表支持
- 序列化与反序列化支持
- 自定义类型序列化与反序列化支持
- 非常轻量，去除注释仅1000行代码

## 应用介绍

### 1. 导入模块与命名空间
```c++
import Vct.Tools.Json;
using namespace Vct::Tools;
// 也可以 using namespace Vct::Tools::Json;
// 但是容易导致名称冲突
```

### 2. 重要类型与函数介绍
```c++
// 广义的“值类型”，包含 对象 数组 等全部6种JSON数据类型。
class Json::Value;

// 对象类型，本质是 std::map<std::string, Json::Value>
class Json::Object;

// 数组类型，本质是 std::vector<Json::Value>
class Json::Array;

// 使用 JsonValue.type() 函数，获取内部JSON数据类型
enum class Json::Type{
    object, /**< JSON 对象类型 */
    array,/**< JSON 数组类型 */
    string, /**< JSON 字符串类型 */
    number, /**< JSON 数值类型 */
    boolean, /**< JSON 布尔类型 */
    null, /**< JSON null类型 */
};

// 反序列化函数
Json::Value Json::deserialize(std::string_view str);

// 序列化函数模板以及各种重载
std::string Json::serialize(T t);
```

### 3. 列表初始化
参考代码：
```c++
Json::Value json = {
    { "key", nullptr },
    { 1, 2, 3, 4, 5 },
    "string",
    true,
    false,
    1234.5
};
```

生成规则：
1. 如果初始化器为空，则生成null类型对象。
2. 如果初始化器只要1个对象，则等同于类型转换构造函数
3. 如果初始化器只有2个元素，且第1个元素是字符串，则生成object类型对象。
4. 其他情况则生成array类型对象。

显然编译器不能很好的区分array和object类型。<br>
需要时你可以使用如下方式创建：
```c++
// JsonObject本质是std::map<std::string, JsonValue>，必须套双重括号进行列表初始化
Json::Value json = Json::Array{
    Json::Object { {"key", nullptr} },
    Json::Array{ 1, 2, 3, 4, 5 },
    "string",
    true,
    false,
    1234.5
};
```

### 4. 序列化与反序列化
使用`Json::deserialize()`函数进行反序列化。<br>
使用`Json::serialize(T t)`函数进行序列化<br>

对于`Json::Value`对象，可以: <br>
使用`对象.serialize()`成员函数进行序列化。<br>
使用`对象.serialize_pretty()`函数函数进行美化序列化，可指定缩进长度。<br>
*（`Json::serialize(T t)`的T如果是`Json::Value`，就是直接调用`.serialize()`成员函数）*

参考代码：
```c++
Json::Value json = Json::deserialize(R"(
    {
        "语法": ["C++", "原始字符串", false ],
        "key": "支持\t中文\\\n与\"转义字符",
        "na\"\\me": [ 114,514 , null ],
        "map": [ {} , [ [ "嵌套" ] , {} ] ]
    }
    )");

// serialize序列化 不保留无效空格
std::cout << Json::serialize( json ) << std::endl;
std::cout << json.serialize() << std::endl;
// serialize_pretty序列化 带空格和换行，默认一次缩进2空格，可指定
std::cout << json.serialize_pretty() << std::endl;
```

注意，`对象.serialize()`成员函数是`Json::Value`类型特有的。

而`Json::serialize(T t)`函数是函数模板：<br>
- 对Json::Value，bool，nullptr等类型进行了普通函数重载，
- 对数值类型，可序列化类型（存在.serialize()成员函数)，字符串类型，标准数组类型，标准键值对类型等 进行了概念函数模板的编写。

所以他能够处理非常多的类型，甚至你的自定义类型（只要满足上述条件中的一种）。


### 4. 增删改查
需要注意的是，数组类型本质是`std::vector`，所以中间插入和删除元素是O(m)的，修改是正常O(1)。<br>
无特殊情况，尽量使用`push_back()`和`pop_back()`在末尾修改。

而对象类型的本质是`std::map`，操作都是O(log m)级别。

参考代码：
```c++
// json变量是上面【3. 反序列...】中的json变量
json.erase("na\"\\me"); // 删除
json["map"][1].clear(); // 清空
json["语法"] = 114514; // 修改
json["add"] = deserialize("[[[]]]"); //增加
json["add"].push_back(1); 

std::cout << json.serialize() << std::endl;
std::cout << json["key"].as_string() << std::endl; // 获取字符串并转义
```
可能的输出：
```
{"add":[[[]],1],"key":"支持\t中文\\\n与\"转义字符","map":[{},[]],"语法":114514}
支持	中文\
与"转义字符
```

### 5.使用is检测类型，使用as获取内容
参考代码：
```c++
Json::Value value = 123456789012345ll;
// is保证不会抛出异常
value.is_array(); // false
value.is_object(); // false
value.is_double(); // false 内部没有小数点
value.is_number(); // true int64和double都算number
// as 转换失败时抛出异常
value.as_int64(); // 123456789012345ll
value.as_double(); // 1.23457e+14 能够转化，但可能丢失精度
value.as_array(); // 抛出异常 Json::TypeException
```

### 6. 迭代器与移动语义支持
需要注意的是，**Json::Value类型不支持迭代器**，因为内部类型不确定。

但是可以通过`as_array()`和`as_object()`获取内部元素的引用，然后使用迭代器。<br>
因为Json::Array本质是`std::vector<Json::Value>`，而Json::Object是`std::map<std::string, Json::Value>`。

移动语义当然是完全支持的，且Json::Array和Json::Object对象可以赋值/移动给Json::Value，必然成功，不会抛出异常。

参考代码：
```c++
Json::Value my_obj = { "key1", { nullptr, 666 } };

Json::Value my_arr = Json::Array { true, Json::Object{} };

// 字符串构造，不会解析内部数据，不会报错，注意使用()而不是{}
Json::Value my_val ("[ {} this is string ]");

// as_array()和as_object()返回引用，其他的as返回副本
for(auto& it: my_arr.as_array()){ 
    // it 的类型是 Jsonlib::JsonValue&
    // 具体操作...
}

// 支持移动，被移动的对象变成JsonType::ISNULL类型，不会删除
my_arr.insert(1, std::move(my_obj["key"]));
my_arr.push_back(my_val);

std::cout << my_arr.serialize_pretty() << std::endl;
```
可能的输出：
```json
[
  true,
  [
    null,
    666
  ],
  { },
  "[ {} this is string ]"
]
```

#### 提醒:
不推荐的移动方式：
```c++
// 标准库容器不保证被移动后变回初始状态。（as_object返回JsonObject，本质是std::map。 JsonArray则是std::vector。）
B = std::move(A.as_object());  // ❌ 总是可行，但是不推荐这样写。
```

Recommended style:
```c++
// 本库的JsonValue类型，保证被移动后重置为ISNULL状态，可以正常使用。
B = std::move(A);          // ✅ A会被重置为初始状态。
B = std::move(A["xxx"]);   // ✅ 这样访问子元素，得到的类型是JsonValue&，所以也是安全的。
```

### 7. 自定义类型的序列化
自定义类型序列化主要有2种方式：
1. 编写类型转换运算符，提供到Json::Value类型的隐式转换
2. 提供.serialize()成员函数

上述两种方式，只需满足一种，就可以调用`Json::serialize(T t)`函数，将你的类型对象传入，进行序列化。

*（实际上，实现字符串类型的隐式转换，满足数组类型条件，满足键值对类型条件等，也可以调用上述函数，但较为麻烦。）*

参考代码：
```c++
struct A{
    std::string name;
    int value;
    bool check;
    // 编写类型转换运算符
    operator Json::Value() const {
        Json::Value result(Json::Type::object);
        result["name"] = name;
        result["value"] = value;
        result["check"] = check;
        return result;
    };
};

struct B{
    std::string name;
    int value;
    bool check;
    // 提供.serialize()成员函数
    std::string serialize(){
        return std::format("{{\"name\":{},\"value\":{},\"check\":{}}}", name, value, check);
    }
};
```

调用`Json::serialize(T t)`函数：
```c++
A a;
B b;
Json::serialzie(a); // 调用的是 Json::Serialize(const JsonValue& v)
Json::serialzie(b); // 调用的是 template<Serializable T> Json::Serialize(T t)
```

### 8. 异常处理
本库使用了三种自定义异常和一种标准：
1. `Json::Exception` : 继承自`std::runtime_error`，没有地方抛出此异常。
2. `Json::TypeException` : 继承自`Json::Exception`，表示类型错误，比如`as_xxx()`函数。
3. `Json::StructureException` : 继承自`Json::Exception`，表示JSON结构错误，导致反序列化失败。
4. `std::out_of_range` : 使用`at()`严格访问子元素，元素不存在或越界时抛出，

参考代码：
```c++
try{
    Json::Value json = Json::deserialize("[ {}} ]");
}
catch(const Json::StructureException& e){
    std::cerr << "JsonStructureException: " << e.what() << std::endl;
}
catch(const Json::Exception& e){
    std::cerr << "JsonException: " << e.what() << std::endl;
}
catch(...){ std::cerr << "other" << std::endl; }
```
可能的输出：
```
JsonStructureException: Unknown Json Structure.
```

#### 注意
赋值/拷贝/移动/序列化/`is`/`type`/`size`...等操作保证不会抛出异常。

只有`deserialze()`反序列化函数，或者`as`类型转换失败，访问越界时可能抛出异常。


## 性能概述

具体性能请参考：<https://github.com/Mysvac/cpp-json-test> <br>
本库是在`mysvac-json`的基础上调整至C++23的，大部分实现完全一致，效率并无差别。<br>
*别说什么抄袭，我就是mysvac，那个库也是我写的*


*时间复杂度其实没什么用，看看就好，后面都是常数优化。*
- **N** : JSON文本长度。
- **m** : 子元素个数。
- 下面提供最坏情况的时空复杂度（虽然没什么用，后面都是常数优化。）：
- **序列化**：时间复杂度O(N)，空间复杂度O(N)。
- **反序列化**: 时间复杂度O(N)，空间复杂度O(N)。
- **键值对-增删改查**: O(log m)。
- **数组-增删**: 末尾操作O(1)，其余位置平均O(m)。
- **数组-改查**: O(1)。




