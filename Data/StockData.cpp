#include "StockData.h"
#include <vector>
#include <string>
#include <sstream>
#include <random>
#include <iomanip>
#include <chrono>
#include <ctime>
#include <mutex>
#include <algorithm>

static vector<string> dsck = {"GG","FPT","SS","AP","LN"};
static mutex ckMutex;

// Them ma co phieu
void addCK(const string& ck) {
    lock_guard<mutex> lock(ckMutex);
    if (find(dsck.begin(), dsck.end(), ck) == dsck.end()) {
        dsck.push_back(ck);
    }
}

// Xoa ma co phieu
void removeCK(const string& ck){
    lock_guard<mutex> lock(ckMutex);
    dsck.erase(remove(dsck.begin(), dsck.end(),ck), dsck.end());
}

// Tra ve time in the mm
string getCurrentTimeString() {
    auto now = chrono::system_clock::now();
    time_t now_time = chrono::system_clock::to_time_t(now);
    std::tm* timeinfo = std::localtime(&now_time);
    char buffer[9];
    std::strftime(buffer, sizeof(buffer), "%H:%M:%S", timeinfo);
    return std::string(buffer);
}

string stockData(){
    static default_random_engine generator(random_device{}());
    uniform_real_distribution<float> distribution(100.0f, 500.0f);
    ostringstream oss;
    oss <<"Time: " << getCurrentTimeString() <<"\n";
    oss <<"___________________________\n";
    lock_guard<mutex> lock(ckMutex);
    for (const auto& ck : dsck){
        double price = distribution(generator);
        oss << ck << ": " << fixed << setprecision(2) << price <<"$\n";
    }

    oss << "------------------\n";
    return oss.str();
}