#ifndef MY_VECTOR_H
#define MY_VECTOR_H

#include <algorithm>
#include <cstddef>
#include <initializer_list>
#include <iterator>
#include <stdexcept>
#include <type_traits>
#include <utility>

namespace myVector
{
    template <typename T>
    class my_vector
    {
      public:
        using value_type = T;
        using size_type = std::size_t;
        using difference_type = std::ptrdiff_t;
        using reference = value_type&;
        using const_reference = const value_type&;
        using pointer = value_type*;
        using const_pointer = const value_type*;
        bool operator==(const my_vector& other) const = default; // for tests


      private:
        pointer   data_;     // pointer to data buffer
        size_type size_;     // number of elements
        size_type capacity_; // buffer size in elements

        // Helper method declarations
        static pointer allocate(size_type n);
        void           deallocate();
        static void    deallocate(pointer ptr, size_type n);
        static void    destroy_range(pointer first, pointer last);
        void           allocate_and_fill(size_type n, const T& value);
        template <typename InputIt>
        void                    allocate_and_copy(InputIt first, InputIt last);
        void                    reallocate(size_type new_capacity);
        void                    reallocate_insert(size_type index, const T& value, size_type new_capacity);
        void                    shift_right(size_type index, size_type count);
        void                    shift_left(size_type index, size_type count);
        [[nodiscard]] size_type calculate_growth(size_type new_size) const;

      public:
        // Constructor declarations
        my_vector() noexcept;
        explicit my_vector(size_type n, const T& value = T());
        template <typename InputIt, typename = typename std::enable_if<!std::is_integral<InputIt>::value>::type>
        my_vector(InputIt first, InputIt last);
        my_vector(std::initializer_list<T> init);
        my_vector(const my_vector& other);
        my_vector(my_vector&& other) noexcept;

        // Destructor declaration
        ~my_vector();

        // Operator declarations
        my_vector& operator=(const my_vector& other);
        my_vector& operator=(my_vector&& other) noexcept;
        my_vector& operator=(std::initializer_list<T> ilist);

        // Element access
        reference       operator[](size_type pos);
        const_reference operator[](size_type pos) const;
        reference       at(size_type pos);
        const_reference at(size_type pos) const;
        reference       front();
        const_reference front() const;
        reference       back();
        const_reference back() const;
        pointer         data() noexcept;
        const_pointer   data() const noexcept;

        // Capacity
        [[nodiscard]] bool      is_empty() const noexcept;
        [[nodiscard]] size_type size() const noexcept;
        static size_type        max_size() noexcept;
        void                    reserve(size_type new_cap);
        [[nodiscard]] size_type capacity() const noexcept;
        void                    shrink_to_fit();

        // Modifiers
        void clear() noexcept;
        void push_back(const T& value);
        void push_back(T&& value);
        template <typename... Args>
        reference emplace_back(Args&&... args);
        void      pop_back();
        void      resize(size_type new_size);
        void      resize(size_type new_size, const T& value);
        void      assign(size_type n, const T& value);
        template <typename InputIt, typename = typename std::enable_if<!std::is_integral<InputIt>::value>::type>
        void assign(InputIt first, InputIt last);
        void assign(std::initializer_list<T> ilist);
        void swap(my_vector& other) noexcept;

        // Spaceship operator
        auto operator<=>(const my_vector& other) const;

        template <typename PtrType, typename RefType>
        class base_iterator
        {
          public:
            using iterator_category = std::random_access_iterator_tag;
            using value_type = T;
            using difference_type = std::ptrdiff_t;
            using pointer = PtrType;
            using reference = RefType;

            base_iterator() : ptr_(nullptr) {}
            explicit base_iterator(pointer ptr) : ptr_(ptr) {}

            template <typename OtherPtr, typename OtherRef>
            explicit base_iterator(const base_iterator<OtherPtr, OtherRef>& other) : ptr_(other.ptr_) {}

            reference operator*() const { return *ptr_; }
            pointer   operator->() const { return ptr_; }

            // clang-format off
            base_iterator& operator++() { ++ptr_; return *this; }
            base_iterator operator++(int) { base_iterator tmp = *this; ++ptr_; return tmp; }

            base_iterator& operator--() { --ptr_; return *this; }
            base_iterator operator--(int) { base_iterator tmp = *this; --ptr_; return tmp; }

            base_iterator& operator+=(difference_type n) { ptr_ += n; return *this; }
            base_iterator& operator-=(difference_type n) { ptr_ -= n; return *this; }
            // clang-format on

            base_iterator operator+(difference_type n) const { return base_iterator(ptr_ + n); }
            base_iterator operator-(difference_type n) const { return base_iterator(ptr_ - n); }

            difference_type operator-(const base_iterator& other) const { return ptr_ - other.ptr_; }

            reference operator[](difference_type n) const { return ptr_[n]; }

            auto operator<=>(const base_iterator& other) const = default;

          private:
            pointer ptr_;

            template <typename, typename>
            friend class base_iterator;
            friend class my_vector<T>;
        };

        using iterator = base_iterator<T*, T&>;
        using const_iterator = base_iterator<const T*, const T&>;

        using reverse_iterator = std::reverse_iterator<iterator>;
        using const_reverse_iterator = std::reverse_iterator<const_iterator>;

        // Iterators
        iterator               begin() noexcept;
        const_iterator         begin() const noexcept;
        const_iterator         cbegin() const noexcept;
        iterator               end() noexcept;
        const_iterator         end() const noexcept;
        const_iterator         cend() const noexcept;
        reverse_iterator       rbegin() noexcept;
        const_reverse_iterator rbegin() const noexcept;
        const_reverse_iterator crbegin() const noexcept;
        reverse_iterator       rend() noexcept;
        const_reverse_iterator rend() const noexcept;
        const_reverse_iterator crend() const noexcept;

        // Modifiers
        iterator insert(const_iterator pos, const T& value);
        iterator insert(const_iterator pos, T&& value);
        iterator insert(const_iterator pos, size_type n, const T& value);
        template <typename InputIt, typename = typename std::enable_if<!std::is_integral<InputIt>::value>::type>
        iterator insert(const_iterator pos, InputIt first, InputIt last);
        iterator insert(const_iterator pos, std::initializer_list<T> ilist);
        iterator erase(const_iterator pos);
        iterator erase(const_iterator first, const_iterator last);
    };

    // Constructor implementations
    template <typename T>
    my_vector<T>::my_vector() noexcept : data_(nullptr), size_(0), capacity_(0) {}

    template <typename T>
    my_vector<T>::my_vector(size_type n, const T& value) : data_(nullptr), size_(0), capacity_(0) {
        if (n > 0)
        {
            allocate_and_fill(n, value);
        }
    }

    template <typename T>
    template <typename InputIt, typename>
    my_vector<T>::my_vector(InputIt first, InputIt last) : data_(nullptr), size_(0), capacity_(0) {
        assign(first, last);
    }

    template <typename T>
    my_vector<T>::my_vector(std::initializer_list<T> init) : data_(nullptr), size_(0), capacity_(0) {
        assign(init.begin(), init.end());
    }

    template <typename T>
    my_vector<T>::my_vector(const my_vector& other) : data_(nullptr), size_(0), capacity_(0) {
        if (other.size_ > 0)
        {
            allocate_and_copy(other.begin(), other.end());
            size_ = other.size_;
        }
    }

    template <typename T>
    my_vector<T>::my_vector(my_vector&& other) noexcept :
        data_(other.data_), size_(other.size_), capacity_(other.capacity_) {
        other.data_ = nullptr;
        other.size_ = 0;
        other.capacity_ = 0;
    }

    // Destructor implementation
    template <typename T>
    my_vector<T>::~my_vector() {
        clear();
        deallocate();
    }

    // Assignment operators implementation
    // Copy
    template <typename T>
    my_vector<T>& my_vector<T>::operator=(const my_vector& other) {
        if (this != &other)
        {
            my_vector tmp(other);
            swap(tmp);
        }
        return *this;
    }

    // Move
    template <typename T>
    my_vector<T>& my_vector<T>::operator=(my_vector&& other) noexcept {
        if (this != &other)
        {
            clear();
            deallocate();

            data_ = other.data_;
            size_ = other.size_;
            capacity_ = other.capacity_;

            other.data_ = nullptr;
            other.size_ = 0;
            other.capacity_ = 0;
        }
        return *this;
    }

    // Init list
    template <typename T>
    my_vector<T>& my_vector<T>::operator=(std::initializer_list<T> ilist) {
        assign(ilist.begin(), ilist.end());
        return *this;
    }

    // Element access implementations
    template <typename T>
    typename my_vector<T>::reference my_vector<T>::operator[](size_type pos) {
        return data_[pos];
    }

    template <typename T>
    typename my_vector<T>::const_reference my_vector<T>::operator[](size_type pos) const {
        return data_[pos];
    }

    template <typename T>
    typename my_vector<T>::reference my_vector<T>::at(size_type pos) {
        if (pos >= size_)
        {
            throw std::out_of_range("my_vector::at: index out of range");
        }
        return data_[pos];
    }

    template <typename T>
    typename my_vector<T>::const_reference my_vector<T>::at(size_type pos) const {
        if (pos >= size_)
        {
            throw std::out_of_range("my_vector::at: index out of range");
        }
        return data_[pos];
    }

    template <typename T>
    typename my_vector<T>::reference my_vector<T>::front() {
        return data_[0];
    }

    template <typename T>
    typename my_vector<T>::const_reference my_vector<T>::front() const {
        return data_[0];
    }

    template <typename T>
    typename my_vector<T>::reference my_vector<T>::back() {
        return data_[size_ - 1];
    }

    template <typename T>
    typename my_vector<T>::const_reference my_vector<T>::back() const {
        return data_[size_ - 1];
    }

    template <typename T>
    typename my_vector<T>::pointer my_vector<T>::data() noexcept {
        return data_;
    }

    template <typename T>
    typename my_vector<T>::const_pointer my_vector<T>::data() const noexcept {
        return data_;
    }

    // Iterator implementations
    template <typename T>
    typename my_vector<T>::iterator my_vector<T>::begin() noexcept {
        return iterator(data_);
    }

    template <typename T>
    typename my_vector<T>::const_iterator my_vector<T>::begin() const noexcept {
        return const_iterator(data_);
    }

    template <typename T>
    typename my_vector<T>::const_iterator my_vector<T>::cbegin() const noexcept {
        return const_iterator(data_);
    }

    template <typename T>
    typename my_vector<T>::iterator my_vector<T>::end() noexcept {
        return iterator(data_ + size_);
    }

    template <typename T>
    typename my_vector<T>::const_iterator my_vector<T>::end() const noexcept {
        return const_iterator(data_ + size_);
    }

    template <typename T>
    typename my_vector<T>::const_iterator my_vector<T>::cend() const noexcept {
        return const_iterator(data_ + size_);
    }

    template <typename T>
    typename my_vector<T>::reverse_iterator my_vector<T>::rbegin() noexcept {
        return reverse_iterator(end());
    }

    template <typename T>
    typename my_vector<T>::const_reverse_iterator my_vector<T>::rbegin() const noexcept {
        return const_reverse_iterator(end());
    }

    template <typename T>
    typename my_vector<T>::const_reverse_iterator my_vector<T>::crbegin() const noexcept {
        return const_reverse_iterator(end());
    }

    template <typename T>
    typename my_vector<T>::reverse_iterator my_vector<T>::rend() noexcept {
        return reverse_iterator(begin());
    }

    template <typename T>
    typename my_vector<T>::const_reverse_iterator my_vector<T>::rend() const noexcept {
        return const_reverse_iterator(begin());
    }

    template <typename T>
    typename my_vector<T>::const_reverse_iterator my_vector<T>::crend() const noexcept {
        return const_reverse_iterator(begin());
    }

    // Capacity implementations
    template <typename T>
    bool my_vector<T>::is_empty() const noexcept {
        return size_ == 0;
    }

    template <typename T>
    typename my_vector<T>::size_type my_vector<T>::size() const noexcept {
        return size_;
    }

    template <typename T>
    typename my_vector<T>::size_type my_vector<T>::max_size() noexcept {
        return std::numeric_limits<size_type>::max() / sizeof(T);
    }

    template <typename T>
    void my_vector<T>::reserve(size_type new_cap) {
        if (new_cap > capacity_)
        {
            reallocate(new_cap);
        }
    }

    template <typename T>
    typename my_vector<T>::size_type my_vector<T>::capacity() const noexcept {
        return capacity_;
    }

    template <typename T>
    void my_vector<T>::shrink_to_fit() {
        if (size_ < capacity_)
        {
            reallocate(size_);
        }
    }

    // Modifier implementations
    template <typename T>
    void my_vector<T>::clear() noexcept {
        destroy_range(data_, data_ + size_);
        size_ = 0;
    }

    template <typename T>
    typename my_vector<T>::iterator my_vector<T>::insert(const_iterator pos, const T& value) {
        size_type index = pos.ptr_ - data_;

        if (size_ == capacity_)
        {
            size_type new_capacity = calculate_growth(size_ + 1);
            reallocate_insert(index, value, new_capacity);
        } else {
            shift_right(index, 1);
            try
            {
                T value_copy(value);
                new (data_ + index) T(std::move(value_copy));
            } catch (...)
            {
                shift_left(index + 1, 1);
                throw;
            }
            ++size_;
        }

        return iterator(data_ + index);
    }

    template <typename T>
    typename my_vector<T>::iterator my_vector<T>::insert(const_iterator pos, T&& value) {
        size_type index = pos.ptr_ - data_;

        if (size_ == capacity_)
        {
            size_type new_capacity = calculate_growth(size_ + 1);
            pointer   new_data = allocate(new_capacity);
            size_type new_size = 0;

            try
            {
                for (size_type i = 0; i < index; ++i)
                {
                    new (new_data + new_size) T(std::move(data_[i]));
                    ++new_size;
                }

                new (new_data + new_size) T(std::move(value));
                ++new_size;

                for (size_type i = index; i < size_; ++i)
                {
                    new (new_data + new_size) T(std::move(data_[i]));
                    ++new_size;
                }
            } catch (...)
            {
                destroy_range(new_data, new_data + new_size);
                deallocate(new_data, new_capacity);
                throw;
            }

            destroy_range(data_, data_ + size_);
            deallocate();

            data_ = new_data;
            size_ = new_size;
            capacity_ = new_capacity;
        } else {
            shift_right(index, 1);
            try
            { new (data_ + index) T(std::move(value)); } catch (...)
            {
                shift_left(index + 1, 1);
                throw;
            }
            ++size_;
        }

        return iterator(data_ + index);
    }

    template <typename T>
    typename my_vector<T>::iterator my_vector<T>::insert(const_iterator pos, size_type n, const T& value) {
        if (n == 0)
        {
            return iterator(const_cast<pointer>(pos.ptr_));
        }

        size_type index = pos.ptr_ - data_;

        if (size_ + n > capacity_)
        {
            size_type new_capacity = calculate_growth(size_ + n);
            pointer   new_data = allocate(new_capacity);
            size_type new_size = 0;

            try
            {
                for (size_type i = 0; i < index; ++i)
                {
                    new (new_data + new_size) T(std::move(data_[i]));
                    ++new_size;
                }

                for (size_type i = 0; i < n; ++i)
                {
                    new (new_data + new_size) T(value);
                    ++new_size;
                }

                for (size_type i = index; i < size_; ++i)
                {
                    new (new_data + new_size) T(std::move(data_[i]));
                    ++new_size;
                }
            } catch (...)
            {
                destroy_range(new_data, new_data + new_size);
                deallocate(new_data, new_capacity);
                throw;
            }

            destroy_range(data_, data_ + size_);
            deallocate();

            data_ = new_data;
            size_ = new_size;
            capacity_ = new_capacity;
        } else {
            shift_right(index, n);
            try
            {
                for (size_type i = 0; i < n; ++i)
                {
                    new (data_ + index + i) T(value);
                }
            } catch (...)
            {
                size_type constructed = 0;
                while (constructed < n && data_ + index + constructed != nullptr)
                {
                    ++constructed;
                }
                destroy_range(data_ + index, data_ + index + constructed);
                shift_left(index + n, n);
                throw;
            }
            size_ += n;
        }

        return iterator(data_ + index);
    }

    template <typename T>
    template <typename InputIt, typename>
    typename my_vector<T>::iterator my_vector<T>::insert(const_iterator pos, InputIt first, InputIt last) {
        if (first == last)
        {
            return iterator(const_cast<pointer>(pos.ptr_));
        }

        size_type index = pos.ptr_ - data_;
        my_vector tmp(first, last);
        size_type numElements = tmp.size();

        if (size_ + numElements > capacity_)
        {
            size_type new_capacity = calculate_growth(size_ + numElements);
            pointer   new_data = allocate(new_capacity);
            size_type new_size = 0;

            try
            {
                for (size_type i = 0; i < index; ++i)
                {
                    new (new_data + new_size) T(std::move(data_[i]));
                    ++new_size;
                }

                for (size_type i = 0; i < numElements; ++i)
                {
                    new (new_data + new_size) T(std::move(tmp.data_[i]));
                    ++new_size;
                }

                for (size_type i = index; i < size_; ++i)
                {
                    new (new_data + new_size) T(std::move(data_[i]));
                    ++new_size;
                }
            } catch (...)
            {
                destroy_range(new_data, new_data + new_size);
                deallocate(new_data, new_capacity);
                throw;
            }

            destroy_range(data_, data_ + size_);
            deallocate();

            data_ = new_data;
            size_ = new_size;
            capacity_ = new_capacity;
        } else {
            shift_right(index, numElements);
            try
            {
                for (size_type i = 0; i < numElements; ++i)
                {
                    new (data_ + index + i) T(std::move(tmp.data_[i]));
                }
            } catch (...)
            {
                size_type constructed = 0;
                while (constructed < numElements && data_ + index + constructed != nullptr)
                {
                    ++constructed;
                }
                destroy_range(data_ + index, data_ + index + constructed);
                shift_left(index + numElements, numElements);
                throw;
            }
            size_ += numElements;
        }

        return iterator(data_ + index);
    }

    template <typename T>
    typename my_vector<T>::iterator my_vector<T>::insert(const_iterator pos, std::initializer_list<T> ilist) {
        return insert(pos, ilist.begin(), ilist.end());
    }

    template <typename T>
    typename my_vector<T>::iterator my_vector<T>::erase(const_iterator pos) {
        return erase(pos, pos + 1);
    }

    template <typename T>
    typename my_vector<T>::iterator my_vector<T>::erase(const_iterator first, const_iterator last) {
        size_type index_first = first.ptr_ - data_;
        size_type index_last = last.ptr_ - data_;
        size_type count = index_last - index_first;

        if (count == 0)
        {
            return iterator(data_ + index_first);
        }

        destroy_range(data_ + index_first, data_ + index_last);

        for (size_type i = index_last; i < size_; ++i)
        {
            try
            { new (data_ + i - count) T(std::move(data_[i])); } catch (...)
            {
                size_ = i - count;
                throw;
            }
            data_[i].~T();
        }

        size_ -= count;
        return iterator(data_ + index_first);
    }

    template <typename T>
    void my_vector<T>::push_back(const T& value) {
        T value_copy(value);
        emplace_back(std::move(value_copy));
    }

    template <typename T>
    void my_vector<T>::push_back(T&& value) {
        emplace_back(std::move(value));
    }

    template <typename T>
    template <typename... Args>
    typename my_vector<T>::reference my_vector<T>::emplace_back(Args&&... args) {
        if (size_ == capacity_)
        {
            size_type new_capacity = calculate_growth(size_ + 1);
            pointer   new_data = allocate(new_capacity);
            size_type new_size = 0;

            try
            {
                new (new_data + size_) T(std::forward<Args>(args)...);
                for (size_type i = 0; i < size_; ++i)
                {
                    new (new_data + i) T(std::move(data_[i]));
                    ++new_size;
                }
                ++new_size;
            } catch (...)
            {
                if (new_data + size_ != nullptr)
                {
                    (new_data + size_)->~T();
                }
                destroy_range(new_data, new_data + new_size);
                deallocate(new_data, new_capacity);
                throw;
            }

            destroy_range(data_, data_ + size_);
            deallocate();

            data_ = new_data;
            size_ = new_size;
            capacity_ = new_capacity;
        } else {
            try
            { new (data_ + size_) T(std::forward<Args>(args)...); } catch (...)
            { throw; }
            ++size_;
        }

        return back();
    }

    template <typename T>
    void my_vector<T>::pop_back() {
        if (size_ > 0)
        {
            --size_;
            data_[size_].~T();
        }
    }

    template <typename T>
    void my_vector<T>::resize(size_type new_size) {
        resize(new_size, T());
    }

    template <typename T>
    void my_vector<T>::resize(size_type new_size, const T& value) {
        if (new_size > size_)
        {
            if (new_size > capacity_)
            {
                size_type new_capacity = calculate_growth(new_size);
                reallocate(new_capacity);
            }

            for (size_type i = size_; i < new_size; ++i)
            {
                try
                { new (data_ + i) T(value); } catch (...)
                {
                    for (size_type j = size_; j < i; ++j)
                    {
                        data_[j].~T();
                    }
                    throw;
                }
            }
            size_ = new_size;
        } else if (new_size < size_) {
            for (size_type i = new_size; i < size_; ++i)
            {
                data_[i].~T();
            }
            size_ = new_size;
        }
    }

    template <typename T>
    void my_vector<T>::assign(size_type n, const T& value) {
        clear();

        if (n > capacity_)
        {
            deallocate();
            data_ = allocate(n);
            capacity_ = n;
        }

        for (size_type i = 0; i < n; ++i)
        {
            try
            { new (data_ + i) T(value); } catch (...)
            {
                destroy_range(data_, data_ + i);
                size_ = 0;
                throw;
            }
        }

        size_ = n;
    }

    template <typename T>
    template <typename InputIt, typename>
    void my_vector<T>::assign(InputIt first, InputIt last) {
        clear();

        size_type count = 0;
        for (InputIt it = first; it != last; ++it)
        {
            ++count;
        }

        if (count > capacity_)
        {
            deallocate();
            data_ = allocate(count);
            capacity_ = count;
        }

        size_type index = 0;
        for (InputIt it = first; it != last; ++it, ++index)
        {
            try
            { new (data_ + index) T(*it); } catch (...)
            {
                destroy_range(data_, data_ + index);
                size_ = 0;
                throw;
            }
        }

        size_ = count;
    }

    template <typename T>
    void my_vector<T>::assign(std::initializer_list<T> ilist) {
        assign(ilist.begin(), ilist.end());
    }

    template <typename T>
    void my_vector<T>::swap(my_vector& other) noexcept {
        using std::swap;
        swap(data_, other.data_);
        swap(size_, other.size_);
        swap(capacity_, other.capacity_);
    }

    // spaceship operator
    template <typename T>
    auto my_vector<T>::operator<=>(const my_vector& other) const {
        if (size_ != other.size_)
        {
            return size_ <=> other.size_;
        }

        for (size_type i = 0; i < size_; ++i)
        {
            if (auto cmp = data_[i] <=> other.data_[i]; cmp != 0)
            {
                return cmp;
            }
        }

        return std::strong_ordering::equal;
    }

    // Helper method implementations
    template <typename T>
    my_vector<T>::pointer my_vector<T>::allocate(size_type n) {
        if (n == 0)
        {
            return nullptr;
        }

        return static_cast<pointer>(::operator new(n * sizeof(T)));
    }

    template <typename T>
    void my_vector<T>::deallocate() {
        if (data_)
        {
            ::operator delete(data_);
            data_ = nullptr;
            capacity_ = 0;
        }
    }

    template <typename T>
    void my_vector<T>::deallocate(pointer ptr, size_type n) {
        if (ptr)
        {
            ::operator delete(ptr);
        }
    }

    template <typename T>
    void my_vector<T>::destroy_range(pointer first, pointer last) {
        for (pointer ptr = first; ptr != last; ++ptr)
        {
            ptr->~T();
        }
    }

    template <typename T>
    void my_vector<T>::allocate_and_fill(size_type n, const T& value) {
        data_ = allocate(n);
        capacity_ = n;

        try
        {
            for (size_type i = 0; i < n; ++i)
            {
                new (data_ + i) T(value);
                ++size_;
            }
        } catch (...)
        {
            destroy_range(data_, data_ + size_);
            deallocate();
            size_ = 0;
            throw;
        }
    }

    template <typename T>
    template <typename InputIt>
    void my_vector<T>::allocate_and_copy(InputIt first, InputIt last) {
        size_type count = std::distance(first, last);

        if (count > 0)
        {
            data_ = allocate(count);
            capacity_ = count;

            try
            {
                for (InputIt it = first; it != last; ++it, ++size_)
                {
                    new (data_ + size_) T(*it);
                }
            } catch (...)
            {
                destroy_range(data_, data_ + size_);
                deallocate();
                size_ = 0;
                throw;
            }
        }
    }

    template <typename T>
    void my_vector<T>::reallocate(size_type new_capacity) {
        pointer   new_data = allocate(new_capacity);
        size_type new_size = 0;

        try
        {
            for (size_type i = 0; i < size_; ++i)
            {
                new (new_data + i) T(std::move(data_[i]));
                ++new_size;
            }
        } catch (...)
        {
            destroy_range(new_data, new_data + new_size);
            deallocate(new_data, new_capacity);
            throw;
        }

        destroy_range(data_, data_ + size_);
        deallocate();

        data_ = new_data;
        size_ = new_size;
        capacity_ = new_capacity;
    }

    template <typename T>
    void my_vector<T>::reallocate_insert(size_type index, const T& value, size_type new_capacity) {
        pointer   new_data = allocate(new_capacity);
        size_type new_size = 0;

        try
        {
            for (size_type i = 0; i < index; ++i)
            {
                new (new_data + new_size) T(std::move(data_[i]));
                ++new_size;
            }

            new (new_data + new_size) T(value);
            ++new_size;

            for (size_type i = index; i < size_; ++i)
            {
                new (new_data + new_size) T(std::move(data_[i]));
                ++new_size;
            }
        } catch (...)
        {
            destroy_range(new_data, new_data + new_size);
            deallocate(new_data, new_capacity);
            throw;
        }

        destroy_range(data_, data_ + size_);
        deallocate();

        data_ = new_data;
        size_ = new_size;
        capacity_ = new_capacity;
    }

    template <typename T>
    void my_vector<T>::shift_right(size_type index, size_type count) {
        if (size_ + count > capacity_)
        {
            throw std::length_error("my_vector::shift_right: capacity exceeded");
        }

        for (size_type i = size_; i > index; --i)
        {
            try
            { new (data_ + i + count - 1) T(std::move(data_[i - 1])); } catch (...)
            {
                for (size_type j = size_; j > i; --j)
                {
                    (data_ + j + count - 1)->~T();
                }
                throw;
            }
            data_[i - 1].~T();
        }
    }

    template <typename T>
    void my_vector<T>::shift_left(size_type index, size_type count) {
        for (size_type i = index; i < size_; ++i)
        {
            data_[i - count].~T();
            try
            { new (data_ + i - count) T(std::move(data_[i])); } catch (...)
            {
                // Leave the vector in a consistent state
                size_ = i - count;
                throw;
            }
            data_[i].~T();
        }
    }

    template <typename T>
    my_vector<T>::size_type my_vector<T>::calculate_growth(size_type new_size) const {
        const size_type max_sz = max_size();

        if (new_size > max_sz)
        {
            throw std::length_error("my_vector::calculate_growth: maximum size exceeded");
        }

        if (capacity_ == 0)
        {
            return std::max<size_type>(1, new_size);
        }

        // TODO: Magic number? -- replace?
        size_type new_capacity = capacity_ + (capacity_ / 2);

        if (new_capacity < capacity_)
        {
            new_capacity = max_sz;
        }

        return std::max(new_capacity, new_size);
    }

    // Non-member functions
    template <typename T>
    void swap(my_vector<T>& lhs, my_vector<T>& rhs) noexcept {
        lhs.swap(rhs);
    }

}; // namespace myVector

#endif // MY_VECTOR_H
