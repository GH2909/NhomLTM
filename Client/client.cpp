
#include <iostream>
#include <winsock2.h>
#include <thread>


#pragma comment(lib, "ws2_32.lib")


#define PORT 9999
#define SERVER_IP "127.0.0.1"  


using namespace std;


SOCKET client;
bool running = true;


void receiveMoves() {
    while (running) {
        unsigned char buffer[3];
        int totalReceived = 0;


        // Nhận đủ 3 bytes
        while (totalReceived < 3) {
            int bytesReceived = recv(client, (char*)buffer + totalReceived, 3 - totalReceived, 0);
            if (bytesReceived <= 0) {
                cout << "Mat ket noi toi server hoac doi thu.\n";
                running = false;
                return;
            }
            totalReceived += bytesReceived;
        }


        unsigned char row = buffer[0];
        unsigned char col = buffer[1];
        char player = buffer[2];


        cout << "Doi thu danh: (" << (int)row << ", " << (int)col << ") - Nguoi choi: " << player << "\n";
        cout << "Nhan duoc " << totalReceived << " bytes tu server.\n";
    }
}




int main() {
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        cout << "Khoi tao Winsock that bai.\n";
        return -1;
    }


    client = socket(AF_INET, SOCK_STREAM, 0);
    if (client == INVALID_SOCKET) {
        cout << "Tao socket that bai.\n";
        return -1;
    }


    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = inet_addr(SERVER_IP);


    if (connect(client, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        cout << "Ket noi toi server that bai.\n";
        closesocket(client);
        WSACleanup();
        return -1;
    }


    // Nhận ký hiệu người chơi từ server
    char playerChar;
    int received = recv(client, &playerChar, 1, 0);
    if (received <= 0) {
        cout << "Khong nhan duoc ky hieu nguoi choi tu server.\n";
        running = false;
        closesocket(client);
        WSACleanup();
        return -1;
    }
    cout << "Ban la nguoi choi: " << playerChar << "\n";


    // Bắt đầu luồng nhận bước đi từ Web qua bridge
    thread recvThread(receiveMoves);


    recvThread.join();
    closesocket(client);
    WSACleanup();
    return 0;
}



