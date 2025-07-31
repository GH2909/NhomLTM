#ifndef GAME_LOGIC_H
#define GAME_LOGIC_H

#include <vector>
#include <iostream>  

class GameLogic {
private:
    int rows, cols;
    int winLength;
    std::vector<std::vector<int>> board;

public:
    GameLogic(int r, int c, int w);

    bool makeMove(int player, int x, int y);
    int checkWin(int x, int y);
    bool isDraw();
    void printBoard();  
};

#endif
