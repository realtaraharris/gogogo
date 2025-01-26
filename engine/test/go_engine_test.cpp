#include <gtest/gtest.h>
#include "go_engine.hpp"

TEST(GoEngineTest, BoardInitialization) {
    GoEngine engine(9);
    EXPECT_EQ(engine.getBoardSize(), 9);
    for (int i = 0; i < 9; ++i) {
        for (int j = 0; j < 9; ++j) {
            EXPECT_FALSE(engine.isStoneAt(i, j, true)); // no black stones
            EXPECT_FALSE(engine.isStoneAt(i, j, false)); // no white stones
        }
    }
}

TEST(GoEngineTest, PlaceStone) {
    GoEngine engine(5);
    engine.placeStone(2, 2, true);  // place a black stone
    EXPECT_TRUE(engine.isStoneAt(2, 2, true));
    EXPECT_FALSE(engine.isStoneAt(2, 2, false)); // no white stone at this position
}

TEST(GoEngineTest, InvalidMoveSamePosition) {
    GoEngine engine(5);
    engine.placeStone(2, 2, true); // place black stone
    EXPECT_FALSE(engine.isValidMove(2, 2, true)); // black cannot place here again
    EXPECT_FALSE(engine.isValidMove(2, 2, false)); // white cannot place here
}

TEST(GoEngineTest, ValidMoveNextToOpponent) {
    GoEngine engine(5);
    // - - - - -
    // - - - - -
    // - - B w -
    // - - w - -
    // - - - - -
    engine.placeStone(2, 2, true); // place black stone
    EXPECT_TRUE(engine.isValidMove(2, 3, false)); // valid for white next to black
    EXPECT_TRUE(engine.isValidMove(3, 2, false)); // valid for white next to black
}

TEST(GoEngineTest, CaptureStone) {
    GoEngine engine(3);
    // set up a capture scenario:
    // B - -
    // B W -
    // W - -

    engine.placeStone(0, 0, true); // B
    engine.placeStone(1, 1, false); // W
    engine.placeStone(0, 1, true); // B
    engine.placeStone(0, 2, false); // W

    EXPECT_TRUE(engine.isStoneAt(0, 0, true));
    EXPECT_TRUE(engine.isStoneAt(1, 1, false));
    EXPECT_TRUE(engine.isStoneAt(0, 1, true));
    EXPECT_TRUE(engine.isStoneAt(0, 2, false));

    EXPECT_EQ(engine.countLiberties(1, 1), 3); // white stone has 2 liberties
    EXPECT_EQ(engine.countLiberties(0, 2), 1); // white stone has 1 liberties

    EXPECT_EQ(engine.countLiberties(0, 0), 1); // black stone group has 1 liberties
    EXPECT_EQ(engine.countLiberties(0, 1), 1); // black stone group has 1 liberties

    engine.placeStone(1, 0, false); // place white to capture
    EXPECT_FALSE(engine.isStoneAt(0, 0, true)); // black stone should be captured
    EXPECT_FALSE(engine.isStoneAt(0, 1, true)); // black stone should be captured
}

TEST(GoEngineTest, TKO) {
    GoEngine engine(9);
    // set up a capture scenario:
    // B B B W
    // B W w W
    // W W W W

    engine.placeStone(0, 0, true); // B
    engine.placeStone(1, 1, false); // W
    engine.placeStone(0, 1, true); // B
    engine.placeStone(0, 2, false); // W

    engine.placeStone(1, 0, true); // B
    engine.placeStone(1, 2, false); // W
    engine.placeStone(2, 0, true); // B
    engine.placeStone(2, 2, false); // W

    engine.placeStone(3, 0, false); // W
    engine.placeStone(3, 1, false); // W
    engine.placeStone(3, 2, false); // W

    EXPECT_TRUE(engine.isStoneAt(0, 0, true));
    EXPECT_TRUE(engine.isStoneAt(1, 1, false));
    EXPECT_TRUE(engine.isStoneAt(0, 1, true));
    EXPECT_TRUE(engine.isStoneAt(0, 2, false));

    EXPECT_TRUE(engine.isStoneAt(1, 0, true));
    EXPECT_TRUE(engine.isStoneAt(1, 2, false));
    EXPECT_TRUE(engine.isStoneAt(2, 0, true));
    EXPECT_TRUE(engine.isStoneAt(2, 2, false));

    EXPECT_TRUE(engine.isStoneAt(3, 0, false));
    EXPECT_TRUE(engine.isStoneAt(3, 1, false));
    EXPECT_TRUE(engine.isStoneAt(3, 2, false));

    EXPECT_EQ(engine.countLiberties(1, 1), 8); // white stone group has 1 liberty
    EXPECT_EQ(engine.countLiberties(0, 2), 8); // white stone group has 1 liberty

    EXPECT_EQ(engine.countLiberties(0, 0), 1); // black stone group has 1 liberty
    EXPECT_EQ(engine.countLiberties(0, 1), 1); // black stone group has 1 liberty

    engine.placeStone(2, 1, false); // place white to capture

    EXPECT_FALSE(engine.isStoneAt(0, 0, true)); // white stone should be captured
    EXPECT_FALSE(engine.isStoneAt(0, 2, true)); // white stone should be captured
}

TEST(GoEngineTest, TotalTKO) {
    GoEngine engine(3);
    // set up a capture scenario:
    // B B B
    // B W b
    // W W W

    engine.placeStone(0, 0, true); // B
    engine.placeStone(1, 1, false); // W
    engine.placeStone(0, 1, true); // B
    engine.placeStone(0, 2, false); // W

    engine.placeStone(1, 0, true); // B
    engine.placeStone(1, 2, false); // W
    engine.placeStone(2, 0, true); // B
    engine.placeStone(2, 2, false); // W

    EXPECT_TRUE(engine.isStoneAt(0, 0, true));
    EXPECT_TRUE(engine.isStoneAt(1, 1, false));
    EXPECT_TRUE(engine.isStoneAt(0, 1, true));
    EXPECT_TRUE(engine.isStoneAt(0, 2, false));

    EXPECT_TRUE(engine.isStoneAt(1, 0, true));
    EXPECT_TRUE(engine.isStoneAt(1, 2, false));
    EXPECT_TRUE(engine.isStoneAt(2, 0, true));
    EXPECT_TRUE(engine.isStoneAt(2, 2, false));

    EXPECT_EQ(engine.countLiberties(1, 1), 1); // white stone group has 1 liberty
    EXPECT_EQ(engine.countLiberties(0, 2), 1); // white stone group has 1 liberty

    EXPECT_EQ(engine.countLiberties(0, 0), 1); // black stone group has 1 liberty
    EXPECT_EQ(engine.countLiberties(0, 1), 1); // black stone group has 1 liberty

    engine.placeStone(2, 1, true); // place black to capture

    EXPECT_FALSE(engine.isStoneAt(0, 0, true)); // black stones should be captured
    EXPECT_FALSE(engine.isStoneAt(1, 0, true)); // black stones should be captured
    EXPECT_FALSE(engine.isStoneAt(2, 0, true)); // black stones should be captured
    EXPECT_FALSE(engine.isStoneAt(0, 1, true)); // black stones should be captured

    EXPECT_TRUE(engine.isStoneAt(1, 1, false)); // white stones should remain
    EXPECT_TRUE(engine.isStoneAt(0, 2, false));
    EXPECT_TRUE(engine.isStoneAt(1, 2, false));
    EXPECT_TRUE(engine.isStoneAt(2, 2, false));
}

TEST(GoEngineTest, NoSelfCapture) {
    GoEngine engine(3);
    // Set up:
    // B B B
    // - W B
    // B B B
    engine.placeStone(0, 0, true); // B
    engine.placeStone(1, 0, true); // B

    engine.placeStone(2, 0, true); // B
    engine.placeStone(2, 1, true); // B
    engine.placeStone(2, 2, true); // B

    engine.placeStone(1, 2, true); // B
    engine.placeStone(0, 2, true); // B

    engine.placeStone(1, 1, false); // W

    EXPECT_FALSE(engine.isValidMove(1, 2, false)); // white cannot self-capture
}

TEST(GoEngineTest, KoRule) {
    GoEngine engine(6);

    // - - - - - -
    // - - W B - -
    // - W b W B -
    // - - W B - -
    // - - - - - -
    // - - - - - -

    engine.placeStone(2, 1, false); // place white stone
    engine.placeStone(3, 3, true); // place black stone
    engine.placeStone(2, 3, false); // place white stone
    engine.placeStone(3, 1, true); // place black stone
    engine.placeStone(1, 2, false); // place white stone
    engine.placeStone(4, 2, true); // place black stone
    engine.placeStone(3, 2, false); // place white stone

    engine.printBoard();
    engine.placeStone(2, 2, true); // place black stone, capturing white at (3, 2)
    engine.printBoard();

    EXPECT_TRUE(engine.isStoneAt(2, 2, true)); // black stone should be there
    engine.printBoard();

    EXPECT_FALSE(engine.isStoneAt(3, 2, false)); // white stone should be captured

    // black should not be able to recapture immediately at (2, 2)
    EXPECT_FALSE(engine.isValidMove(2, 2, true)); // this move should not be valid due to ko rule

    // however, if white plays elsewhere, black should be able to recapture.
    engine.placeStone(0, 0, false); // white plays elsewhere
    engine.printBoard();
    engine.placeStone(0, 5, true); // white plays elsewhere
    engine.printBoard();
    EXPECT_TRUE(engine.isValidMove(3, 2, false)); // now black can recapture

    // place black stone to recapture
    engine.placeStone(3, 2, false);
    engine.printBoard();
    EXPECT_TRUE(engine.isStoneAt(3, 2, false));
    EXPECT_FALSE(engine.isStoneAt(2, 3, true)); // white stone at (2, 3) should be captured now

    // now white should not be able to immediately recapture at (2, 2) due to ko
    EXPECT_FALSE(engine.isValidMove(2, 2, true)); // this move should not be valid due to ko rule
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}
