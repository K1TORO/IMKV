#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include "./IMKV/IMKV.h"
using namespace std;

int main() {
    IMKV *imkv = new IMKV;
    imkv->addSET("key", 42);
    imkv->addINCR("counter", 5);
    imkv->addDECR("counter", 2);
    imkv->addGET("key");
    int value = 0;
    bool found = imkv->get("key", value);
    cout << "GET result -> found: " << found << ", value: " << value << endl;
    imkv->addRANGE_QUERY("a", "z");
    vector<pair<string, int>> rangeResult = imkv->getRangeQuery("a", "z");
    cout << "RANGE_QUERY result size: " << rangeResult.size() << endl;
    imkv->addDELETE("key");
    imkv->addCLEAR();
    cout << "Processed operations: " << imkv->return_amount_of_processed_operations() << endl;
    delete imkv;
    return 0;
}
