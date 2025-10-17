#include "../include/ThreadPool.h"
#include "../../Util/include/Common.h"
#include "../../ImageCoreProcessing/include/ConverterCore.h"

ThreadPool::ThreadPool(size_t _ThreadCount)
{
    for (size_t i = 0; i < _ThreadCount; ++i)
    {
        workers.emplace_back([this]() {
            while (true)
            {
                std::function<void()> task;

                {
                    std::unique_lock<std::mutex> lock(queue_mutex);
                    condition.wait(lock, [this]() {
                        return stop || !tasks.empty();
                    });

                    if (stop && tasks.empty())
                        return;

                    task = std::move(tasks.front());
                    tasks.pop();
                    activeTasks++;   // 작업 시작
                }

                task();
                {
                    std::lock_guard<std::mutex> lock(queue_mutex);
                    activeTasks--; // 작업 완료
                    if (tasks.empty() && activeTasks.load() == 0)
                        g_isProcessing = false;
                        waitAllCondition.notify_all(); // 모든 작업 끝났을 때
                }
            }
        });
    }
}

void ThreadPool::WaitAll()
{
    std::unique_lock<std::mutex> lock(queue_mutex);

    waitAllCondition.wait(lock, [this]() {
        return tasks.empty() && activeTasks.load() == 0;
    });

    Common::Logger("All tasks completed");
}

ThreadPool::~ThreadPool()
{
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        stop = true;
    }

    condition.notify_all();

    for (std::thread &worker : workers)
    {
        if (worker.joinable())
            worker.join();
    }

}