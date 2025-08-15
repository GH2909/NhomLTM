#include <iostream>
#include <string>
#include <thread>
#include <vector>
#include <mutex>
#include <chrono>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <sstream>
#include <iomanip>

#pragma comment(lib, "ws2_32.lib")

const std::string SERVER_IP = "127.0.0.1";
const int SERVER_PORT = 8080;

std::mutex consoleMutex;

bool connectToServer(SOCKET &sock) {
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) {
        return false;
    }

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(SERVER_PORT);
    inet_pton(AF_INET, SERVER_IP.c_str(), &serverAddr.sin_addr);

    if (connect(sock, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        closesocket(sock);
        return false;
    }

    return true;
}

void displayTable(const std::string &data) {
    std::lock_guard<std::mutex> lock(consoleMutex);
    system("cls"); 
    std::istringstream iss(data);
    std::string line;
    std::cout << "+-----------------+----------------+\n";
    std::cout << "| STOCK           | PRICE          |\n";
    std::cout << "+-----------------+----------------+\n";
    while (std::getline(iss, line)) {
        // Kiểm tra dòng có chứa dấu ":"
        auto pos = line.find(":");
        if (pos != std::string::npos) {
            std::string stock = line.substr(0, pos);
            std::string price = line.substr(pos + 1);
            std::cout << "| " << std::setw(15) << std::left << stock
                      << " | " << std::setw(14) << std::right << price << " |\n";
        }
    }
    std::cout << "+-----------------+----------------+\n";
}

// Thread nhận dữ liệu từ server
void receiveData(SOCKET sock) {
    char buffer[4096];
    while (true) {
        int bytesRead = recv(sock, buffer, sizeof(buffer) - 1, 0);
        if (bytesRead <= 0) {
            std::lock_guard<std::mutex> lock(consoleMutex);
            std::cout << "\nDisconnected from server.\n";
            closesocket(sock);
            break;
        }
        buffer[bytesRead] = '\0';
        displayTable(buffer);
    }
}

void sendCommands(SOCKET sock) {
    std::string input;
    while (true) {
        {
            std::lock_guard<std::mutex> lock(consoleMutex);
            std::cout << "\nEnter command (ADD <CODE> / REMOVE <CODE>): ";
        }
        std::getline(std::cin, input);
        if (!input.empty()) {
            int sent = send(sock, input.c_str(), (int)input.size(), 0);
            if (sent == SOCKET_ERROR) {
                std::lock_guard<std::mutex> lock(consoleMutex);
                std::cout << "Failed to send command. Disconnected?\n";
                break;
            }
        }
    }
}

int main() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2,2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed.\n";
        return 1;
    }

    while (true) {
        SOCKET clientSocket;
        std::cout << "Connecting to server...\n";
        if (!connectToServer(clientSocket)) {
            std::cerr << "Failed to connect. Retrying in 5 seconds...\n";
            std::this_thread::sleep_for(std::chrono::seconds(5));
            continue;
        }

        std::cout << "Connected to server!\n";

        std::thread recvThread(receiveData, clientSocket);
        std::thread cmdThread(sendCommands, clientSocket);

        recvThread.join();
        cmdThread.detach(); 
        std::cout << "Reconnecting in 5 seconds...\n";
        std::this_thread::sleep_for(std::chrono::seconds(5));
    }

    WSACleanup();
    return 0;
}
