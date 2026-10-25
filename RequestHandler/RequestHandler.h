#pragma once
#include "../ThreadPool/ThreadPool.h"


class RequestHandler{
    public:
    RequestHandler();
    ~RequestHandler();
    void pushRequest(task_type f);

    private:
    ThreadPool m_tpool;
};
