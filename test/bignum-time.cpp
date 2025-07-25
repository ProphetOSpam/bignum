#include <iostream>
#include <format>
#include "../src/bignum.hpp"

int main(void) {
    BigNum num(std::array {(BigNum::DataType) 1, BigNum::LARGEST_DIGIT, BigNum::LARGEST_DIGIT});
    while (true) {
        puts(std::format("{}", --num).c_str());
    }
}

