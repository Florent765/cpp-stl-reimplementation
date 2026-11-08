#include <iostream>
#include <new>
#include <memory>

template <typename T>
class Vector {
private:
  T* data_ = nullptr;
  size_t capacity_ = 0;
  size_t size_ = 0;

public:
  Vector() = default;

  Vector(const Vector& other) {
    T* buffer = static_cast<T*>(::operator new(other.capacity_ * sizeof(T)));

    for (size_t i = 0; i < other.size_; ++i)
      new (&buffer[i]) T(other.data_[i]);
    capacity_ = other.capacity_;
    size_ = other.size_;
    data_ = buffer;
  }

  Vector(Vector&& other) noexcept {
    capacity_ = other.capacity_;
    size_ = other.size_;
    data_ = other.data_;

    other.size_ = 0;
    other.capacity_ = 0;
    other.data_ = nullptr;
  }

  ~Vector() {
    clear();
    ::operator delete(data_, capacity_ * sizeof(T));
  }

  const T& operator[](size_t index) const {
    return data_[index];
  }

  T& operator[](size_t index) {
    return data_[index];
  }

  void push_back(const T& value) {
    if (size_ >= capacity_)
      reserve(capacity_ == 0 ? 1 : 2 * capacity_);
    
    new (&data_[size_]) T(value);
    ++size_;
  }

  template<typename... Args>
  T& emplace_back(Args&&... args) {
    if (size_ >= capacity_)
      reserve(capacity_ == 0 ? 1 : 2 * capacity_);
    
    new (&data_[size_]) T(std::forward<Args>(args)...);
    return data_[size_++];
  }

  void pop_back() {
    if (size_ > 0) {
      --size_;
      data_[size_].~T();
    }
  }

  void clear() {
    for (size_t i = 0; i < size_; ++i)
      data_[i].~T();

    size_ = 0;
  }

  void reserve(size_t capacity) {
    if (capacity <= capacity_)
      return;

    T* buffer = static_cast<T*>(::operator new(capacity * sizeof(T)));

    for (size_t i = 0; i < size_; ++i)
      new (&buffer[i]) T(std::move(data_[i]));

    for (size_t i = 0; i < size_; i++)
      data_[i].~T();

    ::operator delete(data_, capacity_ * sizeof(T));

    data_ = buffer;
    capacity_ = capacity;
  }

  size_t size() const noexcept {
    return size_;
  }

};




int main() {
  Vector<int> vector{};

  for (int i = 0; i < 8; ++i) {
    vector.push_back(i);
    std::cout << vector[i] << std::endl;
  }
  return 0;
}
