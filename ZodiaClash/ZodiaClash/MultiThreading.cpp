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
*	@file		MultiThreading.cpp
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
*	This file contains the definitions of the class for ThreadPool which
*   creates a threadpool of half the number of logical processors in the
*   system. Threads will remain open until the program goes out of scope. Tasks
*   can be sent to a thread queue and executed by the workes when there are
*   available threads to work on the tasks. Lock guards and mutexes are used to
*   prevent race conditions in the event multiple threads are trying to update
*   a variable simultaneously. Conditional waits may be used to wait for the
*   tasks to finish processing before allowing the main thread to continue
*   since join() and detach() cannot be used bacause threads need to be kept
*   open.
*
******************************************************************************/

#include "MultiThreading.h"
#include <Windows.h>
/*
void ThreadPool::worker_function() {
    // SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL); // turned of for now
    while (true) {
        std::function<void()> task;
        {
            std::unique_lock<std::mutex> lock(queue_mutex);

            // Wait for a task to become available or for the stop signal
            condition.wait(lock, [this]() {
                return stop || !tasks.empty();
                });

            if (stop && tasks.empty()) {
                return; // Exit the thread
            }

            task = std::move(tasks.front());
            tasks.pop();
        }
        task(); // Execute the task
        active_tasks--;
        main_condition.notify_one(); // Notify main thread if necessary
    }
}


ThreadPool::ThreadPool() {
    size_t numThreads = (std::thread::hardware_concurrency() / 2);
    for (size_t i = 0; i < numThreads; ++i) {
        workers.emplace_back(&ThreadPool::worker_function, this);
    }
}

// Add a task to the thread pool
void ThreadPool::enqueue(std::function<void()> task) {
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        tasks.push(task);
        active_tasks++;
    }
    condition.notify_one(); // Notify a waiting worker thread
}

void ThreadPool::WaitForAllTasks() {
    std::unique_lock<std::mutex> lock(queue_mutex);
    main_condition.wait(lock, [this]() {
        return active_tasks == 0;
        });
}

// Shutdown the thread pool
void ThreadPool::shutdown() {
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        stop = true;
    }
    condition.notify_all(); // Notify all worker threads

    for (std::thread& worker : workers) {
        if (worker.joinable()) {
            worker.join();
        }
    }
}

ThreadPool::~ThreadPool() {
    shutdown();
}



/******************************************************************************

 format for enqueueing thread, with capture variables within [].
 
 for (int i = 0; i < 2; ++i) {
 ThreadPool::threadPool().enqueue([i, &cout_mutex]() { // capture cout_mutex by reference
    {
        std::lock_guard<std::mutex> lock(cout_mutex);
        std::cout << "Task " << i << " executed by thread " << std::this_thread::get_id() << std::endl;
    }
    });
 }
 ThreadPool::threadPool().WaitForAllTasks();

******************************************************************************/
