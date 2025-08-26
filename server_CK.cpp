#include <iostream>
#include <algorithm>
#include <thread>
#include <chrono>
#include <vector>
#include <string>
#include <sstream>
#include <random>
#include <mutex>
#include <ctime>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")

std::mutex mtx;

struct Stock {
    std::string code;
    double price;
    std::vector<std::pair<long long,double>> history;
};

std::vector<Stock> stocks = {
    {"GG", 120.5, {}},
    {"FPT", 89.3, {}},
    {"SS", 135.7, {}},
    {"VNM", 72.4, {}},
    {"HPG", 22.8, {}},
    {"VCB", 95.6, {}},
    {"MWG", 42.3, {}},
    {"VIC", 56.7, {}},
    {"BID", 33.2, {}},
    {"CTG", 29.5, {}},
    {"TCB", 32.8, {}},
    {"MBB", 28.1, {}},
    {"MSN", 64.9, {}}
};


long long currentTime() {
    return std::time(nullptr) * 1000; // ms
}

void updatePrices() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> pct(0.01, 0.05); // 1% - 5%
    std::uniform_int_distribution<> dir(0,1);         // 0 = giảm, 1 = tăng

    std::lock_guard<std::mutex> lock(mtx);
    for (auto& s : stocks) {
        double changePercent = pct(gen);
        int direction = dir(gen) == 0 ? -1 : 1;
        double newPrice = s.price * (1 + direction * changePercent);

        // không cho giá < 10.0
        if (newPrice < 10.0) newPrice = 10.0;

        s.price = newPrice;
        s.history.push_back({currentTime(), s.price});
        if (s.history.size() > 20) s.history.erase(s.history.begin());
    }
}
std::string generateJSON() {
    std::lock_guard<std::mutex> lock(mtx);
    std::ostringstream ss;
    ss << "{ \"stocks\": [";
    for (size_t i=0;i<stocks.size();i++){
        ss << "{\"code\":\"" << stocks[i].code << "\",\"price\":" << stocks[i].price << "}";
        if(i<stocks.size()-1) ss << ",";
    }
    ss << "], \"history\":{";
    for(size_t i=0;i<stocks.size();i++){
        ss << "\"" << stocks[i].code << "\":[";
        for(size_t j=0;j<stocks[i].history.size();j++){
            ss << "{\"time\":" << stocks[i].history[j].first << ",\"price\":" << stocks[i].history[j].second << "}";
            if(j<stocks[i].history.size()-1) ss << ",";
        }
        ss << "]";
        if(i<stocks.size()-1) ss << ",";
    }
    ss << "} }";
    return ss.str();
}

void runServer() {
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2,2), &wsaData);
    SOCKET listenSock = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(8080);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    bind(listenSock,(sockaddr*)&serverAddr,sizeof(serverAddr));
    listen(listenSock, 5);
    std::cout << "C++ Server running on port 8080\n";

    SOCKET clientSock = accept(listenSock, nullptr, nullptr);
    std::cout << "Bridge connected\n";

    std::thread([clientSock](){
        while(true){
            char buffer[1024] = {0};
            int bytes = recv(clientSock, buffer, sizeof(buffer)-1, 0);
            if(bytes>0){
                std::string msg(buffer, bytes);
                // xử lý add stock từ web client
                if(msg.find("\"action\":\"add\"")!=std::string::npos){
                    size_t c1 = msg.find("\"code\":\"")+8;
                    size_t c2 = msg.find("\"",c1);
                    std::string code = msg.substr(c1,c2-c1);
                    size_t p1 = msg.find("\"price\":")+8;
                    size_t p2 = msg.find("}",p1);
                    double price = std::stod(msg.substr(p1,p2-p1));

                    std::lock_guard<std::mutex> lock(mtx);
                    bool exists=false;
                    for(auto& s:stocks) if(s.code==code) exists=true;
                    if(!exists){
                        stocks.push_back({code,price,{{currentTime(),price}}});
                        std::cout << "Added stock: " << code << "\n";
                    }
                }
            }
        }
    }).detach();

    while(true){
        updatePrices();
        std::string data = generateJSON();
        send(clientSock,data.c_str(),data.size(),0);
        std::this_thread::sleep_for(std::chrono::seconds(10));
    }
}

int main() {
    runServer();
    return 0;
}
