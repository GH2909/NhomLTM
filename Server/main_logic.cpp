#include "game_logic.h"
#include <iostream>

int main() {
    GameLogic game(15, 15, 5);

    int x, y;
    int currentPlayer = 1;

    while (true) {
        game.printBoard();

        std::cout << "Player " << currentPlayer << ", enter move (row col): ";
        std::cin >> x >> y;

        if (!game.makeMove(currentPlayer, x, y)) {
            std::cout << "Invalid move! Try again.\n";
            continue;
        }

        int winner = game.checkWin(x, y);
        if (winner != 0) {
            game.printBoard();
            std::cout << "Player " << winner << " wins!\n";
            break;
        }

        if (game.isDraw()) {
            game.printBoard();
            std::cout << "Game is a draw!\n";
            break;
        }

        currentPlayer = (currentPlayer == 1) ? 2 : 1;
    }

    return 0;
}
