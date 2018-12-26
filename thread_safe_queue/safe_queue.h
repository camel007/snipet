/**************************************************************************************************
 * @file   : safe_queue.h
 * @brief  : 线程安全队列
 * @author : zhangyao <yaozhang.chn@gmail.com>
 * @date   : 2018-09-15
 * @remarks:
 **************************************************************************************************/

#include <condition_variable>
#include <memory>
#include <mutex>
#include <queue>
#include <iostream>

template <class T> class SafeQueue
{
public:
        explicit SafeQueue(size_t capacity) : capacity_(capacity)
        {
        }
        SafeQueue(const SafeQueue &other) = delete;
        SafeQueue &operator=(const SafeQueue &other) = delete;

        bool empty() const
        {
                std::lock_guard<std::mutex> lock(mut_);
                return data_queue_.empty();
        }

        size_t size() const
        {
                std::lock_guard<std::mutex> lock(mut_);
                return data_queue_.size();
        }

        /// @brief 如果队列已满，先进入出队，然后新元素入队
        /// @parameter[in] value 待入队新元素
        /// @return 无
        void enqueue(T &value)
        {
                std::unique_lock<std::mutex> locker(mut_);
                if (data_queue_.size() > capacity_) {
                        data_queue_.pop_front();
                }
                data_queue_.push_back(value);
                locker.unlock();
                cond_var_.notify_all();
        }

        /// @brief 如果队列已满，阻塞线程，等待其他线程取走元素
        /// @parameter[in] value 待入队新元素
        /// @return 无
        void wait_and_enqueue(T &value)
        {
                std::unique_lock<std::mutex> locker(mut_);
                cond_var_.wait(locker, [this] { return data_queue_.size() < capacity_; });
                data_queue_.push_back(value);
                cond_var_.notify_all();
        }

        /// @brief 如果队列为空，直接返回false，不会阻塞线程
        /// @parameter[out] value 出队元素
        /// @return 如果队列中有元素出队，则返回true,否则返回flase
        bool dequeue(T &value)
        {
                std::unique_lock<std::mutex> locker(mut_);
                if (data_queue_.empty()) {
                        return false;
                }

                value = std::move(data_queue_.front());
                data_queue_.pop_front();
                locker.unlock();

                cond_var_.notify_all();

                return true;
        }

        /// @brief 如果队列为空，反馈nullptr，不会阻塞线程
        /// @return 如果队列中有元素，则返回出队元素，否则返回nullptr
        std::shared_ptr<T> dequeue()
        {
                std::unique_lock<std::mutex> locker(mut_);
                if (data_queue_.empty()) {
                        std::shared_ptr<T> res(nullptr);
                        return res;
                }

                std::shared_ptr<T> element = std::make_shared<T>(std::move(data_queue_.front()));
                data_queue_.pop_front();

                locker.unlock();

                cond_var_.notify_all();

                return element;
        }

        /// @brief 如果队列为空，则阻塞线程，等待其他线程入队
        /// @parameter[out] value 出队元素
        /// @return 无
        void wait_and_dequeue(T &value)
        {
                std::unique_lock<std::mutex> locker(mut_);
                cond_var_.wait(locker, [this] { return !data_queue_.empty(); });
                value = data_queue_.front();

                cond_var_.notify_all();

                data_queue_.pop_front();
        }

        /// @brief 如果队列为空，则阻塞线程，等待其他线程入队
        /// @return 出队元素
        std::shared_ptr<T> wait_and_dequeue()
        {
                std::unique_lock<std::mutex> locker(mut_);
                cond_var_.wait(locker, [this] { return !data_queue_.empty(); });

                std::shared_ptr<T> element = std::make_shared<T>(data_queue_.front());

                data_queue_.pop_front();

                cond_var_.notify_all();

                return element;
        }

private:
        std::mutex mut_;
        std::deque<T> data_queue_;
        std::condition_variable cond_var_;
        size_t capacity_;
};
