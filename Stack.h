#pragma once
#include <cstddef>
#include <sstream>
#include <string>

template <typename T>
class Stack {
public:
    Stack() : data_(new T[2]), capacity_(2), size_(0) {}
    ~Stack() { delete[] data_; }

    Stack(const Stack&) = delete;
    Stack& operator=(const Stack&) = delete;

    void push(const T& value) {
        if (size_ == capacity_) grow();
        data_[size_++] = value;
    }

    bool pop(T& value) {
        if (size_ == 0) return false;
        value = data_[--size_];
        return true;
    }
    bool peek(T& value) const {
        if (size_ == 0) return false;
        value = data_[size_ - 1];
        return true;
    }
    bool isEmpty() const { return size_ == 0; }
    std::size_t size() const { return size_; }
    std::size_t capacity() const { return capacity_; }

    std::string toString() const {
        std::ostringstream out;
        out << "[";
        for (std::size_t i = 0; i < size_; ++i) out << (i ? ", " : "") << data_[i];
        out << "]";
        return out.str();
    }

private:
    void grow() {
        std::size_t newCap = capacity_ * 2;
        T* bigger = new T[newCap];
        for (std::size_t i = 0; i < size_; ++i) bigger[i] = data_[i];
        delete[] data_;
        data_ = bigger;
        capacity_ = newCap;
    }

    T* data_;
    std::size_t capacity_;
    std::size_t size_;
};
