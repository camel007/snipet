#include "safe_queue.h"

#include <iostream>
#include <thread>
#include <chrono>

#include <unistd.h>

void func1(SafeQueue<int> &que)
{
        for (int i = 10; i > 0; i--) {
                // que.enqueue(i * i);
                que.enqueue(i);
                std::this_thread::sleep_for(std::chrono::seconds(1));
        }
}

int main()
{
        SafeQueue<int> que(1);

        std::thread th1(func1, std::ref(que));

        int data = 0;
        while (data != 1) {
                std::shared_ptr<int> value = que.wait_and_dequeue();
                if (value.get() == nullptr) {
                        std::cout << "empty" << std::endl;
                        std::this_thread::sleep_for(std::chrono::milliseconds(1));
                        continue;
                }
                data = *value;

                std::cout << data << std::endl;
        }

        th1.join();

        return 0;
}