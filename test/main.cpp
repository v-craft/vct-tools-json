#include <vct/test_unit_macros.hpp>

import std;
import vct.test.unit;
import vct.tools.json;

int main() {
    vct::test::unit::start();
}

struct Test {
    int m_type;

    operator ::vct::tools::json::Value() const { 
        ::vct::tools::json::Value macro_result{::vct::tools::json::Type::eObject}; 
        return macro_result; 
    } 
    operator ::vct::tools::json::Value() const volatile { 
        ::vct::tools::json::Value macro_result{::vct::tools::json::Type::eObject}; 
        return macro_result; 
    } 
    operator ::vct::tools::json::Value() & { 
        ::vct::tools::json::Value macro_result{::vct::tools::json::Type::eObject}; 

        return macro_result; 
    } 
    operator ::vct::tools::json::Value() && { 
        ::vct::tools::json::Value macro_result{::vct::tools::json::Type::eObject}; 

        return macro_result; 
    } 
    operator ::vct::tools::json::Value() volatile & { 
        ::vct::tools::json::Value macro_result{::vct::tools::json::Type::eObject}; 

        return macro_result; 
    } 
    operator ::vct::tools::json::Value() volatile && { 
        ::vct::tools::json::Value macro_result{::vct::tools::json::Type::eObject}; 

        return macro_result; 
    } 

};

