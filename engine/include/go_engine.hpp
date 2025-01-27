#ifndef GO_ENGINE_HPP
#define GO_ENGINE_HPP

#include <vector>
#include <unordered_set> // Include this header
#include <utility>       // For std::pair
#include <string>

enum Stone { EMPTY, BLACK, WHITE };

// Hash function for std::pair<int, int>
struct PairHash {
    size_t operator()(const std::pair<int, int>& p) const {
        return p.first * 31 + p.second;
    }
};

class GoEngine {
public:
    GoEngine(int size);
    int getBoardSize() const;
    Stone getStoneAt(int x, int y) const;
    bool placeStone(int x, int y, Stone stone);
    bool isValidMove(int x, int y, Stone stone) const;
    int countLiberties(int x, int y, Stone stone) const;
    void passTurn(Stone stone);
    void printBoard(std::string title = "") const;

private:
    int boardSize;
    Stone lastPlayer;
    std::vector<std::vector<Stone>> board;
    std::pair<int, int> lastMove;

    void countLibertiesHelper(int x, int y, Stone stone, std::vector<std::vector<bool>>& visited, std::unordered_set<std::pair<int, int>, PairHash>& liberties) const;
    void captureStones(int x, int y, Stone stone);
    void captureStones(int x, int y, Stone stone, std::vector<std::vector<bool>>& visited);
    void removeGroup(int x, int y, Stone stone);
};

#endif // GO_ENGINE_HPP