#include <Window.h>
#include <View.h>
#include <Application.h>
#include <Point.h>
#include <stdio.h>
#include <vector>

const int BOARD_SIZE = 19;
const int CELL_SIZE = 40;

class GoBoard : public BView {
    std::vector<std::vector<int>> board;
    bool blackTurn = true;
    std::pair<int, int> lastMove = {-1, -1}; // Store the last move's coordinates
    std::vector<std::vector<int>> previousBoard; // Store previous board state for ko rule

public:
    GoBoard(BRect frame) : BView(frame, "GoBoardView", B_FOLLOW_ALL, B_WILL_DRAW | B_FULL_UPDATE_ON_RESIZE) {
        board = std::vector<std::vector<int>>(BOARD_SIZE, std::vector<int>(BOARD_SIZE, 0));
        previousBoard = std::vector<std::vector<int>>(BOARD_SIZE, std::vector<int>(BOARD_SIZE, 0));
    }

    void Draw(BRect updateRect) override {
        printf("Drawing\n");
        SetHighColor(0, 0, 0);

        // Draw grid lines through the center of cells
        for (int i = 0; i < BOARD_SIZE; i++) {
            StrokeLine(BPoint(CELL_SIZE / 2 + i * CELL_SIZE, CELL_SIZE / 2),
                       BPoint(CELL_SIZE / 2 + i * CELL_SIZE, CELL_SIZE * (BOARD_SIZE - 0.5)));
            StrokeLine(BPoint(CELL_SIZE / 2, CELL_SIZE / 2 + i * CELL_SIZE),
                       BPoint(CELL_SIZE * (BOARD_SIZE - 0.5), CELL_SIZE / 2 + i * CELL_SIZE));
        }

        // Draw stones centered on vertices
        for (int y = 0; y < BOARD_SIZE; y++) {
            for (int x = 0; x < BOARD_SIZE; x++) {
                if (board[y][x] == 1) { // Black stone
                    FillEllipse(BRect(x * CELL_SIZE, y * CELL_SIZE, x * CELL_SIZE + CELL_SIZE, y * CELL_SIZE + CELL_SIZE));
                } else if (board[y][x] == 2) { // White stone
                    SetHighColor(255, 255, 255);
                    FillEllipse(BRect(x * CELL_SIZE, y * CELL_SIZE, x * CELL_SIZE + CELL_SIZE, y * CELL_SIZE + CELL_SIZE));
                    SetHighColor(0, 0, 0);
                    StrokeEllipse(BRect(x * CELL_SIZE, y * CELL_SIZE, x * CELL_SIZE + CELL_SIZE, y * CELL_SIZE + CELL_SIZE));
                }
            }
        }
    }

  void MouseDown(BPoint point) override {
    printf("Move attempted at (%f, %f)\n", point.x, point.y);
    int x = (int)(point.x / CELL_SIZE);
    int y = (int)(point.y / CELL_SIZE);

    if (!(
      x >= 0 &&
      x < BOARD_SIZE &&
      y >= 0 &&
      y < BOARD_SIZE &&
      board[y][x] == 0 &&
      isValidMove(x, y)
    )) {
      return;
    }

    previousBoard = board;
    board[y][x] = blackTurn ? 1 : 2;
    lastMove = {x, y};
    blackTurn = !blackTurn;

    // Check for captures after placing the stone
    removeCapturedGroups(!blackTurn ? 1 : 2); // Remove captured groups of the other player

    printf("Move made at (%d, %d)\n", x, y);
    Invalidate();
  }

    bool isValidMove(int x, int y) {
//      return board[y][x] == 0; // just check if the spot is empty
        if (board[y][x] == 0) return true;

        // ... existing ko check ...

        int player = blackTurn ? 1 : 2;
        int opponent = blackTurn ? 2 : 1;

        // Check if placing a stone here would capture any opponent's groups
        bool wouldCapture = false;
        for (int dy = -1; dy <= 1; ++dy) {
            for (int dx = -1; dx <= 1; ++dx) {
                if (dx == 0 && dy == 0) continue;
                int nx = x + dx, ny = y + dy;
                if (ny >= 0 && ny < BOARD_SIZE && nx >= 0 && nx < BOARD_SIZE) {
                    if (board[ny][nx] == opponent) {
                        std::vector<std::pair<int, int>> group;
                        if (isCaptured(nx, ny, opponent, group)) {
                            wouldCapture = true;
                            // We don't remove here because we want to do it after placing the stone
                        }
                    }
                }
            }
        }

        // Check if the move would lead to suicide unless it captures
        if (!wouldCapture && !hasLiberty(x, y, player)) {
            return false;
        }

        return true;
    }

    void removeCapturedGroups(int color) {
        for (int y = 0; y < BOARD_SIZE; ++y) {
            for (int x = 0; x < BOARD_SIZE; ++x) {
                if (board[y][x] == color) {
                    std::vector<std::pair<int, int>> group;
                    if (isCaptured(x, y, color, group)) {
                        for (auto& p : group) {
                            board[p.second][p.first] = 0; // Remove the captured stone
                        }
                    }
                }
            }
        }
    }

bool isCapturedOld(int x, int y, int color, std::vector<std::pair<int, int>>& group) {
    if (x < 0 || x >= BOARD_SIZE || y < 0 || y >= BOARD_SIZE || board[y][x] != color) return false;

    if (std::ranges::find(group.begin(), group.end(), std::make_pair(x, y)) != group.end()) {
        return true; // Already visited this stone, part of a group
    }

    group.push_back(std::make_pair(x, y));
    int liberties = 0;
    int opponent = (color == 1) ? 2 : 1;

    for (int dy = -1; dy <= 1; ++dy) {
        for (int dx = -1; dx <= 1; ++dx) {
            if (dx == 0 && dy == 0) continue; // Skip the stone itself
            int nx = x + dx, ny = y + dy;
            if (ny >= 0 && ny < BOARD_SIZE && nx >= 0 && nx < BOARD_SIZE) {
                if (board[ny][nx] == 0) {
                    liberties++;
                } else if (board[ny][nx] == color && !isCaptured(nx, ny, color, group)) {
                    return false; // If any stone in the group isn't captured, the group isn't captured
                }
            }
        }
    }

    return liberties == 0; // If no liberties, the stone (and thus the group) is captured
}
bool isCapturedFuck(int x, int y, int color, std::vector<std::pair<int, int>>& group) {
    if (x < 0 || x >= BOARD_SIZE || y < 0 || y >= BOARD_SIZE || board[y][x] != color) return false;

    if (std::ranges::find(group.begin(), group.end(), std::make_pair(x, y)) != group.end()) {
        return true; // Already visited this stone, part of a group
    }

    group.push_back(std::make_pair(x, y));
    int liberties = 0;
    int opponent = (color == 1) ? 2 : 1;

    // Check for liberties around this stone
    for (int dy = -1; dy <= 1; ++dy) {
        for (int dx = -1; dx <= 1; ++dx) {
            if (dx == 0 && dy == 0) continue; // Skip the stone itself
            int nx = x + dx, ny = y + dy;
            if (ny >= 0 && ny < BOARD_SIZE && nx >= 0 && nx < BOARD_SIZE) {
                if (board[ny][nx] == 0) {
                    liberties++;
                }
            }
        }
    }

    // Special case for single stone capture - if no liberties, we're done
    if (group.size() == 1 && liberties == 0) {
        return true;
    }

    // For groups, we check the entire group's liberties
    for (auto& stone : group) {
        for (int dy = -1; dy <= 1; ++dy) {
            for (int dx = -1; dx <= 1; ++dx) {
                if (dx == 0 && dy == 0) continue; // Skip the stone itself
                int nx = stone.first + dx, ny = stone.second + dy;
                if (ny >= 0 && ny < BOARD_SIZE && nx >= 0 && nx < BOARD_SIZE) {
                    if (board[ny][nx] == 0) return false; // Found a liberty, not captured
                    if (board[ny][nx] == color && std::ranges::find(group.begin(), group.end(), std::make_pair(nx, ny)) == group.end()) {
                        if (!isCaptured(nx, ny, color, group)) return false; // If any part of the group isn't captured, return false
                    }
                }
            }
        }
    }

    return liberties == 0; // If no liberties found for the whole group, it's captured
}

bool isCaptured(int x, int y, int color, std::vector<std::pair<int, int>>& group) {
    if (x < 0 || x >= BOARD_SIZE || y < 0 || y >= BOARD_SIZE || board[y][x] != color) return false;

    group.push_back(std::make_pair(x, y));
    int opponent = (color == 1) ? 2 : 1;

    // Check if this single stone has no liberties
    if (group.size() == 1) {
        bool surrounded = true;
        for (int dy = -1; dy <= 1; ++dy) {
            for (int dx = -1; dx <= 1; ++dx) {
                if (dx == 0 && dy == 0) continue; // Skip the stone itself
                int nx = x + dx, ny = y + dy;
                if (ny >= 0 && ny < BOARD_SIZE && nx >= 0 && nx < BOARD_SIZE) {
                    if (board[ny][nx] == 0 || board[ny][nx] == color) {
                        surrounded = false;
                        break;
                    }
                }
            }
            if (!surrounded) break;
        }
        return surrounded; // If surrounded by opponent stones, it's captured
    }

    // For groups (which isn't the case here for single stone capture, but for completeness)
    int liberties = 0;
    for (auto& stone : group) {
        for (int dy = -1; dy <= 1; ++dy) {
            for (int dx = -1; dx <= 1; ++dx) {
                if (dx == 0 && dy == 0) continue; // Skip the stone itself
                int nx = stone.first + dx, ny = stone.second + dy;
                if (ny >= 0 && ny < BOARD_SIZE && nx >= 0 && nx < BOARD_SIZE) {
                    if (board[ny][nx] == 0) {
                        return false; // Found a liberty, not captured
                    }
                    if (board[ny][nx] == color && std::ranges::find(group.begin(), group.end(), std::make_pair(nx, ny)) == group.end()) {
                        if (!isCaptured(nx, ny, color, group)) return false;
                    }
                }
            }
        }
    }
    return true; // If no liberties found for the whole group (which won't happen for single stones), it's captured
}

    bool hasLiberty(int x, int y, int color) {
        std::vector<std::pair<int, int>> group;
        isCaptured(x, y, color, group);
        for (auto& p : group) {
            for (int dy = -1; dy <= 1; ++dy) {
                for (int dx = -1; dx <= 1; ++dx) {
                    if (dx == 0 && dy == 0) continue;
                    int nx = p.first + dx, ny = p.second + dy;
                    if (ny >= 0 && ny < BOARD_SIZE && nx >= 0 && nx < BOARD_SIZE && board[ny][nx] == 0) {
                        return true;
                    }
                }
            }
        }
        return false;
    }
};

class GoWindow : public BWindow {
public:
    GoWindow(BRect frame) : BWindow(frame, "Go Game", B_TITLED_WINDOW, B_QUIT_ON_WINDOW_CLOSE) {
        AddChild(new GoBoard(Bounds()));
    }
};

class GoApp : public BApplication {
public:
    GoApp() : BApplication("application/x-vnd.Haiku-GoGame") {
        GoWindow *window = new GoWindow(BRect(100, 100, (BOARD_SIZE + 2.5) * CELL_SIZE, (BOARD_SIZE + 2.5) * CELL_SIZE));
        window->Show();
    }
};

int main() {
    GoApp app;
    app.Run();
    return 0;
}
