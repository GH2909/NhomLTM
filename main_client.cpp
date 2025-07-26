#include "socket_helper.h"
#include "game_logic.h"
#include <SFML/Graphics.hpp>
#include <iostream>

#define SERVER_IP "127.0.0.1"
#define PORT 9999

int main() {
    initSocket();
    SOCKET client = createClientSocket(SERVER_IP, PORT);
    std::cout << "Connected to server.\n";

    GameLogic game;
    sf::RenderWindow window(sf::VideoMode(600, 600), "Caro Client");

    const float cellSize = 600.0f / BOARD_SIZE;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            if (event.type == sf::Event::MouseButtonPressed) {
                int x = event.mouseButton.x / cellSize;
                int y = event.mouseButton.y / cellSize;

                if (game.getCurrentPlayer() == O && game.makeMove(y, x)) {
                    int move[] = { y, x };
                    send(client, (char*)move, sizeof(move), 0);
                    if (game.isWin(y, x)) {
                        std::cout << "Player O wins!\n";
                        window.close();
                    }
                }
            }
        }

        // Nhận nước đi từ server (player X)
        int move[2];
        if (game.getCurrentPlayer() == X && recv(client, (char*)move, sizeof(move), MSG_PEEK) > 0) {
            recv(client, (char*)move, sizeof(move), 0);
            game.makeMove(move[0], move[1]);
            if (game.isWin(move[0], move[1])) {
                std::cout << "Player X wins!\n";
                window.close();
            }
        }

        // Vẽ bàn cờ
        window.clear(sf::Color::White);
        for (int i = 1; i < BOARD_SIZE; ++i) {
            sf::Vertex line1[] = {
                sf::Vertex(sf::Vector2f(i * cellSize, 0)),
                sf::Vertex(sf::Vector2f(i * cellSize, 600))
            };
            sf::Vertex line2[] = {
                sf::Vertex(sf::Vector2f(0, i * cellSize)),
                sf::Vertex(sf::Vector2f(600, i * cellSize))
            };
            window.draw(line1, 2, sf::Lines);
            window.draw(line2, 2, sf::Lines);
        }

        // Vẽ X và O
        for (int i = 0; i < BOARD_SIZE; ++i)
            for (int j = 0; j < BOARD_SIZE; ++j) {
                sf::Text text;
                sf::Font font;
                if (!font.loadFromFile("arial.ttf")) continue;

                text.setFont(font);
                text.setCharacterSize(24);
                text.setPosition(j * cellSize + 10, i * cellSize + 5);

                if (game.getCell(i, j) == X)
                    text.setString("X");
                else if (game.getCell(i, j) == O)
                    text.setString("O");

                window.draw(text);
            }

        window.display();
    }

    closesocket(client);
    cleanupSocket();
    return 0;
}