#pragma once
#include <queue>
#include <condition_variable>
#include <vector>
#include <mutex>
#include <atomic>
#include <functional>
#include <thread>
#include "BlockedQueue.h"

using namespace std;

typedef std::function<void()> task_type;

class ThreadPool{
    public:
    ThreadPool();
    void start();
    void stop();
    void push_task(task_type f);
    void threadFunc(int qindex);
    private:
    int m_thread_count;
    vector<thread> m_threads;
    mutex m_locker;
    vector<BlockedQueue<task_type>> m_thread_queues;
    int m_index;
    condition_variable m_event_holder;
    atomic<bool> m_work;
};
