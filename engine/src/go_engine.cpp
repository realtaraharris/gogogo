#include "go_engine.hpp"
#include <iostream>
#include <unordered_set>

GoEngine::GoEngine(int size)
    : boardSize(size), board(size, std::vector<Stone>(size, EMPTY)), lastMove(-1, -1), lastPlayer(EMPTY) {}

int GoEngine::getBoardSize() const {
    return boardSize;
}

Stone GoEngine::getStoneAt(int x, int y) const {
    return board[x][y];
}

bool GoEngine::placeStone(int x, int y, Stone stone) {
    if (!isValidMove(x, y, stone)) {
    std::cout << "placeStone, quitting due to invalid move" << std::endl;
        return false;
    }

    board[x][y] = stone;
    lastMove = {x, y};
    lastPlayer = stone;
    captureStones(x, y, stone);
    return true;
}

bool GoEngine::isValidMove(int x, int y, Stone stone) const {
    if (lastPlayer == stone) {
    std::cout << "isValidMove, quitting due to lastPlayer == stone: " << stone << std::endl;
        return false;
    }

    if (x < 0 || x >= boardSize || y < 0 || y >= boardSize || board[x][y] != EMPTY) {
            std::cout << "isValidMove, quitting due to board bounds:" << std::endl;
            std::cout << "    x: " << x << std::endl;
            std::cout << "    y: " << y << std::endl;
            std::cout << "    boardSize: " << boardSize << std::endl;
            std::cout << "    board[x][y]: " << board[x][y] << std::endl;
        return false;
    }

    // Check for Ko rule
    if (x == lastMove.first && y == lastMove.second) {
        std::cout << "isValidMove, quitting due to KO rule" << std::endl;

        return false;
    }

    // Check for self-capture
    std::vector<std::vector<bool>> visited(boardSize, std::vector<bool>(boardSize, false));
    if (countLiberties(x, y, stone) == 0) {
            std::cout << "isValidMove, quitting due to self-capture" << std::endl;

        return false;
    }

    return true;
}

int GoEngine::countLiberties(int x, int y, Stone stone) const {
    std::vector<std::vector<bool>> visited(boardSize, std::vector<bool>(boardSize, false));
    std::unordered_set<std::pair<int, int>, PairHash> liberties;
    countLibertiesHelper(x, y, stone, visited, liberties);
    return liberties.size();
}

void GoEngine::countLibertiesHelper(int x, int y, Stone stone, std::vector<std::vector<bool>>& visited, std::unordered_set<std::pair<int, int>, PairHash>& liberties) const {
    if (x < 0 || x >= boardSize || y < 0 || y >= boardSize || visited[x][y]) {
        return;
    }

    visited[x][y] = true;

    if (board[x][y] == EMPTY) {
        liberties.insert({x, y});
        return;
    }

    if (board[x][y] != stone) {
        return;
    }

    // Recursively check adjacent positions
    countLibertiesHelper(x + 1, y, stone, visited, liberties);
    countLibertiesHelper(x - 1, y, stone, visited, liberties);
    countLibertiesHelper(x, y + 1, stone, visited, liberties);
    countLibertiesHelper(x, y - 1, stone, visited, liberties);
}

void GoEngine::passTurn(Stone stone) {
    lastPlayer = stone;
    lastMove = {-1, -1};
}

void GoEngine::printBoard(std::string title) const {
    std::cout << title << ": {" << std::endl;
    for (const auto& row : board) {
        for (const auto& cell : row) {
            std::cout << (cell == EMPTY ? '-' : (cell == BLACK ? 'B' : 'W')) << " ";
        }
        std::cout << std::endl;
    }
    std::cout << "}" << std::endl;
}

void GoEngine::captureStones(int x, int y, Stone stone) {
    // Temporarily place the stone on the board
    board[x][y] = stone;

    // Check adjacent positions for opponent stones
    std::vector<std::pair<int, int>> directions = {{1, 0}, {-1, 0}, {0, 1}, {0, -1}};
    for (const auto& dir : directions) {
        int nx = x + dir.first;
        int ny = y + dir.second;

        if (nx >= 0 && nx < boardSize && ny >= 0 && ny < boardSize && board[nx][ny] != EMPTY && board[nx][ny] != stone) {
            std::vector<std::vector<bool>> visited(boardSize, std::vector<bool>(boardSize, false));
            if (countLiberties(nx, ny, board[nx][ny]) == 0) {
                removeGroup(nx, ny, board[nx][ny]);
            }
        }
    }

    // Check if the newly placed stone's group has zero liberties (self-capture)
    std::vector<std::vector<bool>> visited(boardSize, std::vector<bool>(boardSize, false));
    if (countLiberties(x, y, stone) == 0) {
        removeGroup(x, y, stone);
    }
}

void GoEngine::removeGroup(int x, int y, Stone stone) {
    if (x < 0 || x >= boardSize || y < 0 || y >= boardSize || board[x][y] != stone) {
        return;
    }

    board[x][y] = EMPTY;

    removeGroup(x + 1, y, stone);
    removeGroup(x - 1, y, stone);
    removeGroup(x, y + 1, stone);
    removeGroup(x, y - 1, stone);
}

/*
#include "go_engine.hpp"
#include <iostream>

GoEngine::GoEngine(int size)
    : boardSize(size), board(size, std::vector<Stone>(size, EMPTY)), lastMove(-1, -1), lastPlayer(EMPTY) {}

int GoEngine::getBoardSize() const {
    return boardSize;
}

Stone GoEngine::getStoneAt(int x, int y) const {
    return board[x][y];
}

bool GoEngine::placeStone(int x, int y, Stone stone) {
    if (lastPlayer == stone || !isValidMove(x, y, stone)) {
    std::cout << "not a valid move" << std::endl;
        return false;
    }

    board[x][y] = stone;
    lastMove = {x, y};
    lastPlayer = stone;
    captureStones(x, y, stone);
    return true;
}

bool GoEngine::isValidMove(int x, int y, Stone stone) const {
    if (x < 0 || x >= boardSize || y < 0 || y >= boardSize || board[x][y] != EMPTY) {
        return false;
    }

    // Check for Ko rule
    if (x == lastMove.first && y == lastMove.second) {
        return false;
    }

    // Check for self-capture
    std::vector<std::vector<bool>> visited(boardSize, std::vector<bool>(boardSize, false));
    if (countLiberties(x, y, stone, visited) == 0) {
        return false;
    }

    return true;
}

int GoEngine::countLiberties(int x, int y, Stone stone) const {
    std::vector<std::vector<bool>> visited(boardSize, std::vector<bool>(boardSize, false));
    return countLiberties(x, y, stone, visited);
}

void GoEngine::passTurn(Stone stone) {
    lastPlayer = stone;
    lastMove = {-1, -1};
}

void GoEngine::printBoard() const {
    std::cout << "{" << std::endl;
    for (const auto& row : board) {
        for (const auto& cell : row) {
            std::cout << (cell == EMPTY ? '-' : (cell == BLACK ? 'B' : 'W')) << " ";
        }
        std::cout << std::endl;
    }
    std::cout << "}" << std::endl;
}

int GoEngine::countLiberties(int x, int y, Stone stone, std::vector<std::vector<bool>>& visited) const {
    if (x < 0 || x >= boardSize || y < 0 || y >= boardSize || visited[x][y]) {
        return 0;
    }

    if (board[x][y] == EMPTY) {
        return 1;
    }

    if (board[x][y] != stone) {
        return 0;
    }

    visited[x][y] = true;

    int liberties = 0;
    liberties += countLiberties(x + 1, y, stone, visited);
    liberties += countLiberties(x - 1, y, stone, visited);
    liberties += countLiberties(x, y + 1, stone, visited);
    liberties += countLiberties(x, y - 1, stone, visited);

    return liberties;
}

void GoEngine::captureStones(int x, int y, Stone stone) {
    // Temporarily place the stone on the board
    board[x][y] = stone;

    // Check adjacent positions for opponent stones
    std::vector<std::pair<int, int>> directions = {{1, 0}, {-1, 0}, {0, 1}, {0, -1}};
    for (const auto& dir : directions) {
        int nx = x + dir.first;
        int ny = y + dir.second;

        if (nx >= 0 && nx < boardSize && ny >= 0 && ny < boardSize && board[nx][ny] != EMPTY && board[nx][ny] != stone) {
            std::vector<std::vector<bool>> visited(boardSize, std::vector<bool>(boardSize, false));
            if (countLiberties(nx, ny, board[nx][ny], visited) == 0) {
                removeGroup(nx, ny, board[nx][ny]);
            }
        }
    }

    // Check if the newly placed stone's group has zero liberties (self-capture)
    std::vector<std::vector<bool>> visited(boardSize, std::vector<bool>(boardSize, false));
    if (countLiberties(x, y, stone, visited) == 0) {
        removeGroup(x, y, stone);
    }
}

void GoEngine::removeGroup(int x, int y, Stone stone) {
    if (x < 0 || x >= boardSize || y < 0 || y >= boardSize || board[x][y] != stone) {
        return;
    }

    board[x][y] = EMPTY;
    std::cout << "removing position: " << x << ", " << y << std::endl;

    removeGroup(x + 1, y, stone);
    removeGroup(x - 1, y, stone);
    removeGroup(x, y + 1, stone);
    removeGroup(x, y - 1, stone);
}
*/