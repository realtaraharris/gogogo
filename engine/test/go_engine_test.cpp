#include <vector>
#include <utility> // for std::pair
#include <string>
#include <stdexcept>
#include <iostream>
#include <queue>
#include <format>

#include <gtest/gtest.h>

#include <vector>
#include <string>
#include <stdexcept>
#include <cctype>

constexpr const char STONE_CHARS[] = {'-', 'b', 'w'};
constexpr const char* STONE_NAMES[] = {"EMPTY", "BLACK", "WHITE"};
enum Stone { EMPTY, BLACK, WHITE };
using Board = std::vector<std::vector<Stone>>;
using Group = std::vector<std::pair<int, int>>;
using Captures = std::vector<Group>;
struct Move {
    int row;
    int col;
    Stone side;
};

std::pair<Board, Move> parseGoBoard(const std::string& boardStr, int size, Stone turn) {
    // Collect all non-whitespace characters
    std::vector<char> stones;
    for (char ch : boardStr) {
        if (!isspace(static_cast<unsigned char>(ch))) {
            stones.push_back(ch);
        }
    }

    // Check if the number of stones matches the expected board size
    if (stones.size() != size * size) {
        throw std::invalid_argument("Input string has incorrect number of non-whitespace characters.");
    }

    // Initialize the board with EMPTY stones
    Board board(size, std::vector<Stone>(size, EMPTY));
    Move move = Move{-1, -1, turn};
    int moveCount = 0;

    // Iterate over the stones and populate the board
    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
            char ch = stones[i * size + j];
            switch (static_cast<unsigned char>(ch)) {
                case '-':
                    board[i][j] = EMPTY;
                    break;
                case 'b':
                    board[i][j] = BLACK;
                    break;
                case 'w':
                    board[i][j] = WHITE;
                    break;
                case '*':
                    board[i][j] = turn;
                    move = Move{static_cast<int>(i), static_cast<int>(j), turn};
                    moveCount++;
                    break;
                default:
                    throw std::invalid_argument(std::string("Invalid character in board string: ") + ch);
            }
        }
    }

    // If more than one move was made, return an error
    if (moveCount > 1) {
        throw std::invalid_argument("More than one move detected.");
    }

    return {board, move};
}

std::string printMove(Move move) {
    return
    (move.side == BLACK ? "BLACK" : move.side == WHITE ? "WHITE": "BLANK") +
    ((move.row > -1 && move.col > -1) ?
      std::format(" placed a stone at ({}, {})", move.row, move.col) :
      " passed");
}

const std::string printCaptures(const Board board, const Captures captures, const bool newlines = false) {
    std::string out;
    const int size = board.size();
    std::vector<std::vector<int>> numbers(size, std::vector<int>(size, -1));

    int counter = 1;
    for (const auto& group : captures) {
        for (const auto& pos : group) {
            numbers[pos.first][pos.second] = counter++;
        }
    }

    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            if (numbers[i][j] > -1) {
                out += std::to_string(numbers[i][j]);
            } else {
                out += '-';
            }
            if (j < size - 1) {
                out += ' ';
            }
        }
        if (newlines && i < size - 1) {
            out += '\n';
        }
    }

    return out;
}

std::string printBoard(const Board board, const bool newlines = false) {
    std::string out;
    const int size = board.size();
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            out += STONE_CHARS[board[i][j]];
            if (j < size - 1) {
                out += ' ';
            }
         }
         if (newlines && i < size - 1) {
             out += '\n';
         }
    }
    return out;
}

bool groupHasLibertiesWithGroup(const Board& board, int startRow, int startCol,
                                std::vector<std::vector<bool>>& visited,
                                Group& group) {
    const Stone color = board[startRow][startCol];
    std::queue<std::pair<int, int>> q;
    q.push({startRow, startCol});
    visited[startRow][startCol] = true;
    group.emplace_back(startRow, startCol);
    const int size = board.size();
    constexpr int dirs[4][2] = {{-1,0}, {1,0}, {0,-1}, {0,1}};

    bool hasLiberty = false;

    while (!q.empty()) {
        auto [r, c] = q.front();
        q.pop();

        for (const auto& [dr, dc] : dirs) {
            int nr = r + dr;
            int nc = c + dc;

            if (nr < 0 || nr >= size || nc < 0 || nc >= size) continue;

            if (board[nr][nc] == EMPTY) {
                hasLiberty = true; // Found at least one liberty
            }

            if (board[nr][nc] == color && !visited[nr][nc]) {
                visited[nr][nc] = true;
                q.push({nr, nc});
                group.emplace_back(nr, nc);
            }
        }
    }
    return hasLiberty;
}

Captures findCaptures(const Board board, const Move move) {
    const int size = board.size();
    std::vector<std::vector<bool>> visited(size, std::vector<bool>(size, false));
    std::vector<Group> capturedGroups;

    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
            if (board[i][j] != EMPTY && !visited[i][j]) {
                Group group;
                if (!groupHasLibertiesWithGroup(board, i, j, visited, group) &&
                  !(move.row == i && move.col == j && move.side == board[i][j])
                ) {
                    capturedGroups.push_back(group);
                }
            }
        }
    }

    return capturedGroups;
}

Board applyCapturesAndMove(const Board board, const Move move, const Captures captures) {
    const int size = board.size();
    Board outBoard(size, std::vector<Stone>(size, EMPTY));

    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
           if (move.row == i && move.col == j) {
               outBoard[i][j] = move.side;
           } else {
               outBoard[i][j] = board[i][j];
           }
        }
    }

    for (const auto& group : captures) {
        for (const auto& pos : group) {
            outBoard[pos.first][pos.second] = EMPTY;
        }
    }

    return outBoard;
}


TEST(GoEngineTest, BlankBoardBlackPasses) {
  auto [b, m] = parseGoBoard("- - - - - - - - -" \
                             "- - - - - - - - -" \
                             "- - - - - - - - -" \
                             "- - - - - - - - -" \
                             "- - - - - - - - -" \
                             "- - - - - - - - -" \
                             "- - - - - - - - -" \
                             "- - - - - - - - -" \
                             "- - - - - - - - -", 9, BLACK);
  EXPECT_EQ(printMove(m), "BLACK passed");
  Captures c = findCaptures(b, m);
  EXPECT_EQ(c.size(), 0);
}

TEST(GoEngineTest, BlankBoardWhitePasses) {
  auto [b, m] = parseGoBoard("- - - - - - - - -" \
                             "- - - - - - - - -" \
                             "- - - - - - - - -" \
                             "- - - - - - - - -" \
                             "- - - - - - - - -" \
                             "- - - - - - - - -" \
                             "- - - - - - - - -" \
                             "- - - - - - - - -" \
                             "- - - - - - - - -", 9, WHITE);
  EXPECT_EQ(printMove(m), "WHITE passed");
  Captures c = findCaptures(b, m);
  EXPECT_EQ(c.size(), 0);
}

TEST(GoEngineTest, BlackPlays) {
  auto [b, m] = parseGoBoard("* - - - - - - - -" \
                             "- - - - - - - - -" \
                             "- - - - - - - - -" \
                             "- - - - - - - - -" \
                             "- - - - - - - - -" \
                             "- - - - - - - - -" \
                             "- - - - - - - - -" \
                             "- - - - - - - - -" \
                             "- - - - - - - - -", 9, BLACK);
  EXPECT_EQ(printMove(m), "BLACK placed a stone at (0, 0)");
  Captures c = findCaptures(b, m);
  EXPECT_EQ(c.size(), 0);
}

TEST(GoEngineTest, BlackCapturesOneWhiteStone) {
  auto [b, m] = parseGoBoard("w - - - - - - - -" \
                             "- - - - - - - - -" \
                             "- - - - - - w - -" \
                             "- - - - b - - - -" \
                             "- - - b w b - - -" \
                             "- - - - * - - - -" \
                             "- - - - - - - - -" \
                             "- - - - - - - - -" \
                             "- - - - - - - - -", 9, BLACK);
  EXPECT_EQ(printMove(m), "BLACK placed a stone at (5, 4)");
  Captures c = findCaptures(b, m);

  // verify the number of groups
  EXPECT_EQ(c.size(), 1);
  // verify the number of stones
  EXPECT_EQ(c.at(0).size(), 1);

  EXPECT_EQ(printCaptures(b, c), "- - - - - - - - -" \
                                 "- - - - - - - - -" \
                                 "- - - - - - - - -" \
                                 "- - - - - - - - -" \
                                 "- - - - 1 - - - -" \
                                 "- - - - - - - - -" \
                                 "- - - - - - - - -" \
                                 "- - - - - - - - -" \
                                 "- - - - - - - - -");
}

TEST(GoEngineTest, KoRule) {
  auto [b1, m1] = parseGoBoard("- - - - - - - - -" \
                               "- - w - - - - - -" \
                               "- w * w - - - - -" \
                               "- b w b - - - - -" \
                               "- - b - - - - - -" \
                               "- - - - - - - - -" \
                               "- - - - - - - - -" \
                               "- - - - - - - - -" \
                               "- - - - - - - - -", 9, BLACK);

std::cout << "printBoard:\n" << printBoard(b1, true) << std::endl;

  EXPECT_EQ(printMove(m1), "BLACK placed a stone at (2, 2)");
  Captures c1 = findCaptures(b1, m1);
  // verify the number of groups
  EXPECT_EQ(c1.size(), 1);
  // verify the number of stones
  EXPECT_EQ(c1.at(0).size(), 1);
//std::cout << "printCaptures:\n" << printCaptures(b1, c1, true) << std::endl;
  EXPECT_EQ(printCaptures(b1, c1), "- - - - - - - - -" \
                                   "- - - - - - - - -" \
                                   "- - - - - - - - -" \
                                   "- - 1 - - - - - -" \
                                   "- - - - - - - - -" \
                                   "- - - - - - - - -" \
                                   "- - - - - - - - -" \
                                   "- - - - - - - - -" \
                                   "- - - - - - - - -");

  auto b2 = applyCapturesAndMove(b1, m1, c1);
//std::cout << "printBoard:\n" << printBoard(b2, true) << std::endl;
  EXPECT_EQ(printBoard(b2, true), "- - - - - - - - -" \
                                  "- - w - - - - - -" \
                                  "- w b w - - - - -" \
                                  "- b - b - - - - -" \
                                  "- - b - - - - - -" \
                                  "- - - - - - - - -" \
                                  "- - - - - - - - -" \
                                  "- - - - - - - - -" \
                                  "- - - - - - - - -");
}

TEST(GoEngineTest, BlackCapturesFourWhiteStones) {
  auto [b, m] = parseGoBoard("w - - - - - - - w" \
                             "- - - - - - - - -" \
                             "- - - - b - - - -" \
                             "- - - b w b - - -" \
                             "- - b w * w b - -" \
                             "- - - b w b - - -" \
                             "- - - - b - - - -" \
                             "- - - - - - - - -" \
                             "w - - - - - - - w", 9, BLACK);
  EXPECT_EQ(printMove(m), "BLACK placed a stone at (4, 4)");
  Captures c = findCaptures(b, m);
  EXPECT_EQ(c.size(), 4);
  EXPECT_EQ(c.at(0).size(), 1);
  EXPECT_EQ(c.at(1).size(), 1);
  EXPECT_EQ(c.at(2).size(), 1);
  EXPECT_EQ(c.at(3).size(), 1);
  EXPECT_EQ(printCaptures(b, c), "- - - - - - - - -" \
                                 "- - - - - - - - -" \
                                 "- - - - - - - - -" \
                                 "- - - - 1 - - - -" \
                                 "- - - 2 - 3 - - -" \
                                 "- - - - 4 - - - -" \
                                 "- - - - - - - - -" \
                                 "- - - - - - - - -" \
                                 "- - - - - - - - -");
}

TEST(GoEngineTest, BlackCapturesEightWhiteStones) {
  auto [b, m] = parseGoBoard("w - - - - - - - w" \
                             "- w - - b - - w -" \
                             "- - - b w b - - -" \
                             "- - b b w b b - -" \
                             "- b w w * w w b -" \
                             "- - b b w b b - -" \
                             "- - - b w b - - -" \
                             "- w - - b - - w -" \
                             "w - - - - - - - w", 9, BLACK);
  EXPECT_EQ(printMove(m), "BLACK placed a stone at (4, 4)");
  Captures c = findCaptures(b, m);
  EXPECT_EQ(c.size(), 4);
  EXPECT_EQ(c.at(0).size(), 2);
  EXPECT_EQ(c.at(1).size(), 2);
  EXPECT_EQ(c.at(2).size(), 2);
  EXPECT_EQ(c.at(3).size(), 2);
  EXPECT_EQ(printCaptures(b, c), "- - - - - - - - -" \
                                 "- - - - - - - - -" \
                                 "- - - - 1 - - - -" \
                                 "- - - - 2 - - - -" \
                                 "- - 3 4 - 5 6 - -" \
                                 "- - - - 7 - - - -" \
                                 "- - - - 8 - - - -" \
                                 "- - - - - - - - -" \
                                 "- - - - - - - - -");
}

TEST(GoEngineTest, BlackTKO) {
  auto [b, m] = parseGoBoard("b b b b b b b b b" \
                             "b b b b b b b b b" \
                             "b b b b b b b b b" \
                             "b b b b b b b b b" \
                             "b b b b * b b b b" \
                             "b b b b b b b b b" \
                             "b b b b b b b b b" \
                             "b b b b b b b b b" \
                             "b b b b b b b b b", 9, WHITE);
  EXPECT_EQ(printMove(m), "WHITE placed a stone at (4, 4)");
  Captures c = findCaptures(b, m);
  EXPECT_EQ(c.size(), 1);
  EXPECT_EQ(c.front().size(), 80);
}

TEST(GoEngineTest, WhiteTKO) {
  auto [b, m] = parseGoBoard("w w w w w w w w w" \
                             "w w w w w w w w w" \
                             "w w w w w w w w w" \
                             "w w w w w w w w w" \
                             "w w w w * w w w w" \
                             "w w w w w w w w w" \
                             "w w w w w w w w w" \
                             "w w w w w w w w w" \
                             "w w w w w w w w w", 9, BLACK);
  EXPECT_EQ(printMove(m), "BLACK placed a stone at (4, 4)");
  Captures c = findCaptures(b, m);
  EXPECT_EQ(c.size(), 1);
  EXPECT_EQ(c.front().size(), 80);
}

/*
#include "go_engine.hpp"

TEST(GoEngineTest, BoardInitialization) {
    GoEngine engine(9);
    EXPECT_EQ(engine.getBoardSize(), 9);
    for (int i = 0; i < 9; ++i) {
        for (int j = 0; j < 9; ++j) {
            EXPECT_FALSE(engine.getStoneAt(i, j) == BLACK); // no black stones
            EXPECT_FALSE(engine.getStoneAt(i, j) == WHITE); // no white stones
        }
    }
}

TEST(GoEngineTest, Turns) {
    GoEngine engine(9);
    engine.placeStone(2, 2, BLACK);
    engine.placeStone(2, 1, WHITE);

    EXPECT_FALSE(engine.placeStone(3, 3, WHITE)); // white can't play again
}

TEST(GoEngineTest, PlaceStone) {
    GoEngine engine(5);
    engine.placeStone(2, 2, BLACK);  // place a black stone
    EXPECT_TRUE(engine.getStoneAt(2, 2) == BLACK);
    EXPECT_FALSE(engine.getStoneAt(2, 2) == WHITE); // no white stone at this position
}

TEST(GoEngineTest, InvalidMoveSamePosition) {
    GoEngine engine(5);
    engine.placeStone(2, 2, BLACK); // place black stone
    EXPECT_FALSE(engine.isValidMove(2, 2, BLACK)); // black cannot place here again
    EXPECT_FALSE(engine.isValidMove(2, 2, WHITE)); // white cannot place here
}

TEST(GoEngineTest, ValidMoveNextToOpponent) {
    GoEngine engine(5);
    // - - - - -
    // - - - - -
    // - - B w -
    // - - w - -
    // - - - - -
    engine.placeStone(2, 2, BLACK); // place black stone
    EXPECT_TRUE(engine.isValidMove(2, 3, WHITE)); // valid for white next to black
    EXPECT_TRUE(engine.isValidMove(3, 2, WHITE)); // valid for white next to black
}

TEST(GoEngineTest, Liberties) {
    GoEngine engine(5);
    // B - - - W
    // - - - - -
    // - - B - -
    // - - - - -
    // W - - - B

    engine.placeStone(0, 0, BLACK);
    EXPECT_TRUE(engine.countLiberties(0, 0, BLACK) == 2);

    engine.placeStone(0, 4, WHITE);
    EXPECT_TRUE(engine.countLiberties(0, 4, WHITE) == 2);

    engine.placeStone(4, 4, BLACK);
    EXPECT_TRUE(engine.countLiberties(4, 4, BLACK) == 2);

    engine.placeStone(4, 0, WHITE);
    EXPECT_TRUE(engine.countLiberties(4, 0, WHITE) == 2);

    engine.placeStone(2, 2, BLACK);
    EXPECT_TRUE(engine.countLiberties(2, 2, BLACK) == 4);

    GoEngine engine2(3);
    // B - W
    // - B -
    // W - B

    engine2.placeStone(0, 0, BLACK);
    EXPECT_TRUE(engine2.countLiberties(0, 0, BLACK) == 2);

    engine2.placeStone(0, 2, WHITE);
    EXPECT_TRUE(engine2.countLiberties(0, 2, WHITE) == 2);

    engine2.placeStone(2, 2, BLACK);
    EXPECT_TRUE(engine2.countLiberties(2, 2, BLACK) == 2);

    engine2.placeStone(2, 0, WHITE);
    EXPECT_TRUE(engine2.countLiberties(2, 0, WHITE) == 2);

    engine2.placeStone(1, 1, BLACK);
    EXPECT_TRUE(engine2.countLiberties(1, 1, BLACK) == 4);

    engine2.placeStone(1, 2, WHITE);
    EXPECT_TRUE(engine2.countLiberties(1, 2, WHITE) == 1);
}

TEST(GoEngineTest, CaptureStone) {
    GoEngine engine(9);
    // set up a capture scenario:
    // B - -
    // B W -
    // W - -
    engine.placeStone(0, 0, BLACK); // B
    engine.placeStone(1, 1, WHITE); // W
    engine.placeStone(0, 1, BLACK); // B
    engine.placeStone(0, 2, WHITE); // W

    EXPECT_TRUE(engine.getStoneAt(0, 0) == BLACK);
    EXPECT_TRUE(engine.getStoneAt(1, 1) == WHITE);
    EXPECT_TRUE(engine.getStoneAt(0, 1) == BLACK);
    EXPECT_TRUE(engine.getStoneAt(0, 2) == WHITE);

    EXPECT_EQ(engine.countLiberties(1, 1, WHITE), 3);
    EXPECT_EQ(engine.countLiberties(0, 2, WHITE), 2);

    EXPECT_EQ(engine.countLiberties(0, 0, BLACK), 1);
    EXPECT_EQ(engine.countLiberties(0, 1, BLACK), 1);

    engine.passTurn(BLACK);

    engine.placeStone(1, 0, WHITE); // place white to capture
    EXPECT_FALSE(engine.getStoneAt(0, 0) == BLACK); // black stone should be captured
    EXPECT_FALSE(engine.getStoneAt(0, 1) == BLACK); // black stone should be captured
}

TEST(GoEngineTest, TKO) {
    GoEngine engine(5);
    // set up a capture scenario:
    // B B B W
    // B W w W
    // W W W W

    engine.placeStone(0, 0, BLACK); // B
    engine.placeStone(1, 1, WHITE); // W
    engine.placeStone(0, 1, BLACK); // B
    engine.placeStone(0, 2, WHITE); // W

    engine.placeStone(1, 0, BLACK); // B
    engine.placeStone(1, 2, WHITE); // W
    engine.placeStone(2, 0, BLACK); // B
    engine.placeStone(2, 2, WHITE); // W

    engine.passTurn(BLACK);

    engine.placeStone(3, 0, WHITE); // W
    engine.passTurn(BLACK);

    engine.placeStone(3, 1, WHITE); // W
    engine.passTurn(BLACK);
    engine.placeStone(3, 2, WHITE); // W
    engine.passTurn(BLACK);

    EXPECT_EQ(engine.getStoneAt(0, 0), BLACK);
    EXPECT_EQ(engine.getStoneAt(1, 1), WHITE);
    EXPECT_EQ(engine.getStoneAt(0, 1), BLACK);
    EXPECT_EQ(engine.getStoneAt(0, 2), WHITE);

    EXPECT_EQ(engine.getStoneAt(1, 0), BLACK);
    EXPECT_EQ(engine.getStoneAt(1, 2), WHITE);
    EXPECT_EQ(engine.getStoneAt(2, 0), BLACK);
    EXPECT_EQ(engine.getStoneAt(2, 2), WHITE);

    EXPECT_EQ(engine.getStoneAt(3, 0), WHITE);
    EXPECT_EQ(engine.getStoneAt(3, 1), WHITE);
    EXPECT_EQ(engine.getStoneAt(3, 2), WHITE);

    EXPECT_EQ(engine.countLiberties(1, 1, WHITE), 8);
    EXPECT_EQ(engine.countLiberties(0, 2, WHITE), 8);

    EXPECT_EQ(engine.countLiberties(0, 0, BLACK), 1);
    EXPECT_EQ(engine.countLiberties(0, 1, BLACK), 1);

    engine.placeStone(2, 1, WHITE); // place white to capture

    // black stones should be captured
    EXPECT_EQ(engine.getStoneAt(0, 0), EMPTY);
    EXPECT_EQ(engine.getStoneAt(1, 0), EMPTY);
    EXPECT_EQ(engine.getStoneAt(2, 0), EMPTY);
    EXPECT_EQ(engine.getStoneAt(0, 1), EMPTY);
}

TEST(GoEngineTest, TotalTKO) {
    GoEngine engine(3);
    // set up a capture scenario:
    // B B B
    // B W b
    // W W W

    engine.placeStone(0, 0, BLACK); // B
    engine.placeStone(1, 1, WHITE); // W
    engine.placeStone(0, 1, BLACK); // B
    engine.placeStone(0, 2, WHITE); // W

    engine.placeStone(1, 0, BLACK); // B
    engine.placeStone(1, 2, WHITE); // W
    engine.placeStone(2, 0, BLACK); // B
    engine.placeStone(2, 2, WHITE); // W

    EXPECT_EQ(engine.getStoneAt(0, 0), BLACK);
    EXPECT_EQ(engine.getStoneAt(1, 1), WHITE);
    EXPECT_EQ(engine.getStoneAt(0, 1), BLACK);
    EXPECT_EQ(engine.getStoneAt(0, 2), WHITE);

    EXPECT_EQ(engine.getStoneAt(1, 0), BLACK);
    EXPECT_EQ(engine.getStoneAt(1, 2), WHITE);
    EXPECT_EQ(engine.getStoneAt(2, 0), BLACK);
    EXPECT_EQ(engine.getStoneAt(2, 2), WHITE);

    EXPECT_EQ(engine.countLiberties(1, 1, WHITE), 1); // white stone group has 1 liberty
    EXPECT_EQ(engine.countLiberties(0, 2, WHITE), 1);

    EXPECT_EQ(engine.countLiberties(0, 0, BLACK), 1); // black stone group has 1 liberty
    EXPECT_EQ(engine.countLiberties(0, 1, BLACK), 1);

    engine.placeStone(2, 1, BLACK); // place black to capture

    EXPECT_EQ(engine.getStoneAt(0, 0), BLACK); // black stones should be captured
    EXPECT_EQ(engine.getStoneAt(1, 0), BLACK); // black stones should be captured
    EXPECT_EQ(engine.getStoneAt(2, 0), BLACK); // black stones should be captured
    EXPECT_EQ(engine.getStoneAt(0, 1), BLACK); // black stones should be captured

    EXPECT_EQ(engine.getStoneAt(1, 1), EMPTY); // no white stones should remain
    EXPECT_EQ(engine.getStoneAt(0, 2), EMPTY);
    EXPECT_EQ(engine.getStoneAt(1, 2), EMPTY);
    EXPECT_EQ(engine.getStoneAt(2, 2), EMPTY);
}

TEST(GoEngineTest, NoSelfCapture) {
    GoEngine engine(3);
    // Set up:
    // B B B
    // - W B
    // B B B
    engine.placeStone(0, 0, BLACK); // B
    engine.passTurn(WHITE);
    engine.placeStone(1, 0, BLACK); // B
    engine.passTurn(WHITE);

    engine.placeStone(2, 0, BLACK); // B
    engine.passTurn(WHITE);
    engine.placeStone(2, 1, BLACK); // B
    engine.passTurn(WHITE);
    engine.placeStone(2, 2, BLACK); // B
    engine.passTurn(WHITE);

    engine.placeStone(1, 2, BLACK); // B
    engine.passTurn(WHITE);
    engine.placeStone(0, 2, BLACK); // B

    engine.placeStone(1, 1, WHITE); // W

    EXPECT_FALSE(engine.isValidMove(1, 2, WHITE)); // white cannot self-capture
}

TEST(GoEngineTest, KoRule) {
    GoEngine engine(6);

    // - - - - - -
    // - - W B - -
    // - W b W B -
    // - - W B - -
    // - - - - - -
    // - - - - - -

    engine.placeStone(2, 1, WHITE); // place white stone
    engine.placeStone(3, 3, BLACK); // place black stone
    engine.placeStone(2, 3, WHITE); // place white stone
    engine.placeStone(3, 1, BLACK); // place black stone
    engine.placeStone(1, 2, WHITE); // place white stone
    engine.placeStone(4, 2, BLACK); // place black stone
    engine.placeStone(3, 2, WHITE); // place white stone

    engine.placeStone(2, 2, BLACK); // place black stone, capturing white at (3, 2)

    EXPECT_EQ(engine.getStoneAt(2, 2), BLACK); // black stone should be there

    EXPECT_EQ(engine.getStoneAt(3, 2), EMPTY); // white stone should be captured

    // black should not be able to recapture immediately at (2, 2)
    EXPECT_FALSE(engine.isValidMove(2, 2, BLACK)); // this move should not be valid due to ko rule

    // however, if white plays elsewhere, black should be able to recapture.
    engine.placeStone(0, 0, WHITE); // white plays elsewhere
    engine.placeStone(0, 5, BLACK); // black plays elsewhere
    EXPECT_TRUE(engine.isValidMove(3, 2, WHITE)); // now black can recapture

    // place white stone to recapture
    engine.placeStone(3, 2, WHITE);
    EXPECT_EQ(engine.getStoneAt(3, 2), WHITE);
    EXPECT_EQ(engine.getStoneAt(2, 2), EMPTY); // white stone at (2, 2) should be captured now

    engine.printBoard("291");
    // now white should not be able to immediately recapture at (2, 2) due to ko
//  EXPECT_FALSE(engine.isValidMove(2, 2, BLACK)); // this move should not be valid due to ko rule
}
*/

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}
