#pragma once
#include "../RequestHandler/RequestHandler.h"
#include "../ConcurrentHashMap/ConcurrentHashMap.h"
#include <atomic>
#include <mutex>
#include <string>
#include <vector>
#include <utility>

class IMKV {
    public:
    IMKV();
    ~IMKV();
    void addTask(task_type f);
    void addSET(const std::string& key, int value);
    void addINCR(const std::string& key, int value);
    void addDECR(const std::string& key, int value);
    void addDELETE(const std::string& key);
    void addCLEAR();
    void addGET(const std::string& key);
    bool get(const std::string& key, int& value);
    void addRANGE_QUERY(const std::string& start_key, const std::string& end_key);
    std::vector<std::pair<std::string, int>> getRangeQuery(const std::string& start_key, const std::string& end_key);
    long long return_amount_of_processed_operations();

    private:
    ConcurrentHashMap<std::string, int> m_map;
    RequestHandler m_request_handler;
    std::mutex m_get_mutex;
    std::string m_get_key;
    bool m_get_found;
    int m_get_value;
    std::mutex m_range_mutex;
    std::vector<std::pair<std::string, int>> m_range_result;
};
