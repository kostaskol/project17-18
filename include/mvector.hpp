#ifndef MY_VECTOR_H
#define MY_VECTOR_H

#include <iostream>
#include <cstdlib>
#include <sstream>
#include "logger.hpp"

// Simple resizable array template class
// that includes some of std::vector's basic operations


// NOTE: mstd::vector<T*> takes ownership of the passed pointers instead of copying them
// That means that if the pointers are deleted elsewhere, the program will *hopefully* crash
namespace mstd {
    template <typename T>
    class vector {
    private:
        size_t _size{};
        size_t _capacity;
        T *_entries;

        void _enlarge() {
            auto *tmp = new T[_capacity << 1];
            std::copy(_entries, _entries + _size, tmp);

            delete[] _entries;
            _entries = tmp;

            _capacity = _capacity << 1;
        }

        void _swap(T *a, T *b) {
            T c = *a;
            *a = *b;
            *b = c;
        }

        int _partition(T *arr, int low, int high)
        {
            int pivot = arr[high];    // pivot
            int i = (low - 1);  // Index of smaller element

            for (int j = low; j <= high- 1; j++)
            {
                // If current element is smaller than or
                // equal to pivot
                if (arr[j] <= pivot)
                {
                    i++;    // increment index of smaller element
                    _swap(&arr[i], &arr[j]);
                }
            }
            _swap(&arr[i + 1], &arr[high]);
            return (i + 1);
        }


        void _quick_sort(int arr[], int low, int high)
        {
            if (low < high)
            {
                int pi = _partition(arr, low, high);

                _quick_sort(arr, low, pi - 1);
                _quick_sort(arr, pi + 1, high);
            }
        }
    public:
        explicit vector(size_t capacity = 1)
                : _size(0), _capacity(capacity) {
            _entries = new T[_capacity];
        }

        vector(const vector &other)
                : _size(other._size),
                  _capacity(other._capacity) {
            _entries = new T[_capacity];
            std::copy(other._entries, other._entries + other._size, _entries);
        }

        vector(const vector &other, size_t start, size_t end) {
            if (start < 0 || end > other._size) return;
            _capacity = other._capacity;
            _size = (size_t) end - start;
            _entries = new T[_capacity];
            std::copy(other._entries + start, other._entries + end, _entries);
        }

        vector(vector &&other) noexcept : vector() {
            // other object is swapped with this (which has been instantiated and is thus safe to delete)
            _swap_vectors(*this, other);
        }

        ~vector() {
            delete[] _entries;
        }

        vector sublist(size_t start, size_t length) const {
            if (start + length > _size) {
                throw std::runtime_error("Bad indices");
            }

            vector tmp;
            for (auto i = (size_t) start; i <= (size_t) start + length; i++) {
                tmp.push(_entries[i]);
            }

            return tmp;
        }

        void push(const T &ent) {
            if (_size + 1>= (size_t) _capacity) {
                _enlarge();
            }

            _entries[_size++] = T(ent);
        }

        T *m_push(T &ent) {
            if (_size + 1 >= (size_t) _capacity) _enlarge();

            _entries[_size] = std::move(ent);
            return &_entries[_size++];
        }

        void add(const T &ent) {
            push(ent);
        }

        T *m_insert_at(int index, T &ent) {
            if ((_size == 0) || (index == _size)) {
                return m_push(ent);
            }

            if (_size + 1 >= _capacity) _enlarge();

            for (size_t i = _size - 1; (int) i >= index; i--) {
                _entries[i + 1] = std::move(_entries[i]);
            }

            _entries[index] = std::move(ent);
            _size++;
            return &_entries[index];
        }

        void shrink_to_size() {
            auto *tmp = new T[_size];
            for (size_t i = 0; i < _size; i++) {
                tmp[i] = _entries[i];
            }
            delete[] _entries;
            _entries = tmp;
            _capacity = _size;
        }

        bool in(const T &ent) const {
            for (size_t i = 0; i < _size; i++) {
                if (_entries[i] == ent) {
                    return true;
                }
            }
            return false;
        }

        T &at(size_t index) const {
            if (index >= (int) _size) {
                throw std::runtime_error("Index out of range:");
            }

            return _entries[index];
        }

        // Method sugar
        T &get(size_t index) const {
            return at(index);
        }



        T *at_p(size_t index) {
            if (index >= _size) {
                throw std::runtime_error("Index out of range");
            }

            return &_entries[index];
        }

        // Method sugar
        T *get_p(size_t index) {
            return at_p(index);
        }

        int get_index(const T &ent) const {
            for (size_t i = 0; i < _size; i++) {
                if (_entries[i] == ent) return (int) i;
            }
            return -1;
        }

        void clear(size_t new_cap = 1) {
            delete[] _entries;
            _size = 0;

            _capacity = new_cap;
            _entries = new T[_capacity];
        }

        void set_at(size_t index, const T &ent) {
            if (index < 0 || index > _size) {
                throw std::runtime_error("Bad index: " + index);
            }

            _entries[index] = T(ent);
        }


        bool remove(const T &ent) {
            auto *tmp = new T[_capacity];
            int j = 0;
            bool found = false;

            for (size_t i = 0; i < _size; i++) {
                if (_entries[i] == ent) {
                    found = true;
                    continue;
                }

                tmp[j++] = std::move(_entries[i]);
            }

            if (found) {
                delete[] _entries;
                _entries = tmp;

                _size--;
            } else {
                delete[] tmp;
            }

            return found;
        }

        // Iterators. Allows for foreach loops
        // (for (int i : vec))
        T *begin() {
            return &_entries[0];
        }

        T *end() {
            return &_entries[_size];
        }

        void remove_at(size_t index) {
            if (index >= _size || index < 0) {
                throw std::runtime_error("Bad index: " + std::to_string(index));
            }
            for (size_t i = index + 1; i < _size; i++) {
                _entries[i - 1] = std::move(_entries[i]);
            }

            _size--;
        }

        void sort() {
            _quick_sort(_entries, 0, _size - 1);
        }

        size_t size() const { return _size; }


        T get_cpy(size_t index) const {
            if (index >= _size) {
                throw std::runtime_error("Index out of range");
            }

            return _entries[index];
        }

        T &operator[](size_t index) const {
            return at(index);
        }

        // Copy operator=
        vector &operator=(const vector &other) {
            vector tmp(other);
            _swap_vectors(*this, tmp);

            return *this;
        }

        // Move operator=
        vector &operator=(vector &&other) noexcept {
            _swap_vectors(*this, other);
        }

        friend void _swap_vectors(vector &v1, vector &v2) {
            using std::swap;
            swap(v1._size, v2._size);
            swap(v1._capacity, v2._capacity);
            swap(v1._entries, v2._entries);
        }

    };
}

#endif //MY_VECTOR_H
