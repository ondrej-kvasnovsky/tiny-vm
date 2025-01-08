 #include <iostream>
 #include <atomic>
 #include <thread>

 std::atomic<int> sharedValue(0);  // Shared variable

 void casTest(int expected, const int newValue) {
     // Atomically compare and swap
     bool result = sharedValue.compare_exchange_strong(expected, newValue);

     if (result) {
         std::cout << "Thread " << std::this_thread::get_id()
                   << " swapped value to " << newValue << "\n";
     } else {
         std::cout << "Thread " << std::this_thread::get_id()
                   << " failed to swap, current value is " << sharedValue.load() << "\n";
     }
 }

 void threadFunction() {
     int expected = 0;
     for (int i = 0; i < 5; ++i) {
         int newValue = expected + 1;
         casTest(expected, newValue);
         expected = sharedValue.load();
     }
 }

 int main() {
     // Start multiple threads
     std::vector<std::thread> threads;
     for (int i = 0; i < 3; ++i) {
         threads.push_back(std::thread(threadFunction));
     }

     for (auto& t : threads) {
         t.join();
     }

     return 0;
 }
