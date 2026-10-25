#include "IMKV.h"
#include <iostream>

using namespace std;

IMKV::IMKV() {}

IMKV::~IMKV() {
    return_amount_of_processed_operations();
}

void IMKV::addTask(task_type f){
    m_request_handler.pushRequest(f);
}

void IMKV::addSET(const std::string& key, int value){
    task_type task = [this, key, value] {
        m_map.SET(key, value);
        m_map.count_processed_operations();
    };
    m_request_handler.pushRequest(task);
}

void IMKV::addINCR(const std::string& key, int value){
    task_type task = [this, key, value] {
        m_map.INCR(key, value);
        m_map.count_processed_operations();
    };
    m_request_handler.pushRequest(task);
}

void IMKV::addDECR(const std::string& key, int value){
    task_type task = [this, key, value] {
        m_map.DECR(key, value);
        m_map.count_processed_operations();
    };
    m_request_handler.pushRequest(task);
}

void IMKV::addDELETE(const std::string& key){
    task_type task = [this, key] {
        m_map.DELETE(key);
        m_map.count_processed_operations();
    };
    m_request_handler.pushRequest(task);
}

void IMKV::addCLEAR(){
    task_type task = [this] {
        m_map.CLEAR();
        m_map.count_processed_operations();
    };
    m_request_handler.pushRequest(task);
}

void IMKV::addGET(const std::string& key){
    task_type task = [this, key] {
        lock_guard<mutex> lk(m_get_mutex);
        bool found = m_map.GET(key, m_get_value);
        m_get_found = found;
        m_get_key = key;
        m_map.count_processed_operations();
    };
    m_request_handler.pushRequest(task);
}

bool IMKV::get(const std::string& key, int& value){
    lock_guard<mutex> lk(m_get_mutex);
    bool found = (m_get_key == key) && m_get_found;
    value = m_get_value;
    return found;
}

void IMKV::addRANGE_QUERY(const std::string& start_key, const std::string& end_key){
    task_type task = [this, start_key, end_key] {
        lock_guard<mutex> lk(m_range_mutex);
        m_range_result.clear();
        m_map.RANGE_QUERY(start_key, end_key, m_range_result);
        m_map.count_processed_operations();
    };
    m_request_handler.pushRequest(task);
}

vector<pair<string, int>> IMKV::getRangeQuery(const std::string& start_key, const std::string& end_key){
    lock_guard<mutex> lk(m_range_mutex);
    return m_range_result;
}

long long IMKV::return_amount_of_processed_operations(){
    return m_map.get_amount_of_processed_operations();
}
