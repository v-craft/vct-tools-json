# **Json.nul**

```cpp
constexpr Null& nul() & ;
constexpr Null&& nul() && ;
constexpr const Null& nul() const & ;
constexpr const Null&& nul() const && ;
```

获取内部 `Null` 值的引用。

## 返回值

根据类对象本身，返回 `Null` 的 `const` 或非 `const` 、左值或右值引用。

## 异常

内部数据不是 `Null` 时，抛出 `std::bad_variant_access` 异常。

## 复杂度

常数。

## 版本

v0.8.0 至今。
