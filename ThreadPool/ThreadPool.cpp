#include "ThreadPool.h"
#include <iostream>

using namespace std;

ThreadPool::ThreadPool() : m_thread_count(thread::hardware_concurrency() != 0 ? thread::hardware_concurrency() : 3) , m_thread_queues(m_thread_count) , m_index(0) {}

void ThreadPool::start() {
   cerr << "TP: start called, threads=" << m_thread_count << endl;
   for(int i=0;i<m_thread_count; i++) {
       m_threads.emplace_back(&ThreadPool::threadFunc, this, i);
   }
}

void ThreadPool::stop() {
   cerr << "TP: stop called" << endl;
   for(int i=0;i<m_thread_count; i++) {
       // put dummy tasks into the queues of all threads
       // for completing the threads
       task_type empty_task;
       m_thread_queues[i].push(empty_task);
   }
   cerr << "TP: stop pushed all dummies" << endl;
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
    cerr << "TP: task pushed to queue " << (m_index - 1) % m_thread_count << endl;
}


void ThreadPool::threadFunc(int qindex) {
    cerr << "TP: thread " << qindex << " started" << endl;
    while(true) {
        cerr << "TP: thread " << qindex << " loop-iteration" << endl;
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

        if(!task_found) {
            // No tasks available -> block until stop() pushes a dummy task into this queue
            m_thread_queues[qindex].pop(task_to_do);
        }

        if(!task_to_do) {
            // Empty task received -> dummy shutdown task -> stop this thread
            cerr << "TP: thread " << qindex << " exiting (dummy)" << endl;
            return;
        }

        cerr << "TP: thread " << qindex << " executing task" << endl;
        // complete the task
        task_to_do();
        cerr << "TP: thread " << qindex << " task done" << endl;
        cerr << "TP: thread " << qindex << " loop-iteration-done" << endl;
    }
}
