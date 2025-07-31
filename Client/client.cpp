#include <iostream>
#include <winsock2.h>
#include <thread>

#pragma comment(lib, "ws2_32.lib")

#define PORT 9999
#define SERVER_IP "127.0.0.1"  // Thay đổi nếu server không chạy cùng máy

using namespace std;

SOCKET client;
bool running = true;

// Hàm nhận dữ liệu từ đối thủ
void receiveMoves() {
    while (running) {
        int move[2];
        int bytesReceived = recv(client, (char*)move, sizeof(move), 0);
        if (bytesReceived <= 0) {
            cout << "Mất kết nối tới server hoặc đối thủ.\n";
            running = false;
            break;
        }

        cout << "Đối thủ đánh: (" << move[0] << ", " << move[1] << ")\n";
    }
}

int main() {
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        cout << "Khởi tạo Winsock thất bại.\n";
        return -1;
    }

    client = socket(AF_INET, SOCK_STREAM, 0);
    if (client == INVALID_SOCKET) {
        cout << "Tạo socket thất bại.\n";
        return -1;
    }

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = inet_addr(SERVER_IP);

    if (connect(client, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        cout << "Kết nối tới server thất bại.\n";
        closesocket(client);
        WSACleanup();
        return -1;
    }

    cout << "Đã kết nối tới server. Nhập tọa độ để đánh (hàng cột), hoặc -1 để thoát.\n";

    thread recvThread(receiveMoves);

    while (running) {
        int x, y;
        cout << "Nhập nước đi (hàng cột): ";
        cin >> x;

        if (x == -1) {
            running = false;
            break;
        }

        cin >> y;

        int move[2] = {x, y};
        int bytesSent = send(client, (char*)move, sizeof(move), 0);
        if (bytesSent <= 0) {
            cout << "Gửi dữ liệu thất bại.\n";
            running = false;
            break;
        }
    }

    recvThread.join();
    closesocket(client);
    WSACleanup();

    return 0;
}
