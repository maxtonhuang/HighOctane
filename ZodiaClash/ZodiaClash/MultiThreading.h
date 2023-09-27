#pragma once

#include <vector>
#include <thread>
#include <functional>
#include <queue>
#include <mutex>
#include <condition_variable>

class ThreadPool {
private:
    std::vector<std::thread> workers;
    std::queue<std::function<void()>> tasks;
    std::mutex queue_mutex;
    std::condition_variable condition;
    std::condition_variable main_condition;
    std::atomic<int> active_tasks{ 0 };
    bool stop = false;

    void worker_function();

    ThreadPool();
    ~ThreadPool();

public:

    static ThreadPool & threadPool() {
        static ThreadPool tp;
        return tp;
    }

    // Add a task to the thread pool
    void enqueue(std::function<void()> task);

    // Wait for all tasks to finish in the current cycle
    void WaitForAllTasks();


    // Shutdown the thread pool
    void shutdown();

    
};
