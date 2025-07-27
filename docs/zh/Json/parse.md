# **Json.parse**

```cpp
static std::expected<Json, ParseError> parse(const std::string_view text, const std::int32_t max_depth = 256);

static std::expected<Json, ParseError> parse(std::istream& is_text, const std::int32_t max_depth = 256);
```

静态成员函数，将目标字符串或输入流中的 JSON 文本解析为 `Json` 对象。

## 参数

- `text`: 一个 `std::string_view` 类型的字符串视图，包含要解析的 JSON 文本。

- `is_text`: 一个输入流（`std::istream`）对象，包含要解析的 JSON 文本。

- `max_depth`: 限制解析内容的最大嵌套深度。此参数用于防止过深的嵌套导致栈溢出或性能问题。解析器会在解析过程中跟踪当前的嵌套深度，并与此参数进行比较。
  - 如果嵌套深度超过 `max_depth`，则返回 `ParseError::eDepthExceeded` 错误。
  - 默认值为 256。


## 返回值

返回一个 `std::expected<Json, ParseError>` 对象：

- 解析成功时内涵 `Json` 对象，是解析后的 JSON 数据。
- 解析失败时内涵 `ParseError` 枚举值，指示解析错误的类型。

## 异常

函数本身没有任何异常，但是如果在解析中出现内存不足等问题抛出异常，不会被本函数捕获，需要你自行处理（此时函数也不会正常返回内容）。

## 注意

JSON 文本解析失败是非常常见且不可预期的，因为在实际应用中，JSON 格式很容易出现错误或者出现一些垃圾数据。
因此本库默认使用 `std::expected<>` 的方式返回序列化结果，避免进行异常处理。

函数还设置了 `max_depth` 参数指定解析的最大嵌套深度（默认为 256），避免处理某些垃圾数据（如过长的 `[[[[[[]]]]]]`）时可能导致的栈溢出问题。
（虽然本库保证总的解析复杂度为 `O(n)`，即严格单次遍历，但由于使用递归处理嵌套结构，因此需要此参数来避免栈溢出问题。）

虽然使用了 `std::expected<>` 包裹错误，但它仅代表JSON格式错误的问题，如果在解析时扩容内部数据、键值对或字符串时出现内存不足等问题依然会抛出异常，这些严重问题需要你显式处理。
（这些问题一般不会发生，就像你使用 `vector.push_back()` 时通常不会处理异常，但它确实可能抛出。）

## 复杂度

线性，仅取决于输入文本的长度（与嵌套层数无关）。

## 版本

v0.8.0 至今。
