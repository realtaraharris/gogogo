#include <gtest/gtest.h>
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
/*
TEST(GoEngineTest, CaptureStone) {
    GoEngine engine(9);
    // set up a capture scenario:
    // B - -
    // B W -
    // W - -
engine.printBoard();

    engine.placeStone(0, 0, BLACK); // B
engine.printBoard();
    engine.placeStone(1, 1, WHITE); // W
engine.printBoard();
    engine.placeStone(0, 1, BLACK); // B
engine.printBoard();
    engine.placeStone(0, 2, WHITE); // W

engine.printBoard();

    EXPECT_TRUE(engine.getStoneAt(0, 0) == BLACK);
    EXPECT_TRUE(engine.getStoneAt(1, 1) == WHITE);
    EXPECT_TRUE(engine.getStoneAt(0, 1) == BLACK);
    EXPECT_TRUE(engine.getStoneAt(0, 2) == WHITE);

    EXPECT_EQ(engine.countLiberties(1, 1, WHITE), 3); // white stone has 2 liberties
    EXPECT_EQ(engine.countLiberties(0, 2, WHITE), 1); // white stone has 1 liberties

    EXPECT_EQ(engine.countLiberties(0, 0, BLACK), 1); // black stone group has 1 liberties
    EXPECT_EQ(engine.countLiberties(0, 1, BLACK), 1); // black stone group has 1 liberties

    engine.passTurn(BLACK);

    engine.placeStone(1, 0, WHITE); // place white to capture
    EXPECT_FALSE(engine.getStoneAt(0, 0) == BLACK); // black stone should be captured
    EXPECT_FALSE(engine.getStoneAt(0, 1) == BLACK); // black stone should be captured
}
*/
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
    EXPECT_EQ(engine.getStoneAt(0, 1), EMPTY);
    EXPECT_EQ(engine.getStoneAt(0, 2), EMPTY);
    EXPECT_EQ(engine.getStoneAt(1, 0), EMPTY);
}
/*
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

    EXPECT_TRUE(engine.isStoneAt(0, 0, BLACK));
    EXPECT_TRUE(engine.isStoneAt(1, 1, WHITE));
    EXPECT_TRUE(engine.isStoneAt(0, 1, BLACK));
    EXPECT_TRUE(engine.isStoneAt(0, 2, WHITE));

    EXPECT_TRUE(engine.isStoneAt(1, 0, BLACK));
    EXPECT_TRUE(engine.isStoneAt(1, 2, WHITE));
    EXPECT_TRUE(engine.isStoneAt(2, 0, BLACK));
    EXPECT_TRUE(engine.isStoneAt(2, 2, WHITE));

    EXPECT_EQ(engine.countLiberties(1, 1), 1); // white stone group has 1 liberty
    EXPECT_EQ(engine.countLiberties(0, 2), 1); // white stone group has 1 liberty

    EXPECT_EQ(engine.countLiberties(0, 0), 1); // black stone group has 1 liberty
    EXPECT_EQ(engine.countLiberties(0, 1), 1); // black stone group has 1 liberty

    engine.placeStone(2, 1, BLACK); // place black to capture

    EXPECT_FALSE(engine.isStoneAt(0, 0, BLACK)); // black stones should be captured
    EXPECT_FALSE(engine.isStoneAt(1, 0, BLACK)); // black stones should be captured
    EXPECT_FALSE(engine.isStoneAt(2, 0, BLACK)); // black stones should be captured
    EXPECT_FALSE(engine.isStoneAt(0, 1, BLACK)); // black stones should be captured

    EXPECT_TRUE(engine.isStoneAt(1, 1, WHITE)); // white stones should remain
    EXPECT_TRUE(engine.isStoneAt(0, 2, WHITE));
    EXPECT_TRUE(engine.isStoneAt(1, 2, WHITE));
    EXPECT_TRUE(engine.isStoneAt(2, 2, WHITE));
}
*/

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

    engine.printBoard();
    engine.placeStone(2, 2, BLACK); // place black stone, capturing white at (3, 2)
    engine.printBoard();

    EXPECT_EQ(engine.getStoneAt(2, 2), BLACK); // black stone should be there
    engine.printBoard();

    EXPECT_EQ(engine.getStoneAt(3, 2), WHITE); // white stone should be captured

    // black should not be able to recapture immediately at (2, 2)
    EXPECT_FALSE(engine.isValidMove(2, 2, BLACK)); // this move should not be valid due to ko rule

    // however, if white plays elsewhere, black should be able to recapture.
    engine.placeStone(0, 0, WHITE); // white plays elsewhere
    engine.printBoard();
    engine.placeStone(0, 5, BLACK); // white plays elsewhere
    engine.printBoard();
    EXPECT_TRUE(engine.isValidMove(3, 2, WHITE)); // now black can recapture

    // place black stone to recapture
    engine.placeStone(3, 2, WHITE);
    engine.printBoard();
    std::cout << "engine.getStoneAt(3, 2): " << engine.getStoneAt(3, 2) << std::endl;
    EXPECT_EQ(engine.getStoneAt(3, 2), WHITE);
    EXPECT_EQ(engine.getStoneAt(2, 2), EMPTY); // white stone at (2, 2) should be captured now

    // now white should not be able to immediately recapture at (2, 2) due to ko
    EXPECT_FALSE(engine.isValidMove(2, 2, BLACK)); // this move should not be valid due to ko rule
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}
