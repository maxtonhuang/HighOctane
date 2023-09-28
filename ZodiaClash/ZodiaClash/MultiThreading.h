/******************************************************************************
*
*	\copyright
*		All content(C) 2023/2024 DigiPen Institute of Technology Singapore.
*		All rights reserved. Reproduction or disclosure of this file or its
*		contents without the prior written consent of DigiPen Institute of
*		Technology is prohibited.
*
* *****************************************************************************
*
*	@file		MultiThreading.h
*
*	@author		Maxton Huang Xinghua
*
*	@email		m.huang\@digipen.edu
*
*	@course		CSD 2401 - Software Engineering Project 3
*				CSD 2451 - Software Engineering Project 4
*
*	@section	Section A
*
*	@date		22 September 2023
*
* *****************************************************************************
*
*	@brief		Threadpool Class
*
*	This file contains the class for ThreadPool which creates a threadpool of
*   half the number of logical processors in the system. Threads will remain
*   open until the program goes out of scope. Tasks can be sent to a thread
*   queue and executed by the workes when there are available threads to work
*   on the tasks.
*
******************************************************************************/



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

    ~ThreadPool();
};
