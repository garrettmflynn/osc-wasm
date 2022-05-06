#include "emscripten.h"
#include <emscripten/bind.h>
#include <iostream>
#include "dummy/arithm.hpp"
#include "dummy/fibonacci.hpp"

EMSCRIPTEN_KEEPALIVE
using namespace emscripten;

    
int main() {
    int a = add(10, 20);
    std::cout << "10 + 20 = " << a << std::endl;
    int m = mul(10, 20);
    std::cout << "10 x 20 = " << m << std::endl;
    float d = divide(30, 10);
    std::cout << "30 / 10 = " << d << std::endl;
    
    std::cout << "Fibonacci(16):" << fibonacci(16) << std::endl;
}

EMSCRIPTEN_BINDINGS(my_module) {
    function("main", &main);
}
