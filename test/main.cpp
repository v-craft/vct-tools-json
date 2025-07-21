import std;
import vct.test.unit;
import vct.tools.json;

using namespace vct::tools;

int main() {
    std::println("=====================================");
    std::println("Value size {}", sizeof(vct::tools::json::Value));
    auto it = sizeof(std::variant<
            json::Null,       ///< JSON null value storage
            json::Bool,       ///< JSON boolean value storage
            json::Number,     ///< JSON number storage
            json::String,     ///< JSON string storage
            json::Array,      ///< JSON array storage
            json::Object      ///< JSON object storage
        >);
    std::println("variant size {}", it);


    return vct::test::unit::start();
}

