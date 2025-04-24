#include "my_array.hpp"
#include "my_vector.hpp"
#include <gtest/gtest.h>
#include <complex>
#include <vector>
#include <string>
#include <array>
#include <algorithm>
#include <numeric>


using myVector::my_vector;

// construction & capacity
TEST(MyVectorBasic, DefaultCtor) {
    my_vector<int> v;
    EXPECT_TRUE(v.is_empty());
    EXPECT_EQ(v.size(), 0u);
}

TEST(MyVectorBasic, InitializerList) {
    my_vector<int> v{1, 2, 3, 4};
    EXPECT_EQ(v.size(), 4u);
    EXPECT_EQ(v[2], 3);
}

TEST(MyVectorBasic, AtThrows) {
    my_vector<int> v{10, 20};
    EXPECT_THROW(v.at(2), std::out_of_range);
}

// Push/pop and element access
TEST(MyVectorModifiers, PushBackPopBack) {
    my_vector<int> v;
    v.push_back(5);
    v.push_back(7);
    EXPECT_EQ(v.front(), 5);
    EXPECT_EQ(v.back(), 7);
    EXPECT_EQ(v.size(), 2u);
    v.pop_back();
    EXPECT_EQ(v.size(), 1u);
    EXPECT_EQ(v.back(), 5);
}

// resize, clear and swap
TEST(MyVectorModifiers, ResizeAndClear) {
    my_vector<std::string> v;
    v.resize(3, "hi");
    EXPECT_EQ(v.size(), 3u);
    for (auto &s : v) EXPECT_EQ(s, "hi");
    v.clear();
    EXPECT_TRUE(v.is_empty());
}

TEST(MyVectorModifiers, Swap) {
    my_vector<int> a{1, 2, 3}, b{4, 5};
    a.swap(b);
    EXPECT_EQ(a.size(), 2u);
    EXPECT_EQ(b.size(), 3u);
    EXPECT_EQ(a[0], 4);
    EXPECT_EQ(b[2], 3);
}

// iterator compatibility with STL
TEST(MyVectorIterators, StdAlgorithms) {
    my_vector<int> v{1, 2, 3, 4, 5};
    int sum = std::accumulate(v.begin(), v.end(), 0);
    EXPECT_EQ(sum, 15);
    std::reverse(v.begin(), v.end());
    EXPECT_EQ(v.front(), 5);
}

// insert & erase variants
TEST(MyVectorInsertErase, SingleInsertErase) {
    my_vector<int> v{1, 3};
    auto it = v.begin() + 1;
    v.insert(v.cbegin(), 2);
    EXPECT_EQ(v.size(), 3u);
    EXPECT_EQ(v[1], 2);
    v.erase(v.cbegin());
    EXPECT_EQ(v.size(), 2u);
    EXPECT_EQ(v.front(), 2);
}

TEST(MyVectorInsertErase, FillInsert) {
    my_vector<int> v{1};
    v.insert(v.cbegin(), 3, 0);
    EXPECT_EQ(v.size(), 4u);
    for (size_t i = 0; i < 3; ++i) EXPECT_EQ(v[i], 0);
    EXPECT_EQ(v[3], 1);
}

TEST(MyVectorInsertErase, RangeInsert) {
    std::vector<int> src{7, 8, 9};
    my_vector<int> v{1, 2};
    v.insert(v.cbegin() + 1, src.begin(), src.end());
    EXPECT_EQ(v.size(), 5u);
    EXPECT_EQ(v[1], 7);
    EXPECT_EQ(v[4], 2);
}

// reserve/shrink assign
TEST(MyVectorCapacity, AssignAndReserve) {
    std::array<int,3> arr = {10,20,30};
    my_vector<int> v;
    v.assign(arr.begin(), arr.end());
    EXPECT_EQ(v.size(), 3u);
    EXPECT_EQ(v[1], 20);
    v.reserve(10);
    EXPECT_GE(v.capacity(), 10u);
    v.shrink_to_fit();
    EXPECT_EQ(v.capacity(), 3u);
}

// copyig move and comparison
TEST(MyVectorSpecial, CopyMoveCompare) {
    my_vector<int> a{1,2,3};
    auto b = a;               // copy
    EXPECT_EQ(b, a);
    auto c = std::move(a);
    EXPECT_EQ(c.size(), 3u);
    EXPECT_TRUE(a.is_empty());
    my_vector<int> d{1,2,4};
    EXPECT_TRUE(c < d);
    EXPECT_TRUE(d > c);
    my_vector<int> e{1,2,3};
    EXPECT_TRUE(c == e);
}

// complex types
TEST(MyVectorComplex, StdComplex) {
    using cd = std::complex<double>;
    my_vector<cd> v{cd(1,2), cd(3,4)};
    EXPECT_EQ(v[0], cd(1,2));
    EXPECT_EQ(v.back(), cd(3,4));
}

// fundamental type tests
TEST(MyArrayFundamental, DefaultCtorAndFill) {
    my_array<int, 3> a;
    EXPECT_EQ(a.size(), 3u);
    a.fill(42);
    for (size_t i = 0; i < a.size(); ++i)
        EXPECT_EQ(a[i], 42);
}

TEST(MyArrayFundamental, InitializerList) {
    my_array<int, 4> a{1, 2, 3, 4};
    for (size_t i = 0; i < a.size(); ++i)
        EXPECT_EQ(a[i], static_cast<int>(i + 1));
}

TEST(MyArrayFundamental, AtThrows) {
    my_array<int, 2> a{5, 6};
    EXPECT_THROW(a.at(2), std::out_of_range);
}

// simple class (std::complex)
TEST(MyArrayComplex, StdComplex) {
    using cd = std::complex<double>;
    my_array<cd, 2> a{cd(1, 2), cd(3, 4)};
    EXPECT_EQ(a[0], cd(1, 2));
    EXPECT_EQ(a[1], cd(3, 4));
    // test copy via std::array conversion
    std::array<cd, 2> arr{{cd(9, 9), cd(8, 8)}};
    my_array<cd, 2> b = arr;
    EXPECT_EQ(b[0], cd(9, 9));
}

// complex class (std::vector, std::string)
TEST(MyArrayComplexType, VectorAndString) {
    my_array<std::string, 3> a;
    a.fill("hi");
    for (auto &s : a) EXPECT_EQ(s, "hi");
    // transform
    std::transform(a.begin(), a.end(), a.begin(), [](auto const &s){ return s + "!"; });
    EXPECT_EQ(a[1], "hi!");

    my_array<std::vector<int>, 2> v;
    v[0] = std::vector<int>{1, 2};
    v[1] = std::vector<int>{3, 4, 5};
    EXPECT_EQ(v[0].size(), 2u);
    EXPECT_EQ(v[1].size(), 3u);
}

// nested my_array
TEST(MyArrayNested, NestedArray) {
    my_array<my_array<int, 5>, 5> nested;
    // fill each inner array with 7
    for (auto &inner : nested) inner.fill(7);
    for (auto &inner : nested)
        for (auto val : inner)
            EXPECT_EQ(val, 7);
}

// comparisons tests
TEST(MyArrayComparison, OperatorSpaceship) {
    my_array<int, 3> a{1, 2, 3};
    my_array<int, 3> b{1, 2, 4};
    EXPECT_TRUE(a < b);
    EXPECT_TRUE(a <= b);
    EXPECT_TRUE(b > a);
    EXPECT_TRUE(a != b);
    my_array<int, 3> c{1, 2, 3};
    EXPECT_TRUE(a == c);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
