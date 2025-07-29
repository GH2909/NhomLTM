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
        int move[2];
        int bytesReceived = recv(sender, (char*)move, sizeof(move), 0);
        if (bytesReceived <= 0) {
            cout << senderName << " đã mất kết nối.\n";
            gameRunning = false;
            break;
        }

        cout << senderName << " gửi: (" << move[0] << ", " << move[1] << ")\n";

        int bytesSent = send(receiver, (char*)move, sizeof(move), 0);
        if (bytesSent <= 0) {
            cout << "Gửi dữ liệu thất bại từ " << senderName << ".\n";
            gameRunning = false;
            break;
        }
    }
}

int main() {
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        cout << "Khởi tạo Winsock thất bại.\n";
        return -1;
    }

    SOCKET server = socket(AF_INET, SOCK_STREAM, 0);
    if (server == INVALID_SOCKET) {
        cout << "Tạo socket thất bại.\n";
        return -1;
    }

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(server, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        cout << "Bind thất bại.\n";
        closesocket(server);
        WSACleanup();
        return -1;
    }

    listen(server, 2);
    cout << "Server đang chạy. Chờ 2 người chơi kết nối...\n";

    client1 = accept(server, nullptr, nullptr);
    cout << "Người chơi 1 đã kết nối.\n";

    client2 = accept(server, nullptr, nullptr);
    cout << "Người chơi 2 đã kết nối.\n";

    cout << "Trận đấu bắt đầu!\n";

    thread t1(relayMoves, client1, client2, "Player O");
    thread t2(relayMoves, client2, client1, "Player X");

    t1.join();
    t2.join();

    closesocket(client1);
    closesocket(client2);
    closesocket(server);
    WSACleanup();

    return 0;
}
