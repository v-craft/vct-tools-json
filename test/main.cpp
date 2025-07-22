import std;
import vct.test.unit;
import vct.tools.json;

using namespace vct::tools;

int main() {

    // std::ofstream out{ CURRENT_PATH "/files/many_string.json" };
    // if (!out) {
    //     std::cerr << "Failed to open output file." << std::endl;
    //     return 1;
    // }
    //
    // static const std::string charset = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789`~!@#$%^&*()_+-={}[]:;\"'<>,.?/ \t\n\r\f\b";  // 空格、制表
    // static std::random_device rd;
    // static std::mt19937 gen(rd());
    // std::uniform_int_distribution<> dis(0, charset.size() - 1);
    //
    // json::Value val = json::Array{};
    // for (int i=0; i< 20000; ++i) {
    //     std::string str;
    //     str.reserve(50*( i / 500 + 1));
    //     for (int j=0;j<100*( i / 1000 + 1);++j) {
    //         str += charset[dis(gen)];
    //     }
    //     val.push_back(std::move(str));
    // }
    // val.prettify_to(out);
    // out.close();


    return vct::test::unit::start();
}

