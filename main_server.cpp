#include <iostream>
#include <winsock2.h>
#include <thread>

#pragma comment(lib, "ws2_32.lib")

#define PORT 9999
#define BUFFER_SIZE 1024

using namespace std;

SOCKET client1, client2;
void handleClient(SOCKET sender, SOCKET receiver, const string& name) {
	char buffer[BUFFER_SIZE];

	while (true) {

	}
 }