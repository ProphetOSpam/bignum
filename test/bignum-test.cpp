#include <iostream>
#include <format>
#include <cassert>

#include "../src/bignum.hpp"

int main(void) {
    BigNum bignum1(std::array {1, 0, 3});
    BigNum bignum2(std::array    {2, 3});

    BigNum copy(bignum1);

    // Addition
    puts(std::format("{}", BigNum(std::array {1, 0}) + BigNum(std::array {1, 0, 0})).c_str());
    puts(std::format("{}", BigNum(std::array {1, 0, 0}) + BigNum(std::array {1, 0, 0})).c_str());
    puts(std::format("{}", BigNum(std::array {1, 0, 0}) + BigNum(std::array {1, 0, 0})).c_str());

    puts(std::format("num1 copy {}", copy).c_str());

    puts(std::format("num1 {}", bignum1).c_str());
    puts(std::format("num2 {}", bignum2).c_str());
    puts(std::format("addition {}", bignum1 + bignum2).c_str());
    puts(std::format("addition with num {}", bignum2 + 1).c_str());

    copy += bignum2;

    puts(std::format("copy addition and assignment {}", copy).c_str());

    puts(std::format("pre-incrememt {}", ++bignum2).c_str());
    puts(std::format("post-incrememt {}", bignum2++).c_str());
    puts(std::format("after the post-incrememt {}", bignum2).c_str());


    BigNum smol(std::array {3, 1});
    puts(std::format("1: {}, 2: {}", smol, bignum2).c_str());
    puts(std::format("subtraction {}", smol - bignum2).c_str());

    BigNum beeg(std::array {BigNum::LARGEST_DIGIT});
    puts(std::format("overflow addition with reallocation before {}", beeg).c_str());
    puts(std::format("overflow addition with reallocation after {}", beeg + 1).c_str());

    assert(beeg + 1 == beeg + 1);
    assert(beeg + 2 != beeg + 1);


    assert(BigNum(1) == BigNum(1));
    assert(BigNum(1) == 1);
    assert(BigNum(1) < BigNum(std::array {1, 2, 3}));
    assert(BigNum(1) < 5);
    assert(BigNum(std::array {1, 2, 3}) > BigNum(1));
    assert(BigNum(5) > 1);
}