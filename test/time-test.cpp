#include <iostream>
#include <chrono>
#include <functional>
#include <format>

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
    long cout_time = benchmark([](){
        std::cout << "Hello world!" << std::endl;
    }, 1000).count();
    
    long printf_time = benchmark([](){
        std::printf("Hello world!\n");
    }, 1000).count();
    
    printf("printf was %f times faster than c++ streams", (float) cout_time / printf_time);
    
    return 0;
}