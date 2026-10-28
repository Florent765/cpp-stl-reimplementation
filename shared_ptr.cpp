#include <header.h>

class ControlBlock {
private:
    std::atomic<size_t> strong_count = 1;
    // size_t weak_count;
public:
    void increment() noexcept {
        strong_count.fetch_add(1, std::memory_order_relaxed);
    }

    size_t decrement() noexcept {
        assert(strong_count);
        return strong_count.fetch_sub(1, std::memory_order_acq_rel);
    }

    [[nodiscard]] size_t get_strong_count() const noexcept {
        return strong_count;
    }
};

template <typename T>
class sp {
public:
    explicit sp() noexcept = default;

    // Delays allocating heap memory for the control block
    explicit sp(std::nullptr_t) noexcept = default;

    explicit sp(T* in_ptr) : underlying_ptr(in_ptr) {
        cb = new ControlBlock{};
    }

    sp(const sp& other) noexcept : underlying_ptr(other.underlying_ptr), cb(other.cb) {
        copy_from(other);
    }

    sp(sp&& other) noexcept {
        move_from(std::move(other));
    }

    sp& operator=(sp other) noexcept {
        swap(other); 
        return *this;
    }

    ~sp() {
        try_release();
    }

private:
    void copy_from(const sp& other) noexcept {
        if (other.cb) {
            other.cb->increment();
        }
    }

    void swap(sp& other) noexcept {
        std::swap(underlying_ptr, other.underlying_ptr);
        std::swap(cb, other.cb);
    }

    void try_release() {
        cb->decrement();
        if (strong_count == 0) {
            delete underlying_ptr;
            delete cb;
        }
    }
};