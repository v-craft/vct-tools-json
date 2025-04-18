//
// Created by mys_vac on 25-4-18.
//

export module json;
import std;

#ifndef VCT_TOOL_PORT_SIGNAL
    #define VCT_TOOL_PORT_SIGNAL
#endif

#define WIN_PORT_SIGNAL VCT_TOOL_PORT_SIGNAL

/**
 * @namespace Json
 * @brief 存放Json解析器相关内容
 */
export namespace Json {

    /**
     * @enum Type
     * @brief 表示Json对象内部数据类型
     */
    enum class WIN_PORT_SIGNAL Type{
        object, /**< JSON 对象类型 */
        array,/**< JSON 数组类型 */
        string, /**< JSON 字符串类型，值类型 */
        number, /**< JSON 数值类型，值类型 */
        boolean, /**< JSON 布尔类型，值类型 */
        null, /**< JSON null类型，值类型 */
    };

    /**
     * @class Exception
     * @brief Json专用异常基类类，继承自std::runtime_error。
     * @note 本异常不会直接抛出。
     */
    class WIN_PORT_SIGNAL Exception : public std::runtime_error {
    public:
        Exception() = delete;
        explicit Exception(const std::string& message) : std::runtime_error(message) {}
    };

    /**
     * @class TypeException
     * @brief Json类型错误异常类，继承自Json:Exception
     * @details as类型转换，或at,[],insert等操作发现类型不正确时抛出
     */
    class WIN_PORT_SIGNAL TypeException final : public Exception {
    public:
        /**
         * @brief 默认构造函数。
         */
        TypeException() : Exception("Unknown JSON Type Exception.\n") {}
        /**
         * @brief 带异常描述文本的构造函数。
         */
        explicit TypeException(const std::string& message) : Exception(message) {}
    };

    /**
     * @class StructureException
     * @brief Json结构错误异常类，继承自Json:Exception
     * @details 只在调用deserialize()函数时可能抛出
     */
    class WIN_PORT_SIGNAL StructureException final : public Exception {
    public:
        /**
         * @brief 默认构造函数。
         */
        StructureException() : Exception("Unknown Json Structure.\n") {}
        /**
         * @brief 带异常描述文本的构造函数。
         */
        explicit StructureException(const std::string& message) : Exception(message) {}
    };

    // 前向声明
    class Value;

    /**
     * @brief Object类型，本质是 std::map<std::string,Value>
     */
    using Object = std::map<std::string,Value>;

    /**
     * @brief Array类型，本质是 std::vector<Value>
     */
    using Array = std::vector<Value>;

    /**
     * @class Value
     * @brief 纯储Json数据的通用类类型。
     * @details
     * JSON反序列化后得到的可操作对象。
     * 内部可以纯储JSON标准的6种类型，包括对象和数组。
     * 本库任何子对象访问，获取到的都是此类型。
     */
    class WIN_PORT_SIGNAL Value{
    private:
        /**
         * @brief 内部数据块类型，仅内部使用
         */
        using JsonVariant = std::variant<bool, std::string, Object, Array>;

        /**
         * @brief 存储JSON数据。
         * @details std::variant类型，存储JSON数据，需要C++17。
         * @note 注意，Map和List存储的也是JsonObject对象，含有嵌套内容。
         */
        JsonVariant content_ { false };

        /**
         * @brief 表示当前对象存储的数据类型，
         */
        Type type_ { Type::null };

    public:
        /**
         * @brief 默认构造函数
         * @details 默认构造，生成null类型的JSON数据。
         */
        Value() = default;

        /**
         * @brief 指定类型的构造函数
         */
        explicit Value(const Type& jsonType) noexcept;

        /**
         * @brief 获取当前对象数据类型
         * @return Json::JsonType类型，表示当前对象数据类型
         */
        [[nodiscard]]
        Type type() const noexcept { return type_; }

        /**
         * @brief 重置，设为null值，不会调用其他函数
         */
        void reset() noexcept {
            type_ = Type::null;
            content_ = false;
        }

        /**
         * @brief 按类型清除，不会调用其他函数
         * @details
         * 对象变为空字典，
         * 数组变空数组，
         * 数值变0，
         * 布尔变false，
         * 字符串变空串，
         * null不变。
         */
        void clear() noexcept;

        /**
         * @brief 获取子元素数量
         * @details
         * 对象和列表返回子元素个数，
         * 字符串返回长度，
         * 其他类型返回1。
         */
        [[nodiscard]]
        size_t size() const noexcept;

        /**
         * @brief 反序列化构造函数
         * @param str 需要反序列化的字符串
         * @param it 起始位置
         * @exception StructureException 结构错误解析失败时抛出。
         * @details
         * 从str的it位置开始反序列化。
         * 最多反序列化一个有效对象，完成后不解析后续文本。
         * 不推荐直接使用，建议使用deserialize()函数。
         */
        Value(std::string_view str, std::string_view::const_iterator& it);

        /**
         * @brief 列表初始化器
         * @param init_list 初始化列表
         * @details
         * 参数数量为0时，被认为是null类型
         * 参数数量为1时，等同于普通构造函数
         * 参数数量为2且第一个参数是字符串类型时，被认为是object
         * 其他时候都会被认为是array
         * @note 注意区分{}初始化器和()初始化器，二者效果可能不同
         */
        Value(const std::initializer_list<Value>& init_list);

        /**
         * @brief 拷贝构造
         */
        Value(const Value& jsonValue) noexcept;

        /**
         * @brief 移动构造
         */
        Value(Value&& jsonValue) noexcept;

        /**
         * @brief 拷贝赋值
         */
        Value& operator=(const Value& jsonValue) noexcept;

        /**
         * @brief 移动赋值
         */
        Value& operator=(Value&& jsonValue) noexcept;

        /**
         * @brief 序列化对象
         * @details
         * 将对象序列化，生成JSON结构的字符串。
         * 不含无效空白字符，速度较快。
         * @return std::string类型，序列化后的文本。
         */
        [[nodiscard]]
        std::string serialize() const noexcept;

        /**
         * @brief 序列化对象且美化
         * @details
         * 对象序列化，生成JSON结构的字符串。
         * 自动生成换行和空格缩进，速度略慢。
         * 可以指定缩进长度，以及初始缩进此时。
         * @param space_num 每次缩进使用的空格数量 默认 2
         * @param depth 当前对象缩进次数，默认 0
         * @return std::string类型，序列化后的文本。
         */
        [[nodiscard]]
        std::string serialize_pretty(const size_t& space_num = 2,const size_t& depth = 0) const noexcept;

        /**
         * @brief 字符串构造函数
         * @param str 需要转义的字符串
         * @details
         * 将输入字符串反转义，生成对象。
         * 注意，不会视为JSON结构解析，而是直接当做JSON字符串类型。
         */
        Value(std::string_view str) noexcept;

        /**
         * @brief 字符串赋值函数
         * @param str JSON格式的文本数据
         * @details
         * 将输入字符串反转义，生成对象。
         * 注意，不会视为JSON结构解析，而是直接当做JSON字符串类型。
         */
        Value& operator=(std::string_view str) noexcept;

        /**
         * @brief 字符串构造函数
         * @param str 需要转义的字符串
         * @details
         * 将输入字符串反转义，生成对象。
         * 注意，不会视为JSON结构解析，而是直接当做JSON字符串类型。
         */
        Value(const std::string& str) noexcept;

        /**
         * @brief 字符串赋值函数
         * @param str JSON格式的文本数据
         * @details
         * 将输入字符串反转义，生成对象。
         * 注意，不会视为JSON结构解析，而是直接当做JSON字符串类型。
         */
        Value& operator=(const std::string& str) noexcept;

        /**
         * @brief 字符串字面量构造函数
         * @param str JSON格式的文本数据。
         * @details
         * 将输入字符串反转义，生成对象。
         * 注意，不会视为JSON结构解析，而是直接当做JSON字符串类型。
         */
        Value(const char* str) noexcept;

        /**
         * @brief 字符串字面量赋值函数
         * @param str JSON格式的文本数据。
         * @details
         * 将输入字符串反转义，生成对象。
         * 注意，不会视为JSON结构解析，而是直接当做JSON字符串类型。
         */
        Value& operator=(const char* str) noexcept;


        /**
         * @brief 布尔类型构造
         */
        Value(const bool& bl) noexcept: content_(bl), type_(Type::boolean) { }
        /**
         * @brief 整数类型构造
         */
        Value(const int& num) noexcept: content_(std::to_string(num)),type_(Type::number) { }
        /**
         * @brief 长整数类型构造
         */
        Value(const long long& num) noexcept: content_(std::to_string(num)), type_(Type::number) { }
        /**
         * @brief 浮点数类型构造
         */
        Value(const double& num) noexcept: content_(std::to_string(num)),type_(Type::number) { }
        /**
         * @brief 浮点数类型构造
         */
        Value(const long double& num) noexcept: content_(std::to_string(num)),type_(Type::number) { }
        /**
         * @brief nullptr_t类型构造
         */
        Value(const std::nullptr_t&) noexcept{ }
        /**
         * @brief 布尔类型赋值
         */
        Value& operator=(const bool& bl) noexcept {
            type_ = Type::boolean;
            content_ = bl;
            return *this;
        }
        /**
         * @brief 整数类型赋值
         */
        Value& operator=(const int& num) noexcept {
            type_ = Type::number;
            content_ = std::to_string(num);
            return *this;
        }
        /**
         * @brief 长整数类型赋值
         */
        Value& operator=(const long long& num) noexcept{
            type_ = Type::number;
            content_ = std::to_string(num);
            return *this;
        }
        /**
         * @brief 浮点数类型赋值
         */
        Value& operator=(const double& num) noexcept {
            type_ = Type::number;
            content_ = std::to_string(num);
            return *this;
        }
        /**
         * @brief 浮点数类型赋值
         */
        Value& operator=(const long double& num) noexcept{
            type_ = Type::number;
            content_ = std::to_string(num);
            return *this;
        }
        /**
         * @brief null类型赋值
         */
        Value& operator=(const std::nullptr_t& ) noexcept{
            type_ = Type::null;
            content_ = false;
            return *this;
        }


        /**
         * @brief Json::Array拷贝构造
         */
        Value(const Array& jsonArray) noexcept;
        /**
         * @brief Json::Array移动构造
         */
        Value(Array&& jsonArray) noexcept;
        /**
         * @brief Json::Object拷贝构造
         */
        Value(const Object& jsonObject) noexcept;
        /**
         * @brief Json::Object移动构造
         */
        Value(Object&& jsonObject) noexcept;
        /**
         * @brief Json::Array拷贝赋值
         */
        Value& operator=(const Array& jsonArray) noexcept;
        /**
         * @brief Json::Array移动赋值
         */
        Value& operator=(Array&& jsonArray) noexcept;
        /**
         * @brief Json::Object拷贝赋值
         */
        Value& operator=(const Object& jsonObject) noexcept;
        /**
         * @brief Json::Object移动赋值
         */
        Value& operator=(Object&& jsonObject) noexcept;


        /**
         * @brief 判断是不是对象
         */
        [[nodiscard]]
        bool is_object() const noexcept { return type_ == Type::object; }
        /**
         * @brief 判断是不是数组
         */
        [[nodiscard]]
        bool is_array() const noexcept { return type_ == Type::array; }
        /**
         * @brief 判断是不是字符串
         */
        [[nodiscard]]
        bool is_string() const noexcept { return type_ == Type::string; }
        /**
         * @brief 判断是不是布尔
         */
        [[nodiscard]]
        bool is_bool() const noexcept { return type_ == Type::boolean; }
        /**
         * @brief 判断是不是数值
         */
        [[nodiscard]]
        bool is_number() const noexcept { return type_ == Type::number; }
        /**
         * @brief 判断是不是整数
         */
        [[nodiscard]]
        bool is_int64() const noexcept { return type_ == Type::number && std::get<std::string>(content_).find('.') == std::string::npos; }
        /**
         * @brief 判断是不是浮点
         */
        [[nodiscard]]
        bool is_double() const noexcept { return type_ == Type::number && std::get<std::string>(content_).find('.') != std::string::npos; }
        /**
         * @brief 判断是不是null
         */
        [[nodiscard]]
        bool is_null() const noexcept { return type_ == Type::null; }
        /**
         * @brief 判断是不是值类型
         */
        [[nodiscard]]
        bool is_value() const noexcept { return type_ != Type::object && type_ != Type::array; }


        /**
         * @brief 转换成long long类型，复制一份，必须是number
         * @exception TypeException 转换失败，类型错误
         */
        [[nodiscard]]
        long long as_int64() const;
        /**
         * @brief 转换成double类型，复制一份，必须是number
         * @exception TypeException 转换失败，类型错误
         */
        [[nodiscard]]
        double as_double() const;
        /**
         * @brief 转换成double类型，复制一份，必须是number
         * @exception TypeException 转换失败，类型错误
         */
        [[nodiscard]]
        long double as_ldouble() const;
        /**
         * @brief 转换成bool类型，复制一份，必须是bool
         * @exception TypeException 转换失败，类型错误
         */
        [[nodiscard]]
        bool as_bool() const;
        /**
         * @brief 转换成string类型，注意是拷贝一份，不会抛出异常
         * @details
         * 字符串类型，转义后输出
         * 其他类型，等同于调用serialize()成员函数
         */
        [[nodiscard]]
        std::string as_string() const noexcept;
        /**
         * @brief 获取内部Object对象的引用，类型必须是OBJECT
         * @details 获取内部Object对象的引用，可调用std::map相关函数，非const，可修改内容
         * @exception TypeException 转换失败，类型错误
         */
        Object& as_object();
        /**
         * @brief 获取内部JsonObject对象的const引用，类型必须是OBJECT
         * @details 获取内部Object对象的const引用，可调用std::map相关函数，const，不可修改内容
         * @exception TypeException 转换失败，类型错误
         */
        [[nodiscard]]
        const Object& as_object() const;
        /**
         * @brief 获取内部Array对象的引用，类型必须是ARRAY
         * @details 获取内部Array对象的引用，可调用std::vector相关函数，非const，可修改内容
         * @exception TypeException 转换失败，类型错误
         */
        Array& as_array();
        /**
         * @brief 获取内部JsonArray对象的const引用，类型必须是ARRAY
         * @details 获取内部Array对象的引用，可调用std::vector相关函数，const，不可修改内容
         * @exception TypeException 转换失败，类型错误
         */
        [[nodiscard]]
        const Array& as_array() const;


        /**
         * @brief at元素访问数组
         * @details
         * at访问，带越界检查，
         * 不会创建新元素，
         * 越界抛出out_of_range，必须是ARRAY类型。
         * @exception TypeException 类型错误异常
         * @exception std::out_of_range 越界不存在
         */
        Value& at(const size_t& index);
        /**
         * @brief at元素访问对象
         * @details
         * at访问，带越界检查，
         * 不会创建新元素，
         * key不存在时抛出out_of_range，必须是OBJECT类型。
         * @exception TypeException 类型错误异常
         * @exception std::out_of_range key不存在
         */
        Value& at(const std::string& key);
        /**
         * @brief []元素访问数组
         * @details
         * []访问，必须是ARRAY类型，无越界检查。
         * 如果指向末尾，会创建新元素。
         * @exception TypeException 类型错误异常
         */
        Value& operator[](const size_t& index);
        /**
         * @brief []元素访问对象
         * @details
         * []访问，必须是OBJECT类型，无越界检查，
         * 如果key不存在，会创建新元素，所以可以直接赋值。
         * @exception TypeException 类型错误异常
         */
        Value& operator[](const std::string& key);
        /**
         * @brief 检查是否存在某个key
         * @details
         * 检查是否存在某个key，
         * 不会抛出异常，
         * 非object类型直接返回false。
         */
        [[nodiscard]]
        bool contains(const std::string& key) const noexcept;


        /**
         * @brief 数组末尾拷贝插入元素，O(1)，必须是ARRAY类型
         * @exception TypeException 非数组类型抛出异常
         */
        void push_back(const Value& jsonValue);

        /**
         * @brief 数组末尾移动插入元素，O(1)，必须是ARRAY类型
         * @exception TypeException 非数组类型抛出异常
         */
        void push_back(Value&& jsonValue);

        /**
         * @brief 末尾删除元素，O(1)，必须是ARRAY类型
         * @exception TypeException 非数组类型抛出异常
         */
        void pop_back();

        /**
         * @brief 在指定位置插入元素，O(m)，必须是ARRAY类型
         */
        void insert(const size_t& index, const Value& jsonValue);

        /**
         * @brief 在指定位置移入元素，O(m)，必须是ARRAY类型
         */
        void insert(const size_t& index, Value&& jsonValue);

        /**
         * @brief 插入键值对，O(log m)，必须是Object类型
         */
        void insert(const std::string& key, const Value& jsonValue);

        /**
         * @brief 移动插入键值对，O(log m)，必须是Object类型
         */
        void insert(const std::string& key, Value&& jsonValue);

        /**
         * @brief 删除指定位置的元素
         * @details
         * 删除指定位置的元素，
         * 当前对象必须是ARRAY类型，
         * 不能越界
         * @exception TypeException 当前对象并非ARRAY类型。
         * @exception std::out_of_range 索引越界。
         * @note 你也可以使用 as_array().erase( x )，调用std::vector的erase函数
         */
        void erase(const size_t& index);
        /**
         * @brief 删除key的元素
         * @details
         * 删除指定key的元素，
         * 当前对象必须是OBJECT类型，
         * key可以不存在(不会执行任何操作，不会抛出异常）。
         * @exception TypeException 当前对象并非OBJECT类型。
         * @note 你也可以使用 as_object().erase( x )，调用std::map的erase函数
         */
        void erase(const std::string& key);

    };


    /**
     * @brief 反序列化函数
     * @param str 需要反转义的原JSON字符串
     * @note 解析JSON数据统一使用此函数
     */
    WIN_PORT_SIGNAL
    Value deserialize(std::string_view str);

    /**
     *@brief 值类型序列化函数，普通函数重载
     */
    WIN_PORT_SIGNAL
    std::string serialize(const Value &jsonValue);
    /**
     * @brief 布尔类型序列化函数
     */
    WIN_PORT_SIGNAL
    std::string serialize(const bool& bl) noexcept;
    /**
     * @brief nullptr序列化函数
     */
    WIN_PORT_SIGNAL
    std::string serialize(std::nullptr_t) noexcept;
    /**
     *@brief 数组类型序列化函数，普通函数重载
     */
    WIN_PORT_SIGNAL
    std::string serialize(const Array& jsonArray) noexcept;
    /**
     *@brief 对象类型序列化函数，普通函数重载
     */
    WIN_PORT_SIGNAL
    std::string serialize(const Object& jsonObject) noexcept;
    /**
     *@brief 字符串序列化函数，普通函数重载
     */
    WIN_PORT_SIGNAL
    std::string serialize(const std::string& str) noexcept;
    /**
     *@brief 字符串序列化函数，普通函数重载
     */
    WIN_PORT_SIGNAL
    std::string serialize(const char* str) noexcept;
    /**
     *@brief 字符串序列化函数，普通函数重载
     */
    WIN_PORT_SIGNAL
    std::string serialize(std::string_view str) noexcept;


    /**
     * @brief 数值类型的概念
     */
    template <typename T>
    concept Arithmetic = std::integral<T> || std::floating_point<T>;

    /**
     * @brief 数值类型的序列化
     */
    template <Arithmetic T>
    std::string serialize(T value) noexcept {
        return std::to_string(value);
    }

    /**
     * @brief 非数值类型的序列化（要求具有 .serialize() 成员函数）
     */
    template <typename T>
    requires requires (const T& t) { { t.serialize() } -> std::convertible_to<std::string>; }
    std::string serialize(const T& value) {
        return value.serialize();
    }

}

module :private;


namespace Json {
    ////////////////////////////////////////////////////////////////////////////////////
    ////// 模块私有函数，静态函数

    /**
     * @brief 模块私有静态函数，用于转义unicode字符
     * @exception StructureException 不合法的unicode字符
     */
    static void escape_unicode(std::string& res,std::string_view str, std::string_view::const_iterator& it) {
        // 进入时 it 在 \uXXXX 的 u 的位置。
        if (str.end() - it <= 4) throw StructureException{ "Illegal unicode.\n" };
        ++it;
        std::istringstream iss( std::string(str.substr(it-str.begin(), 4)));
        // 函数返回时，it应该在\uABCD 的 D位置，所以只能 +3
        it += 3;
        unsigned int codePoint;
        iss >> std::hex >> codePoint;

        if (iss.fail() || codePoint > 0xFFFF) {
            // 错误的\u转义字符，会直接跳过，不会报错。
            throw StructureException{ "Illegal unicode.\n" };
        }

        // [0xD800 , 0xE000) 范围，是代理对，是连续2波\u转码
        if (codePoint >= 0xD800 && codePoint <= 0xDFFF) {
            // 代理队，必须是 高代理 + 低代理
            // 高代理 [\uD800, \uDBFF]
            // 低代理 [\uDC00, \uDFFF]
            if (codePoint >= 0xDC00) {
                // 低代理开头，直接结束
                throw StructureException{ "Illegal unicode - start with low-code.\n" };
            }

            // 检查下一个转义序列是否是低代理
            if (str.end() - it < 7 || *(it+1) != '\\' || *(it+2) != 'u') {
                // 当前是高代理，但是下个位置不是低代理，也直接返回
                throw StructureException{ "Illegal unicode - only high-code.\n" };
            }

            // 解析低代理 +3 进入 \uABCD 的 A位置
            it += 3;
            std::istringstream lowIss( std::string(str.substr(it-str.begin(), 4) ));
            it += 3; // 移动到 \uABCD的D位置

            unsigned int lowCodePoint;
            lowIss >> std::hex >> lowCodePoint;

            if (lowIss.fail() || lowCodePoint < 0xDC00 || lowCodePoint > 0xDFFF) {
                // 不是低代理对，说明错误
                throw StructureException{ "Illegal unicode - not end with lowcode.\n" };
            }

            // 将代理对组合为单个码点
            codePoint = 0x10000 + ((codePoint - 0xD800) << 10) + (lowCodePoint - 0xDC00);
        }

        // 将码点编码为 UTF-8
        if (codePoint <= 0x7F) {
            res += static_cast<char>(codePoint);
        }
        else if (codePoint <= 0x7FF) {
            res += static_cast<char>(0xC0 | (codePoint >> 6));
            res += static_cast<char>(0x80 | (codePoint & 0x3F));
        }
        else if (codePoint <= 0xFFFF) {
            res += static_cast<char>(0xE0 | (codePoint >> 12));
            res += static_cast<char>(0x80 | ((codePoint >> 6) & 0x3F));
            res += static_cast<char>(0x80 | (codePoint & 0x3F));
        }
        else if (codePoint <= 0x10FFFF) {
            res += static_cast<char>(0xF0 | (codePoint >> 18));
            res += static_cast<char>(0x80 | ((codePoint >> 12) & 0x3F));
            res += static_cast<char>(0x80 | ((codePoint >> 6) & 0x3F));
            res += static_cast<char>(0x80 | (codePoint & 0x3F));
        }
        else throw StructureException{ "Illegal unicode.\n" };;
    }

    /**
     * @brief 内部函数，转义字符串且移动指针
     * @exception StructureException 各种内容错误导致转义失败
     */
    static std::string escape_next(std::string_view str, std::string_view::const_iterator& it) {
        // 跳过字符串起始的双引号
        ++it;
        std::string res;

        while (it != str.end() && *it != '\"') {
            switch (*it) {
                case '\\':
                {
                    // 转义字符处理
                    ++it;
                    if (it == str.end()) throw StructureException{ "String have not end char '\"'." };
                    switch (*it) {
                        case '\"':
                            res += '\"';
                            break;
                        case '\\':
                            res += '\\';
                            break;
                        case '/':
                            res += '/';
                            break;
                        case 'n':
                            res += '\n';
                            break;
                        case 'r':
                            res += '\r';
                            break;
                        case 't':
                            res += '\t';
                            break;
                        case 'f':
                            res += '\f';
                            break;
                        case 'b':
                            res += '\b';
                            break;
                        case 'u':
                        case 'U':
                            escape_unicode(res, str, it);
                            break;
                        default:
                            throw StructureException{ "Illegal escape characters.\n " };
                    }
                }
                break;
                case '\t':
                case '\n':
                case '\f':
                case '\b':
                case '\r':
                    throw StructureException{ "There are characters that have not been escaped.\n" };
                default:
                    res += *it;
                break;
            }
            ++it;
        }
        if(it == str.end()) throw StructureException {"Unclosed string.\n"};
        ++it;
        return res;
    }

    /**
     * @brief 内部函数，转义字符串
     * @exception StructureException 各种内容错误导致转义失败
     */
    static std::string escape(std::string_view str) {
        auto it = str.begin();
        ++it;
        std::string res;

        while (it != str.end() && *it != '\"') {
            switch (*it) {
                case '\\':
                {
                    // 转义字符处理
                    ++it;
                    if (it == str.end()) throw StructureException{ "String have not end char '\"'." };
                    switch (*it){
                        case '\"':
                            res += '\"';
                            break;
                        case '\\':
                            res += '\\';
                            break;
                        case '/':
                            res += '/';
                            break;
                        case 'n':
                            res += '\n';
                            break;
                        case 'r':
                            res += '\r';
                            break;
                        case 't':
                            res += '\t';
                            break;
                        case 'f':
                            res += '\f';
                            break;
                        case 'b':
                            res += '\b';
                            break;
                        case 'u':
                        case 'U':
                            escape_unicode(res, str, it);
                            break;
                        default:
                            break;
                    }
                }
                break;
                default:
                    res += *it;
                break;
            }
            ++it;
        }
        return res;
    }

    /**
     * @brief 内部函数，反转义字符串
     * @note 参数const是因为CLion提示才加的，其实不用加
     */
    static std::string reverse_escape(const std::string_view str) noexcept {
        std::string res;
        // 提前分配空间，减少扩容开销
        if (str.size() > 15) res.reserve(str.size() + (str.size() >> 4));
        res += "\"";
        for (const char& it : str) {
            switch (it) {
                case '\"':
                    res += "\\\"";
                break;
                case '\\':
                    res += "\\\\";
                case '\n':
                    res += "\\n";
                break;
                case '\f':
                    res += "\\f";
                break;
                case '\t':
                    res += "\\t";
                break;
                case '\r':
                    res += "\\r";
                break;
                case '\b':
                    res += "\\b";
                break;
                default:
                    res += it;
                break;
            }
        }
        res += "\"";
        return res;
    }

    ////////////////////////////////////////////////////////////////////////////////////
    ////// 库函数实现

    /////////////////////////////////////////////
    /// 序列化函数 实现
    Value deserialize(std::string_view str){
        auto it = str.begin();
        while(it!=str.end() && std::isspace(*it)) ++it;

        // 禁止空内容
        if(it == str.end()) throw StructureException{ "Empty JSON data.\n" };

        Value jsonValue (str, it);

        // 解析完成，后续不能有多余空内容
        while( it != str.end() ){
            if(!std::isspace(*it)) throw StructureException {"Unknown content at the end.\n"};
            ++it;
        }
        return jsonValue;
    }

    /////////////////////////////////////////////
    /// 序列化函数 实现
    std::string serialize(const Value &jsonValue) {
        return jsonValue.serialize();
    }
    std::string serialize(const bool& bl) noexcept {
        return bl ? "true" : "false";
    }
    std::string serialize(std::nullptr_t) noexcept {
        return "null";
    }
    std::string serialize(const Array& jsonArray) noexcept {
        std::string res{ "[" };
        for (const Value& it : jsonArray) {
            // 递归序列号
            res += it.serialize();
            res += ',';
        }
        if (*res.rbegin() == ',') *res.rbegin() = ']';
        else res += ']';
        return res;
    }
    std::string serialize(const Object& jsonObject) noexcept {
        std::string res{ "{" };
        for (const auto& [fst, snd] : jsonObject) {
            // 键是字符串，需要反转义
            res += reverse_escape(fst);
            res += ':';
            // 递归序列号
            res += snd.serialize();
            res += ',';
        }
        if (*res.rbegin() == ',') *res.rbegin() = '}';
        else res += '}';
        return res;
    }
    std::string serialize(const std::string& str) noexcept {
        return reverse_escape(str);
    }
    std::string serialize(const char* str) noexcept {
        return reverse_escape(str);
    }
    std::string serialize(std::string_view str) noexcept {
        return reverse_escape(str);
    }

    ////////////////////////////////////////////////////////////////////////////////////
    ////// 关键类 Value 的成员函数实现

    /////////////////////////////////////////////
    /// 基础函数
    // 指定类型的构造函数
    Value::Value(const Type& jsonType) noexcept{
        type_ = jsonType;
        switch (jsonType){
            case Type::object:
                content_ = Object{};
                break;
            case Type::array:
                content_ = Array{};
                break;
            case Type::string:
                content_ = std::string{};
                break;
            case Type::number:
                content_ = std::string{ "0" };
                break;
            default:
                // 其他类型不需要处理，默认false即可
                break;
        }
    }

    // 清空内容
    void Value::clear() noexcept {
        switch (type_)
        {
            case Type::object:
                content_ = Object{};
            break;
            case Type::array:
                content_ = Array{};
            break;
            case Type::string:
                content_ = std::string{};
            break;
            case Type::number:
                content_ = std::string{ "0" };
            break;
            case Type::boolean:
                content_ = false;
            break;
            default:
                break;
        }
    }

    // 长度获取
    size_t Value::size() const noexcept{
        switch (type_)
        {
            case Type::object:
                return std::get<Object>(content_).size();
            case Type::array:
                return std::get<Array>(content_).size();
            case Type::string:
                return std::get<std::string>(content_).size();
            default:
                return 1;
        }
    }

    // 反序列化构造
    Value::Value(std::string_view str, std::string_view::const_iterator& it){
        // 去除开头空格
        while(it != str.end() && std::isspace(*it)) ++it;
        // 静止空内容
        if(it == str.end()) throw StructureException{ "Empty JSON data.\n" };
        switch (*it){
            case '{':
            {
                // 对象类型，键值对处理
                type_ = Type::object;
                content_ = Object {};
                auto& jsonObject = std::get<Object>(content_);
                std::string key;
                ++it;
                while(it != str.end()){
                    while (it!=str.end() && std::isspace(*it)) ++it;
                    if(it == str.end() || *it == '}') break;
                    // 寻找key
                    if (*it != '\"') throw StructureException{ "Key is not string.\n" };
                    key = escape_next(str, it);
                    // 寻找分号
                    while (it != str.end() && std::isspace(*it)) ++it;
                    if(it == str.end()) throw StructureException{ "Illegal Json Object content.\n" };
                    if (*it == ':') ++it;
                    else throw StructureException {};
                    // 寻找value，递归解析
                    while (it != str.end() && std::isspace(*it)) ++it;
                    jsonObject[std::move(key)] = Value ( str, it );
                    // 寻找分隔符或结束位
                    while (it != str.end() && std::isspace(*it)) ++it;
                    if (it != str.end() && *it == ',') ++it;
                }
                // 对象类型，完成时必须是'}'，根据上面的写法，直接检测是不是end()即可
                if(it == str.end()) throw StructureException{ "Unclosed Json Object.\n" };
                ++it; // 注意返回时it的位置
            }
            break;
            case '[':
            {
                // 数组类型
                type_ = Type::array;
                content_ = Array{};
                auto& jsonArray = std::get<Array>(content_);
                ++it;
                while(it!=str.end()){
                    while (it!=str.end() && std::isspace(*it)) ++it;
                    if(it == str.end() || *it == ']') break;
                    // 寻找值，递归解析
                    jsonArray.emplace_back( str, it );
                    // 寻找分隔符或结束位
                    while (it != str.end() && std::isspace(*it)) ++it;
                    if (it != str.end() && *it == ',') ++it;
                }
                // 数组类型结束时必须是']'
                if(it==str.end()) throw StructureException{ "Unclosed Json Object.\n" };
                ++it;
            }
            break;
            case '\"':
            {
                // 字符类型，直接存入内部，不转义
                type_ = Type::string;
                auto left = it;
                ++it;
                while(it != str.end() && *it != '\"'){
                    if(*it == '\\'){
                        ++it;
                        if(it == str.end()) break;
                    }
                    ++it;
                }
                if(it == str.end()) throw StructureException {"Unclosed string.\n"};
                content_ = std::string (left, ++it);
            }
            break;
            case 't':
                if(str.end() - it < 4 || str.compare(it-str.begin(), 4, "true")) throw StructureException {};
                type_ = Type::boolean;
                content_ = true;
                it += 4;
            break;
            case 'f':
                if(str.end() - it < 5 || str.compare(it-str.begin(), 5, "false")) throw StructureException {};
                type_ = Type::boolean;
                it += 5;
            break;
            case 'n':
                if(str.end() - it < 4 || str.compare(it-str.begin(), 4, "null")) throw StructureException {};
                it += 4;
            break;
            default:
            {
                // 数值类型
                type_ = Type::number;
                bool have_not_point = true;
                bool have_not_e = true;
                auto left = it;
                // 必须数字或负号开头
                if(*it == '-') ++it;
                if(it == str.end() || !std::isdigit(*it)) throw StructureException {};
                // 根据JSON数据格式，可以出现一次.或者e
                while (it != str.end()) {
                    if (std::isdigit(*it)) ++it;
                    else if (*it == '.' && have_not_point && have_not_e) {
                        have_not_point = false;
                        ++it;
                    }
                    else if ((*it == 'e' || *it == 'E') && have_not_e) {
                        have_not_e = false;
                        ++it;
                        if (it != str.end() && (*it == '-' || *it == '+')) ++it;
                    }
                    else break;
                }
                if(it == left || (*left=='-' && it==left+1)) throw StructureException {};
                content_ = std::string (left, it);
            }
            break;
        }
    }

    // 列表初始化器
    Value::Value(const std::initializer_list<Value>& init_list) {
        if (init_list.size() == 0) return; // 空列表，直接默认构造
        if (init_list.size() == 1) { // 单个元素，直接复制
            *this = *init_list.begin();
        }
        else if (init_list.size() == 2 && init_list.begin()->is_string()){
            // 2个元素且第1个是字符串，则作为对象
            type_ = Type::object;
            content_ = Object{};
            auto& map = std::get<Object>(content_);
            map[init_list.begin()->as_string()] = init_list.begin()[1];
        }
        else { // 其他情况都认为是数组
            type_ = Type::array;
            content_ = Array{};
            auto& list = std::get<Array>(content_);
            list.reserve(init_list.size());
            for (const auto& it : init_list) {
                list.emplace_back(it);
            }
        }
    }

    // 拷贝构造
    Value::Value(const Value& jsonValue) noexcept{
        type_ = jsonValue.type_;
        content_ = jsonValue.content_;
    }
    //移动构造
    Value::Value(Value&& jsonValue) noexcept{
        type_ = jsonValue.type_;
        content_ = std::move(jsonValue.content_);
        jsonValue.reset();
    }
    // 拷贝赋值
    Value& Value::operator=(const Value& jsonValue) noexcept{
        if (this == &jsonValue) return *this;
        type_ = jsonValue.type_;
        content_ = jsonValue.content_;
        return *this;
    }
    // 移动赋值
    Value& Value::operator=(Value&& jsonValue) noexcept{
        if (this == &jsonValue) return *this;
        type_ = jsonValue.type_;
        content_ = std::move(jsonValue.content_);
        jsonValue.reset();
        return *this;
    }

    // 序列化JSON对象
    std::string Value::serialize() const noexcept {
        switch (type_) {
        case Type::object:
        {
            // 对象类型
            // 是否在开头加上逗号
            std::string res{ "{" };
            for (const auto& map = std::get<Object>(content_);
                const auto& [fst, snd] : map) {
                // 键是字符串，需要反转义
                res += reverse_escape(fst);
                res += ':';
                // 递归序列号
                res += snd.serialize();
                res += ',';
            }
            if (*res.rbegin() == ',') *res.rbegin() = '}';
            else res += '}';
            return res;
        }
        case Type::array:
        {
            // 数组类型
            // 是否在开头加上逗号
            std::string res{ "[" };
            for (const auto& list = std::get<Array>(content_);
                const Value& it : list) {
                // 递归序列号
                res += it.serialize();
                res += ',';
            }
            if (*res.rbegin() == ',') *res.rbegin() = ']';
            else res += ']';
            return res;
        }
        case Type::boolean:
            return std::get<bool>(content_) ? "true" : "false";
        case Type::null:
            return "null";
        default:
            // 数值和字符串，可以直接返回内容
            return std::get<std::string>(content_);
        }
    }
    // 序列化JSON对象含空格和换行
    std::string Value::serialize_pretty(const size_t& space_num, const size_t& depth) const noexcept {
        switch (type_) {
        case Type::object:
        {
            // 对象类型
            std::string res{ "{" };
            const size_t tabs  = depth * space_num + space_num;
            const auto& map = std::get<Object>(content_);
            for (const auto& [fst, snd] : map) {
                res += '\n';
                res.append(tabs, ' ');
                // 键是字符串，需要反转义
                res += reverse_escape(fst);
                res += ": ";
                res += snd.serialize_pretty(space_num, depth + 1);
                res += ',';
            }
            if (*res.rbegin() == ',') *res.rbegin() = '\n';
            if(!map.empty()){
                res.append(tabs - space_num, ' ');
                res += '}';
            }
            else res += " }";
            return res;
        }
        case Type::array:
        {
            // 数组类型
            std::string res{ "[" };
            const size_t tabs  = depth * space_num + space_num;
            const auto& list = std::get<Array>(content_);
            for (const Value& it : list) {
                res += '\n';
                res.append(tabs, ' ');
                res += it.serialize_pretty(space_num, depth + 1);
                res += ',';
            }
            if (*res.rbegin() == ',') *res.rbegin() = '\n';
            if(!list.empty()){
                res.append(tabs - space_num, ' ');
                res += ']';
            }
            else res += " ]";
            return res;
        }
        case Type::boolean:
            return std::get<bool>(content_) ? "true" : "false";
        case Type::null:
            return "null";
        default:
            // 数值和字符串，可以直接返回内容
            return std::get<std::string>(content_);
        }
    }

    //////////////////////////////////////////
    /// 类型转换函数

    // 字符串视图类型构造
    Value::Value(std::string_view str) noexcept{
        type_ = Type::string;
        content_ = reverse_escape(str);
    }
    // 字符串视图类型赋值
    Value& Value::operator=(std::string_view str) noexcept{
        type_ = Type::string;
        content_ = reverse_escape(str);
        return *this;
    }
    // 字符串类型构造
    Value::Value(const std::string& str) noexcept{
        type_ = Type::string;
        content_ = reverse_escape(str);
    }
    // 字符串类型赋值
    Value& Value::operator=(const std::string& str) noexcept{
        type_ = Type::string;
        content_ = reverse_escape(str);
        return *this;
    }
    // 字符串字面量构造
    Value::Value(const char* str) noexcept{
        type_ = Type::string;
        content_ = reverse_escape(str);
    }
    // 字符串字面量赋值
    Value& Value::operator=(const char* str) noexcept{
        type_ = Type::string;
        content_ = reverse_escape(str);
        return *this;
    }


    // Json::Array拷贝构造
    Value::Value(const Array& jsonArray) noexcept{
        type_ = Type::array;
        content_ = jsonArray;
    }
    // Json::Array移动构造
    Value::Value(Array&& jsonArray) noexcept{
        type_ = Type::array;
        content_ = std::move(jsonArray);
        jsonArray.clear();
    }
    // Json::Object拷贝构造
    Value::Value(const Object& jsonObject) noexcept{
        type_ = Type::object;
        content_ = jsonObject;
    }
    // Json::Object移动构造
    Value::Value(Object&& jsonObject) noexcept{
        type_ = Type::object;
        content_ = std::move(jsonObject);
        jsonObject.clear();
    }
    // Json::Array拷贝赋值
    Value& Value::operator=(const Array& jsonArray) noexcept{
        if (type_ == Type::array && &jsonArray == &std::get<Array>(content_))
            return *this;

        type_ = Type::array;
        content_ = jsonArray;
        return *this;
    }
    // Json::Array移动赋值
    Value& Value::operator=(Array&& jsonArray) noexcept{
        if (type_ == Type::array && &jsonArray == &std::get<Array>(content_))
            return *this;

        type_ = Type::array;
        content_ = std::move(jsonArray);
        jsonArray.clear();
        return *this;
    }
    // Json::Object拷贝赋值
    Value& Value::operator=(const Object& jsonObject) noexcept{
        if (type_ == Type::object && &jsonObject == &std::get<Object>(content_))
            return *this;

        type_ = Type::object;
        content_ = jsonObject;
        return *this;
    }
    // Json::Object移动赋值
    Value& Value::operator=(Object&& jsonObject) noexcept{
        if (type_ == Type::object && &jsonObject == &std::get<Object>(content_))
            return *this;

        type_ = Type::object;
        content_ = std::move(jsonObject);
        jsonObject.clear();
        return *this;
    }


    //////////////////////////////////////////
    /// as获取内容
    long long Value::as_int64() const {
        if (type_ != Type::number) throw TypeException{ "Is not Number.\n" };
        return std::stoll(std::get<std::string>(content_));
    }
    double Value::as_double() const {
        if (type_ != Type::number) throw TypeException{ "Is not Number.\n" };
        return std::stod(std::get<std::string>(content_));
    }
    long double Value::as_ldouble() const{
        if (type_ != Type::number) throw TypeException{ "Is not Number.\n" };
        return std::stold(std::get<std::string>(content_));
    }
    bool Value::as_bool() const {
        if (type_ != Type::boolean) throw TypeException{ "Is not bool.\n" };
        return std::get<bool>(content_);
    }
    std::string Value::as_string() const noexcept{
        if (type_ == Type::string) return escape(std::get<std::string>(content_));
        return this->serialize();
    }
    Object& Value::as_object(){
        if (type_ != Type::object) throw TypeException{ "Is not object.\n" };
        return std::get<Object>(content_);
    }
    const Object& Value::as_object() const{
        if (type_ != Type::object) throw TypeException{ "Is not object.\n" };
        return std::get<Object>(content_);
    }
    Array& Value::as_array(){
        if (type_ != Type::array) throw TypeException{ "Is not object.\n" };
        return std::get<Array>(content_);
    }
    const Array& Value::as_array() const{
        if (type_ != Type::array) throw TypeException{ "Is not object.\n" };
        return std::get<Array>(content_);
    }

    //////////////////////////////////////////
    /// 子元素访问
    // 列表访问，拒绝新元素
    Value& Value::at(const size_t& index) {
        if (type_ != Type::array) throw TypeException{ "Is not array.\n" };
        auto& list = std::get<Array>(content_);
        if (index < 0 || index >= list.size()) throw std::out_of_range{ "out of range.\n" };
        return list.at(index);
    }
    // 对象访问，拒绝新元素
    Value& Value::at(const std::string& key) {
        if (type_ != Type::object) throw TypeException{ "Is not Object.\n" };
        auto& map = std::get<Object>(content_);
        if (const auto it = map.find(key); it == map.end()) throw std::out_of_range{ + "Key not find.\n" };
        else return it->second;
    }
    // 列表访问，可能创建新元素
    Value& Value::operator[](const size_t& index) {
        if (type_ != Type::array) throw TypeException{ "Is not array.\n" };
        auto& list = std::get<Array>(content_);
        if (index == list.size()) list.emplace_back();
        return list[index];
    }
    // 对象访问，可能创建新元素
    Value& Value::operator[](const std::string& key) {
        if (type_ != Type::object) throw TypeException{ "Is not Object.\n" };
        auto& map = std::get<Object>(content_);
        return map[key];
    }
    // 检查是否包含某个key
    bool Value::contains(const std::string& key) const noexcept{
        if (type_ != Type::object) return false;
        const auto& map = std::get<Object>(content_);
        return map.contains(key);
    }

    //////////////////////////////////////////
    /// 增删检查
    // 数组末尾插入元素
    void Value::push_back(const Value& jsonValue) {
        if (type_ != Type::array) throw TypeException{ "Is not Array.\n" };
        auto& list = std::get<Array>(content_);
        list.push_back(jsonValue);
    }
    // 数组末尾移动进入元素
    void Value::push_back(Value&& jsonValue) {
        if (type_ != Type::array) throw TypeException{ "Is not Array.\n" };
        auto& list = std::get<Array>(content_);
        list.push_back(std::move(jsonValue));
    }
    // 数组尾部删除元素
    void Value::pop_back() {
        if (type_ != Type::array) throw TypeException{ "Is not Array.\n" };
        auto& list = std::get<Array>(content_);
        list.pop_back();
    }

    // 数值指定位置插入元素
    void Value::insert(const size_t& index, const Value& jsonValue) {
        if (type_ != Type::array) throw TypeException{ "Is not Array.\n" };
        auto& list = std::get<Array>(content_);
        if (index < 0 || index > list.size()) throw std::out_of_range{ "out of range.\n" };
        list.insert(std::next(list.begin(), static_cast<std::ptrdiff_t>(index)), jsonValue);
    }
    // 数组指定位置移入元素
    void Value::insert(const size_t& index, Value&& jsonValue) {
        if (type_ != Type::array) throw TypeException{ "Is not Array.\n" };
        auto& list = std::get<Array>(content_);
        if (index < 0 || index > list.size()) throw std::out_of_range{ "out of range.\n" };
        list.insert(std::next(list.begin(), static_cast<std::ptrdiff_t>(index)), std::move(jsonValue));
    }
    // 对象指定位置插入键值对
    void Value::insert(const std::string& key, const Value& jsonValue) {
        if (type_ != Type::object) throw TypeException{ "Is not Object.\n" };
        auto& map = std::get<Object>(content_);
        map[key] = jsonValue;
    }
    // 对象指定位置移动插入键值对
    void Value::insert(const std::string& key, Value&& jsonValue) {
        if (type_ != Type::object) throw TypeException{ "Is not Object.\n" };
        auto& map = std::get<Object>(content_);
        map[key] = std::move(jsonValue);
    }
    // 数值删除指定位置的元素
    void Value::erase(const size_t& index) {
        if (type_ != Type::array) throw TypeException{ "Is not array.\n" };
        auto& list = std::get<Array>(content_);
        if (index < 0 || index >= list.size()) throw std::out_of_range{ std::string{__FILE__} + ":" + std::to_string(__LINE__) + " out of range.\n" };
        list.erase(std::next(list.begin(),static_cast<std::ptrdiff_t>(index)));
    }
    // 对象删除指定key的元素
    void Value::erase(const std::string& key) {
        if (type_ != Type::object) throw TypeException{ "Is not Object.\n" };
        auto& map = std::get<Object>(content_);
        map.erase(key);
    }

    ////////////////////////////////////////////////////////////////////////////////////

}

