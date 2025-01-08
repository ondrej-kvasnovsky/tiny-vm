//
// Created by Ondrej Kvasnovsky on 1/5/25.
//

#include "memory.h"
#include <iostream>
#include <atomic>
#include <thread>

std::atomic<int> x(0);
std::atomic<int> y(0);

void thread1() {
    x.store(1, std::memory_order_relaxed); // Store x
    std::atomic_thread_fence(std::memory_order_release);  // Release memory barrier
    y.store(1, std::memory_order_relaxed); // Store y
}

void thread2() {
    while (y.load(std::memory_order_relaxed) == 0);  // Wait until y is 1
    std::atomic_thread_fence(std::memory_order_acquire); // Acquire memory barrier
    if (x.load(std::memory_order_relaxed) == 1) {
        std::cout << "Thread 2 observed x as 1\n";
    }
}

int main() {
    std::thread t1(thread1);
    std::thread t2(thread2);

    t1.join();
    t2.join();

    return 0;
}
