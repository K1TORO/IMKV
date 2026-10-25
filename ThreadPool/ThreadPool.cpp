#include "ThreadPool.h"
#include <iostream>

using namespace std;

ThreadPool::ThreadPool() : m_thread_count(thread::hardware_concurrency() != 0 ? thread::hardware_concurrency() : 3) , m_thread_queues(m_thread_count) , m_index(0) , m_work(false) {}

void ThreadPool::start() {
   for(int i=0;i<m_thread_count; i++) {
       m_threads.emplace_back(&ThreadPool::threadFunc, this, i);
   }
}

void ThreadPool::stop() {
   cerr << "TP: stop called" << endl;
   m_work = false;
   m_event_holder.notify_all();
   for(auto& t: m_threads) {
       t.join();
   }
   cerr << "TP: stop joined all" << endl;
}

void ThreadPool::push_task(task_type f){
    lock_guard<mutex> l(m_locker);
    task_type new_task = f;
    m_thread_queues[m_index % m_thread_count].push(new_task);
    m_index++;
    m_event_holder.notify_one();
}

void ThreadPool::threadFunc(int qindex) {
    while(true) {
        task_type task_to_do;
        bool task_found = false;
        int i = 0;
        while(i < m_thread_count) {
            // Try to steal a task from other threads' queues
            if(m_thread_queues[(qindex + i) % m_thread_count].fast_pop(task_to_do)) {
                task_found = true;
                break;
            }
            i++;
        }

        if(task_found) {
            cerr << "TP: thread " << qindex << " executing task" << endl;
            task_to_do();
            cerr << "TP: thread " << qindex << " task done" << endl;
            continue;
        }

        // No task available -> wait for a task on this thread's queue or until shutdown
        cerr << "TP: thread " << qindex << " waiting" << endl;
        {
            unique_lock<mutex> lk(m_locker);
            m_event_holder.wait(lk, [this, qindex]() {
                return !m_thread_queues[qindex].empty() || !m_work.load();
            });
        }
        cerr << "TP: thread " << qindex << " woke up" << endl;

        task_type task;
        if(!m_thread_queues[qindex].fast_pop(task)) {
            // queue is empty and shutdown was requested -> stop this thread
            cerr << "TP: thread " << qindex << " exiting (shutdown)" << endl;
            return;
        }
        task();
    }
}
