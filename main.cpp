#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include "./IMKV/IMKV.h"
using namespace std;

int main() {
    IMKV *imkv = new IMKV;

    // All operations below are pushed to the IMKV thread pool as tasks
    // and executed asynchronously on pool threads, backed by the
    // ConcurrentHashMap built-in operations.

    // SET key = 42  (task -> ConcurrentHashMap::SET)
    imkv->addSET("key", 42);

    // INCR counter += 5 and DECR counter -= 2 (tasks -> ConcurrentHashMap::INCR / DECR)
    imkv->addINCR("counter", 5);
    imkv->addDECR("counter", 2);

    // GET key = 42 (task -> ConcurrentHashMap::GET)
    // The task delivers its result back through the synchronous getters.
    imkv->addGET("key");
    int value = 0;
    bool found = imkv->get("key", value);
    cout << "GET result -> found: " << found << ", value: " << value << endl;

    // RANGE_QUERY start..end = "a".."z" (task -> ConcurrentHashMap::RANGE_QUERY)
    imkv->addRANGE_QUERY("a", "z");
    vector<pair<string, int>> rangeResult = imkv->getRangeQuery("a", "z");
    cout << "RANGE_QUERY result size: " << rangeResult.size() << endl;

    // DELETE key = 42 and CLEAR the whole map (tasks -> ConcurrentHashMap::DELETE / CLEAR)
    imkv->addDELETE("key");
    imkv->addCLEAR();

    // Amount of processed operations reported by the thread pool tasks
    cout << "Processed operations: " << imkv->return_amount_of_processed_operations() << endl;

    // Destruction stops the thread pool and waits for all pending tasks to finish
    delete imkv;
    return 0;
}
