#ifndef MY_ARRAY_H
#define MY_ARRAY_H

#include <cstddef>        // std::size_t
#include <stdexcept>      // std::out_of_range, std::length_error
#include <iterator>       // std::reverse_iterator
#include <compare>        // std::strong_ordering
#include <utility>        // std::swap, std::move
#include <type_traits>    // std::integral_constant
#include <tuple>          // std::tuple_size, std::tuple_element
#include <array>          // std::array
#include <concepts>       // std::convertible_to
#include <initializer_list> // std::initializer_list

// implementation does not use dynamic memory allocation
// There are no calls to new/malloc under the hood
// the only allocations you’ll ever see are whatever element type T does


// A fixed-size array container, drop-in replacement for std::array
template<typename T, std::size_t N>
struct my_array {
    // underlying storage
    T elems[N];

    // initializer-list constructor
    constexpr my_array(std::initializer_list<T> il) {
        if (il.size() != N) throw std::length_error("my_array initializer_list size mismatch");
        std::size_t i = 0;
        for (auto& v : il) elems[i++] = v;
    }

    // converting constructor from std::array<U, N>
    template<std::convertible_to<T> U>
    constexpr my_array(const std::array<U, N>& other)
        noexcept(noexcept(T(std::declval<U>())))
    {
        for (std::size_t i = 0; i < N; ++i)
            elems[i] = other[i];
    }

    // defaulted special members
    constexpr my_array() = default;
    constexpr my_array(const my_array&) = default;
    constexpr my_array(my_array&&) noexcept = default;
    constexpr my_array& operator=(const my_array&) = default;
    constexpr my_array& operator=(my_array&&) noexcept = default;
    ~my_array() = default;

    // assignment from std::array<U, N>
    template<std::convertible_to<T> U>
    constexpr my_array& operator=(const std::array<U, N>& other)
        noexcept(noexcept(elems[0] = other[0]))
    {
        for (std::size_t i = 0; i < N; ++i)
            elems[i] = other[i];
        return *this;
    }

    // element access
    constexpr       T* data() noexcept       { return elems; }
    constexpr const T* data() const noexcept { return elems; }
    constexpr       T& operator[](std::size_t i) noexcept       { return elems[i]; }
    constexpr const T& operator[](std::size_t i) const noexcept { return elems[i]; }
    constexpr       T& at(std::size_t i) {
        if (i >= N) throw std::out_of_range("my_array::at");
        return elems[i];
    }
    constexpr const T& at(std::size_t i) const {
        if (i >= N) throw std::out_of_range("my_array::at");
        return elems[i];
    }
    constexpr       T& front() noexcept { static_assert(N>0, "front() on empty"); return elems[0]; }
    constexpr const T& front() const noexcept { static_assert(N>0, "front() on empty"); return elems[0]; }
    constexpr       T& back() noexcept  { static_assert(N>0, "back() on empty"); return elems[N-1]; }
    constexpr const T& back() const noexcept { static_assert(N>0, "back() on empty"); return elems[N-1]; }


    // capacity, i.e. please warn me if the return value of this function (or type) is discarded
    // i've liked the idea of custom message inside nodiscard
    [[nodiscard("returns the number of elements; don’t ignore this value")]]
    constexpr std::size_t size() const noexcept { return N; }
    [[nodiscard("returns true if empty; don’t ignore this check")]]
    constexpr bool empty() const noexcept { return N == 0; }

    // modifiers
    constexpr void fill(const T& value)
        noexcept(noexcept(std::declval<T&>() = value))
    {
        for (auto& e : elems) e = value;
    }
    constexpr void swap(my_array& other)
        noexcept(noexcept(std::swap(std::declval<T&>(), std::declval<T&>())))
    {
        for (std::size_t i=0; i<N; ++i)
            std::swap(elems[i], other.elems[i]);
    }

    // iteration
    constexpr       T* begin() noexcept                { return elems; }
    constexpr const T* begin() const noexcept          { return elems; }
    constexpr       T* end() noexcept                  { return elems + N; }
    constexpr const T* end() const noexcept            { return elems + N; }
    constexpr       T* cbegin() const noexcept         { return elems; }
    constexpr       T* cend()   const noexcept         { return elems + N; }
    constexpr auto rbegin() noexcept                   { return std::reverse_iterator(end()); }
    constexpr auto rend() noexcept                     { return std::reverse_iterator(begin()); }
    constexpr auto crbegin() const noexcept            { return std::reverse_iterator(cend()); }
    constexpr auto crend()   const noexcept            { return std::reverse_iterator(cbegin()); }

    // comparisons
    constexpr auto operator<=>(my_array const&) const
        noexcept(noexcept(std::declval<T>() <=> std::declval<T>()))
        = default;
    constexpr bool operator==(my_array const&) const
        noexcept(noexcept(std::declval<T>() == std::declval<T>()))
        = default;
};

// ADL‑friendly swap
template<typename T, std::size_t N>
constexpr void swap(my_array<T,N>& a, my_array<T,N>& b)
    noexcept(noexcept(a.swap(b)))
{
    a.swap(b);
}

// structured bindings
namespace std {
    template<typename T, std::size_t N>
    struct tuple_size<my_array<T,N>> : integral_constant<std::size_t,N> {};
    template<std::size_t I, typename T, std::size_t N>
    struct tuple_element<I, my_array<T,N>> { using type = T; };
    template<std::size_t I, typename T, std::size_t N>
    constexpr T& get(my_array<T,N>& arr) noexcept { return arr.elems[I]; }
    template<std::size_t I, typename T, std::size_t N>
    constexpr const T& get(const my_array<T,N>& arr) noexcept { return arr.elems[I]; }
    template<std::size_t I, typename T, std::size_t N>
    constexpr T&& get(my_array<T,N>&& arr) noexcept { return std::move(arr.elems[I]); }
}

#endif // MY_ARRAY_H
