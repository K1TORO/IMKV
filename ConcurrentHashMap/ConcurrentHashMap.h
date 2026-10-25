#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include <shared_mutex>
#include <atomic>

using namespace std;

template <typename Key, typename Value>
class ConcurrentHashMap {
    public:
    
    void SET(const Key& key, const Value& value) {
        unique_lock<shared_mutex> lock(m_mutex);
        m_map[key] = value;
    }

    bool GET(const Key& key, Value& value) {
        shared_lock<shared_mutex> lock(m_mutex);
        auto it = m_map.find(key);
        if (it != m_map.end()) {
            value = it->second;
            return true;
        }
        return false;
    }

    void INCR(const Key& key, const Value& value) {
        unique_lock<shared_mutex> lock(m_mutex);
        m_map[key] += value;
    }
    
    void DECR(const Key& key, const Value& value) {
        unique_lock<shared_mutex> lock(m_mutex);
        m_map[key] -= value;
    }

    bool DELETE(const Key& key) {
        unique_lock<shared_mutex> lock(m_mutex);
        return m_map.erase(key) > 0;
    }

    void RANGE_QUERY(const Key& start_key, const Key& end_key, vector<pair<Key, Value>>& result) {
        shared_lock<shared_mutex> lock(m_mutex);
        for (const auto& pair : m_map) {
            if (pair.first >= start_key && pair.first <= end_key) {
                result.push_back(pair);
            }
        }
    }

    void CLEAR() {
        unique_lock<shared_mutex> lock(m_mutex);
        m_map.clear();
    }

    void count_processed_operations() {
        m_processed_operations.fetch_add(1, memory_order_relaxed);
    }

    long long get_amount_of_processed_operations(){
        return m_processed_operations;
    }

    private:
    shared_mutex m_mutex;
    unordered_map<Key, Value> m_map;
    atomic<long long> m_processed_operations{0};
};