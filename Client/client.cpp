#include <iostream>
#include <winsock2.h>
#include <thread>
#include <atomic>
#include <windows.h> 
#include <fcntl.h>
#include <io.h>
using namespace std;

#define SERVER_IP "127.0.0.1"  
#define SERVER_PORT 9999

atomic<bool> running(true);

void receiveData(SOCKET sock) {
    char buffer[512];
    while (running) {
        int ret = recv(sock, buffer, sizeof(buffer) - 1, 0);
        if (ret > 0) {
            buffer[ret] = 0;
            cout << "[Server]: " << buffer << endl;
        } else if (ret == 0) {
            cout << "Server đóng kết nối\n";
            running = false;
        } else {
            cout << "Lỗi khi nhận dữ liệu\n";
            running = false;
        }
    }
}

int main() {
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    SetConsoleOutputCP(CP_UTF8);
    _setmode(_fileno(stdout), _O_U16TEXT);

    SOCKET clientSock = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSock == INVALID_SOCKET) {
        cerr << "Không tạo được socket\n";
        return 1;
    }

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(SERVER_PORT);
    serverAddr.sin_addr.s_addr = inet_addr(SERVER_IP);

    if (connect(clientSock, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        cerr << "Không kết nối được server\n";
        closesocket(clientSock);
        WSACleanup();
        return 1;
    }

    cout << "Đã kết nối tới server " << SERVER_IP << ":" << SERVER_PORT << "\n";
thread pingThread([&]() {
    while (running) {
        const char* msg = "ping";
        send(clientSock, msg, strlen(msg), 0);
        this_thread::sleep_for(chrono::seconds(10));
    }
});

    thread recvThread(receiveData, clientSock);

    recvThread.join();
pingThread.join();

    closesocket(clientSock);
    WSACleanup();

    return 0;
}
