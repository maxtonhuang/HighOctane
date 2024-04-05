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

/******************************************************************************
*
*	@brief Workers to carry out tasks
*
*	The workers carry out any available tasks in the task queue, and moves on
*   to the next task if any, or waits for new tasks if there are no queued
*   tasks, once the current task is complete.
*
******************************************************************************/
void ThreadPool::WorkerFunction() {
    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL); // turned off for now
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

/******************************************************************************
*
*	@brief Sets up Thread Pool
*
*	Sets up a pool of threads equal to half the number of logical cores in the
*   CPU. Usually this is equal to the number of physical cores in modern
*   systems. Threads will remain open until the Thread pool is terminated or
*   until the program exits, whichever comes first.
*
******************************************************************************/
ThreadPool::ThreadPool() {
    size_t numThreads = (std::thread::hardware_concurrency() - 1);
    for (size_t i = 0; i < numThreads; ++i) {
        workers.emplace_back(&ThreadPool::WorkerFunction, this);
    }
}

/******************************************************************************
*
*	@brief Places any new tasks in the task queue
*
*	Places any new tasks in the task queue. The queue's mutex is lock to
*   prevent multiple workers from accessing the same task.
* 
*   Example Usage:
*   \code
*   double sum[3];
*   double a[3] = { 1.5, 2.5, 3.5 };
*   double b[3] = { 3.2, 3.3, 3.4 };
*   for (int i = 0; i < 3; ++i) {
*       ThreadPool::threadPool().enqueue([i, &sum, &a, &b]() {     // lambda function capture list within []
*           sum[i] = a[i] + b[i];
*       });
*   }
*   std::cout << "The average value is " << ((sum[0] + sum[1] + sum[2]) / 3.0) << std::endl;
*   \endcode
*
******************************************************************************/
void ThreadPool::Enqueue(std::function<void()> task) {
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        tasks.push(task);
        active_tasks++;
    }
    condition.notify_one(); // Notify a waiting worker thread
}

/******************************************************************************
*
*	@brief Forces main thread to wait for all tasks to complete
*
*	Calling this function forces the main thread to wait for all tasks to
*   complete before moving on from that point in the code. This should only be
*   called if the next lines fo code require the data that is currently being
*   processed by the workers.
*
******************************************************************************/
void ThreadPool::WaitForAllTasks() {
    std::unique_lock<std::mutex> lock(queue_mutex);
    main_condition.wait(lock, [this]() {
        return active_tasks == 0;
        });
}

/******************************************************************************
*
*	@brief Joins and ends all threads in the Thread Pool
*
*	This function is called when the destructor of the Thread Pool is called.
*   It joins all threads to the main thread, ending all threads in the Thread
*   Pool.
*
******************************************************************************/
void ThreadPool::Shutdown() {
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

/******************************************************************************
*
*	@brief Destructor of the Thread Pool
*
*	-
*
******************************************************************************/
ThreadPool::~ThreadPool() {
    Shutdown();
}
