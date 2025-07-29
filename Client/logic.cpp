#include "game_logic.h"
#include <iostream>  // Phải thêm để dùng std::cout

GameLogic::GameLogic(int r, int c, int w)
    : rows(r), cols(c), winLength(w), board(r, std::vector<int>(c, 0)) {}

bool GameLogic::makeMove(int player, int x, int y) {
    if (x < 0 || x >= rows || y < 0 || y >= cols) return false;
    if (board[x][y] != 0) return false;

    board[x][y] = player;
    return true;
}

int GameLogic::checkWin(int x, int y) {
    int player = board[x][y];
    if (player == 0) return 0;

    const int directions[4][2] = {
        {0, 1},  // ngang
        {1, 0},  // dọc
        {1, 1},  // chéo chính
        {1, -1}  // chéo phụ
    };

    for (auto& dir : directions) {
        int count = 1;
        int dx = dir[0], dy = dir[1];
        int nx = x + dx, ny = y + dy;
        while (nx >= 0 && nx < rows && ny >= 0 && ny < cols && board[nx][ny] == player) {
            count++;
            nx += dx;
            ny += dy;
        }

        nx = x - dx;
        ny = y - dy;
        while (nx >= 0 && nx < rows && ny >= 0 && ny < cols && board[nx][ny] == player) {
            count++;
            nx -= dx;
            ny -= dy;
        }

        if (count >= winLength) return player;
    }

    return 0;
}

bool GameLogic::isDraw() {
    for (int i = 0; i < rows; i++)
        for (int j = 0; j < cols; j++)
            if (board[i][j] == 0) return false;
    return true;
}

void GameLogic::printBoard() {
    std::cout << "  ";
    for (int j = 0; j < cols; j++) {
        if (j < 10) std::cout << j << "  ";
        else std::cout << j << " ";
    }
    std::cout << "\n";

    for (int i = 0; i < rows; i++) {
        if (i < 10) std::cout << i << " ";
        else std::cout << i;

        for (int j = 0; j < cols; j++) {
            char c;
            if (board[i][j] == 0) c = '.';
            else if (board[i][j] == 1) c = 'X';
            else c = 'O';
            std::cout << " " << c << " ";
        }
        std::cout << "\n";
    }
}
