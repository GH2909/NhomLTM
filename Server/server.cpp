#include <iostream>
#include <winsock2.h>
#include <thread>

#pragma comment(lib, "ws2_32.lib")

#define PORT 9999
using namespace std;

SOCKET client1, client2;
bool gameRunning = true;

void relayMoves(SOCKET sender, SOCKET receiver, const string& senderName) {
    while (gameRunning) {
        unsigned char move[3];
        int bytesReceived = recv(sender, (char*)move, 3, 0);
        if (bytesReceived <= 0) {
            cout << senderName << " da mat ket noi.\n";
            gameRunning = false;
            break;
        }

        unsigned char row = move[0];
        unsigned char col = move[1];
        char player = move[2];

        cout << senderName << " gui: (" << (int)row << ", " << (int)col << ") - Nguoi choi: " << player << "\n";

        int bytesSent = send(receiver, (char*)move, 3, 0);
        if (bytesSent <= 0) {
            cout << "Gui du lieu that bai tu " << senderName << ".\n";
            gameRunning = false;
            break;
        }
    }
}

int main() {
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        cout << "Khoi tao Winsock that bai.\n";
        return -1;
    }

    SOCKET server = socket(AF_INET, SOCK_STREAM, 0);
    if (server == INVALID_SOCKET) {
        cout << "Tao socket that bai.\n";
        return -1;
    }

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(server, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        cout << "Bind that bai.\n";
        closesocket(server);
        WSACleanup();
        return -1;
    }

    listen(server, 2);
    cout << "Server đang chạy. Chờ 2 người chơi kết nối...\n";

    client1 = accept(server, nullptr, nullptr);
    cout << "Nguoi choi 1 da ket noi.\n";

    client2 = accept(server, nullptr, nullptr);
    cout << "Nguoi chơi 2 da ket noi.\n";

    // Gửi ký hiệu người chơi cho client 1 và client 2
    char playerX = 'X';
    char playerO = 'O';
    send(client1, &playerX, 1, 0);
    send(client2, &playerO, 1, 0);


    cout << "Trận đấu bắt đầu!\n";

    thread t1(relayMoves, client1, client2, "Player X");
    thread t2(relayMoves, client2, client1, "Player O");

    t1.join();
    t2.join();

    closesocket(client1);
    closesocket(client2);
    closesocket(server);
    WSACleanup();

    return 0;
}







