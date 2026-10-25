#include "RequestHandler.h"

RequestHandler::RequestHandler(){
    m_tpool.start();
}

RequestHandler::~RequestHandler(){
    m_tpool.stop();
}

void RequestHandler::pushRequest(task_type f){
    m_tpool.push_task(f);
}
