#ifndef IMAGECONVERTTHREADPOOL_THREADPOOL_H
#define IMAGECONVERTTHREADPOOL_THREADPOOL_H

#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <queue>
#include <atomic>

class ThreadPool
{
private:
    std::vector<std::thread> workers;
    std::queue<std::function<void()> > tasks;

    std::mutex queue_mutex;
    std::condition_variable condition;
    std::condition_variable waitAllCondition;

    std::atomic<int> activeTasks{0};
    bool stop = false;

public:
    explicit ThreadPool(size_t threadCount = std::thread::hardware_concurrency());

    ~ThreadPool();

    ThreadPool(const ThreadPool &) = delete;

    ThreadPool &operator=(const ThreadPool &) = delete;

    template<class F, class... Args>
    auto enqueue(F &&f, Args &&... args)
        -> std::future<typename std::invoke_result<F, Args...>::type>
    {
        using return_type = typename std::invoke_result<F, Args...>::type;

        auto task = std::make_shared<std::packaged_task<return_type()> >(
            std::bind(std::forward<F>(f), std::forward<Args>(args)...)
        );

        std::future<return_type> res = task->get_future(); {
            std::lock_guard<std::mutex> lock(queue_mutex);
            if (stop)
                throw std::runtime_error("enqueue on stopped ThreadPool");

            tasks.emplace([this, task]()
            {
                (*task)();
            });
        }

        condition.notify_one();
        return res;
    }

    void WaitAll();
};

#endif
