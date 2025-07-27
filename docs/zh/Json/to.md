# **Json.to**

```cpp
template<typename T, typename D = Null>
requires convertible<Json, T> || convertible_map<Json, T, D> || convertible_array<Json, T, D>
T  to( D default_range_elem = D{} ) const;
```

将内部数据强制类型转换（拷贝）为 `T` 类型，如果转换失败则抛出异常。

内部进行了充分的编译期类型检查，不可转换的分支会在编译期被排除。
但即使模板可实例化，最后仍可能转换失败，因为 JSON 内部数据类型是运行时的。

## 转换规则

1. Null -> Null
2. Object -> Object
3. Array -> Array
4. String -> String
5. Bool -> Bool
6. Number -> 枚举类型 (四舍五入至最近的整数)
7. Number -> 整数类型 (四舍五入至最近的整数)
8. Number -> 浮点类型
9. Any -> 提供了 Json 构造函数的类型
10. Object -> 可隐式转换的类型
11. Array -> 可隐式转换的类型
12. String -> 可隐式转换的类型
13. Number -> 可隐式转换的类型
14. Bool -> 可隐式转换的类型
15. Null -> 可隐式转换的类型 (Null 不能隐式转换为 bool ！)
16. Object -> 尝试复制到类型的可转换键值对类型（只需要内部元素可转换）
17. Array -> 尝试复制到类型的可转换数组类型（只需要内部元素可转换）
18. throw std::runtime_error

## 参数

- `T` 需要转换成的目标类型
- `default_range_elem` 转成成数组或键值对类型时，子元素转换失败时的填充值（转换目标不是数组和对象时无需填写）

## 异常

转换失败抛出异常。

## 复杂度

线性，内容全拷贝。

## 版本

v0.8.0 至今。
