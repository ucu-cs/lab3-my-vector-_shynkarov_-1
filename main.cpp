#include <iostream>
#include <algorithm>
#include <string>

#include "my_vector.hpp"
#include "my_array.hpp"



int main() {
    myVector::my_vector<int> vec;
    vec.push_back(148);

    std::cout << "Vector size: " << vec.size() << '\n';
    std::cout << "Vector capacity: " << vec.capacity() << '\n';
    std::cout << "Vector back: " << vec.back() << '\n';

    vec.push_back(5885);
    std::cout << "Vector size: " << vec.size() << '\n';
    std::cout << "Vector capacity: " << vec.capacity() << '\n';
    std::cout << "Vector back: " << vec.back() << '\n';


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
