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
			cout << senderName << "đa mat ket noi.\n";
			gameRunning = false;
		}

		cout << senderName << ": (" << move[0] << ", " << move[1] << ")\n";
		// gui cho ng choi 2
		send(receiver, (char*)move, sizeof(move), 0);
	}
 }

int main() {
	WSADATA wsa;
	WSAStartup(MAKEWORD(2, 2), &wsa);

	SOCKET server = socket(AF_INET, SOCK_STREAM, 0);
	sockaddr_in serverAddr{};
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(PORT);
	serverAddr.sin_addr.s_addr = INADDR_ANY;

	bind(server, (sockaddr*)&serverAddr, sizeof(serverAddr));
	listen(server, 2);

	cout << "Server đang chay. Cho 2 nguoi choi...\n";

	client1 = accept(server, nullptr, nullptr);
	cout << "Client 1 đa ket noi\n";

	client2 = accept(server, nullptr, nullptr);
	cout << "Client 2 đa ket noi\n";

	cout << "Start!\n";

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