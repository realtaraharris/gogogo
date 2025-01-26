// go_engine.hpp
#ifndef GO_ENGINE_HPP
#define GO_ENGINE_HPP

#include <vector>
#include <tuple>
#include <stdexcept>
#include <iostream>

class GoEngine {
public:
  GoEngine(int size);
  int getBoardSize() const;
  bool isStoneAt(int x, int y, bool black) const;
  void placeStone(int x, int y, bool black);
  bool isValidMove(int x, int y, bool black);
  int countLiberties(int x, int y);
  void captureStones(int x, int y, bool black);
  void printBoard();

private:
  int boardSize;
  std::vector<std::vector<int>> board;
  std::vector<std::vector<int>> previousBoard;
  int koLastX;
  int koLastY;
  int koLastColor;
  int countLibertiesDFS(int x, int y, int color, std::vector<std::vector<bool>>& visited);
  void removeStoneGroup(int x, int y, int color);
  int countFreeStones();
  char intToStoneChar(const int stone) const;
};

#endif // GO_ENGINE_HPP
