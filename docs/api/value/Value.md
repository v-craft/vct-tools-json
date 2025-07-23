# **json::Value**

```cpp
class Value;
```

`json::Value` is a universal container for JSON datas. It has only a member `m_data` , which is a `std::variant` that can hold different types of JSON data.

```cpp
private:
    std::variant<
        Null,  
        Bool,  
        Number,
        String,
        Array, 
        Object 
    >  m_data { Null{} };
```

> Variable contains a type index internally.

This class provides a series of functions for processing JSON data.


## Member functions

- **[`Constructor`](./constructor.md)**
- **[`Destructor`](./destructor.md)**
- **[`operator=`](operator_assign.md)**

### Object inspection

- **[`type`](./type.md)** \- return the type of the JSON value
- **[`type_name`](./type_name.md)** \- return the type name of the JSON value
- **[`reset`](./reset.md)** \- reset the JSON value.
- **[`is_nul`](./is_nul.md)** \- check if the JSON value is null
- **[`is_bol`](./is_bol.md)** \- check if the JSON value is a boolean
- **[`is_num`](./is_num.md)** \- check if the JSON value is a number
- **[`is_str`](./is_str.md)** \- check if the JSON value is a string
- **[`is_arr`](./is_arr.md)** \- check if the JSON value is an array
- **[`is_obj`](./is_obj.md)** \- check if the JSON value is an object

### Value access
- **[`nul`](./get_nul.md)** \- get the reference of inner `Null` value
- **[`bol`](./get_bol.md)** \- get the reference of inner `Bool` value
- **[`num`](./get_num.md)** \- get the reference of inner `Number` value
- **[`str`](./get_str.md)** \- get the reference of inner `String` value
- **[`arr`](./get_arr.md)** \- get the reference of inner `Array` value
- **[`obj`](./get_obj.md)** \- get the reference of inner `Object` value
- **[`operator[]`](./operator_bracket.md)** \- access the value by key or index
- **[`at`](./at.md)** \- access the value by key or index with bounds checking
- **[`to`](./to.md)** \- convert the JSON value to a specific type
- **[`to_if`](./to_if.md)** \- convert the JSON value to a specific type if it could be converted
- **[`to_or`](./to_or.md)** \- convert the JSON value to a specific type or return a default value
- **[`move`](./move.md)** \- move the JSON value to another `Value` object
- **[`move_if`](./move_if.md)** \- move the JSON value to another `Value` object if it could be converted
- **[`move_or`](./move_or.md)** \- move the JSON value to another `Value` object or return a default value

### Serialization

- **[`dump`](./dump.md)** \- dump the JSON value to a string
- **[`write`](./write.md)** \- write the JSON value to an output stream
- **[`dumpf`](./dumpf.md)** \- dump the JSON value with formatting to a string
- **[`writef`](./writef.md)** \- write the JSON value with formatting to an output stream

### Comparison

- **[`operator==`](./operator_eq.md)** \- compare two `Value` objects for equality

