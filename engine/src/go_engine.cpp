// go_engine.cpp
#include "go_engine.hpp"

GoEngine::GoEngine(int size) :
boardSize(size),
board(size, std::vector<int>(size, 0)),
koLastX(-1),
koLastY(-1),
koLastColor(0)
{}

char GoEngine::intToStoneChar(const int stone) const {
  if (stone == 0) {
    return '-';
  } else if (stone == 1) {
    return 'B';
  } else if (stone == 2) {
    return 'W';
  }

  return '!';
}

void GoEngine::printBoard() {
  for (int x = 0; x < boardSize; x++) {
    for (int y = 0; y < boardSize; y++) {
      std::cout << intToStoneChar(board[y][x]);
      if (y < boardSize - 1) { std::cout << ' '; }
    }
    std::cout << std::endl;
  }
}

int GoEngine::getBoardSize() const {
    return boardSize;
}

bool GoEngine::isStoneAt(int x, int y, bool black) const {
    return board[x][y] == (black ? 1 : 2);
}

void GoEngine::placeStone(int x, int y, bool black) {
    if (!isValidMove(x, y, black)) {
        throw std::invalid_argument("Invalid move");
    }
    board[x][y] = black ? 1 : 2;
    previousBoard = board;

    captureStones(x, y, black);
}

bool GoEngine::isValidMove(int x, int y, bool black) {
    if (board[x][y] != 0) { return false; }

    // if we only have one liberty and one free spot on the board, that's a TKO
    const int freeStones = countFreeStones();
    const int initialLiberties = countLiberties(x, y);

std::cout << "freeStones: " << freeStones << " initialLiberties: " << initialLiberties << std::endl;

    if (initialLiberties == 0 && freeStones > 0) { // could be a KO
      return true;
    }

    // temporarily place the stone
    board[x][y] = black ? 1 : 2;
    int liberties = countLiberties(x, y);
    board[x][y] = 0;  // Rollback move

    // if no liberties, move is a self-capture
    return liberties > 0;
}

int GoEngine::countLiberties(int x, int y) {
    if (board[x][y] == 0) return 0;
    std::vector<std::vector<bool>> visited(boardSize, std::vector<bool>(boardSize, false));
    return countLibertiesDFS(x, y, board[x][y], visited);
}

int GoEngine::countFreeStones() {
    int freeStones = 0;
    for (int x = 0; x < boardSize; ++x) {
        for (int y = 0; y < boardSize; ++y) {
          if (board[x][y] == 0) { freeStones++; }
        }
    }
    return freeStones;
}

void GoEngine::captureStones(int x, int y, bool black) {
    std::vector<std::vector<bool>> checked(boardSize, std::vector<bool>(boardSize, false));
    for (int i = 0; i < boardSize; ++i) {
        for (int j = 0; j < boardSize; ++j) {
            if (x == i && y == j) { // && board[i][j] == black ? 1 : 2) {
              checked[i][j] = true;
              continue; // make sure we don't remove a group
            }
            if (board[i][j] != 0 && !checked[i][j]) {
                std::vector<std::vector<bool>> visited(boardSize, std::vector<bool>(boardSize, false));
                int liberties = countLibertiesDFS(i, j, board[i][j], visited);
                if (liberties == 0) {
                    removeStoneGroup(i, j, board[i][j]);
                }
                checked[i][j] = true;
            }
        }
    }
}

int GoEngine::countLibertiesDFS(int x, int y, int color, std::vector<std::vector<bool>>& visited) {
    if (x < 0 || y < 0 || x >= boardSize || y >= boardSize || visited[x][y]) {
        return 0;
    }
    visited[x][y] = true;

    if (board[x][y] == 0) return 1;  // found an empty spot, count as a liberty
    if (board[x][y] != color) return 0;  // opponent stone, not a liberty

    // Explore neighbors for connected stones
    int liberties = 0;
    liberties += countLibertiesDFS(x + 1, y, color, visited);
    liberties += countLibertiesDFS(x - 1, y, color, visited);
    liberties += countLibertiesDFS(x, y + 1, color, visited);
    liberties += countLibertiesDFS(x, y - 1, color, visited);

    return liberties;
}

void GoEngine::removeStoneGroup(int x, int y, int color) {
    if (x < 0 || y < 0 || x >= boardSize || y >= boardSize || board[x][y] != color) {
        return;
    }
    board[x][y] = 0;
    removeStoneGroup(x + 1, y, color);
    removeStoneGroup(x - 1, y, color);
    removeStoneGroup(x, y + 1, color);
    removeStoneGroup(x, y - 1, color);
}
