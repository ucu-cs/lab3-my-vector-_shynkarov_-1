#include <iostream>
#include <algorithm>
#include <string>
#include <chrono>
#include <vector>


#include "my_vector.hpp"
#include "my_array.hpp"

using namespace std::chrono;
using myVector::my_vector;


// helper to time a lambda, returning microseconds
template<typename F>
long long time_us(F&& fn) {
    auto t0 = high_resolution_clock::now();
    fn();
    auto t1 = high_resolution_clock::now();
    return duration_cast<microseconds>(t1 - t0).count();
}

int main() {

    constexpr size_t N = 10'000'000;

    // 1) push-back comparioson
    std::vector<int>     std_v; std_v.reserve(N);
    my_vector<int>       my_v;  my_v.reserve(N);

    auto std_push_us = time_us([&](){
        for(size_t i = 0; i < N; ++i) std_v.push_back(int(i));
    });
    auto my_push_us  = time_us([&](){
        for(size_t i = 0; i < N; ++i) my_v.push_back(int(i));
    });

    // 2) copy-construction
    auto std_copy_us = time_us([&](){
        std::vector<int> copy(std_v);
        (void)copy;
    });
    auto my_copy_us  = time_us([&](){
        my_vector<int> copy(my_v);
        (void)copy;
    });

    // 3) iteration
    volatile long long std_sum = 0, my_sum = 0;  // volatile
    auto my_iter_us  = time_us([&](){
        for(auto x : my_v)  my_sum  += x;
    });
    auto std_iter_us = time_us([&](){
        for(auto x : std_v) std_sum += x;
    });

    std::cout
        << "push-back: std=" << std_push_us << "µs, my=" << my_push_us  << " µs\n"
        << "copy-ctor: std=" << std_copy_us << "µs, my=" << my_copy_us  << " µs\n"
        << "iteration: std=" << std_iter_us << "µs, my=" << my_iter_us  << " µs\n";


    // worked hah, im happy. auto-destroy
    myVector::my_vector<myVector::my_vector<int>>({{1,2},{3,4}});

    // ARRAY SECTION BELOW

    my_array<int, 4> a{};
    a.fill(7);

    my_array<int, 4> b{1,2,3,4};
    std::copy(b.begin(), b.end(), a.begin());

    for (auto x : a) std::cout << x << ' ';  // prints: 1 2 3 4
    std::cout<<'\n';

    std::array<std::string, 2> vs{{"hi","bye"}};
    my_array<std::string, 2> ms = vs;      // works via converting ctor
    ms = vs;                               // works via operator=

    std::transform(ms.begin(), ms.end(),
                   ms.begin(),
                   [](auto const& s){ return s + "!"; });

    // structured binding
    auto x0 = ms[0];
    auto x1 = ms[1];
    std::cout << x0 << ',' << x1 << '\n';    // "hi!,bye!"

    my_array<my_array<int, 5>, 5>;

    return 0;
}
