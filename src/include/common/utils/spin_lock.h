#ifndef COMMON_UTILS_SPIN_LOCK_H
#define COMMON_UTILS_SPIN_LOCK_H

#include <atomic>
#include <thread>

namespace puzzle {

class SpinLock {
public:
    SpinLock() = default;
    ~SpinLock() = default;

    void lock() {
        while (flag_.test_and_set(std::memory_order_acquire)) {
            std::this_thread::yield();
        }
    }

    void unlock() {
        flag_.clear(std::memory_order_release);
    }
private:
    std::atomic_flag flag_ = ATOMIC_FLAG_INIT;
};

}  // namespace puzzle

#endif  // COMMON_UTILS_SPIN_LOCK_H