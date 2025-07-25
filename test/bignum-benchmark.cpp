#include <iostream>
#include <chrono>
#include <functional>
#include <format>
#include "../src/bignum.hpp"

auto benchmark(std::function<void()> fn, int iterations) {
    auto accum = std::chrono::nanoseconds::zero();
    for (int i = 0; i < iterations; i++) {
        const auto start = std::chrono::high_resolution_clock::now();
        fn();
        const auto end = std::chrono::high_resolution_clock::now();
        accum += end - start;
    }
    return accum / iterations;
}

int main(void) {
    BigNum i(0);
    long bignum_time = benchmark([i]() mutable {
        for (i.reset(); i < 1'000; ++i);
        puts(std::format("{}", i).c_str());
    }, 100'000).count();
    

    long num_time = benchmark([](){
        for (int j = 0; j < 1'000; j++);
    }, 100'000).count();
    
    puts(std::format("BigNum was {} times slower than regular int", (float) bignum_time / num_time).c_str());
    
    return 0;
}