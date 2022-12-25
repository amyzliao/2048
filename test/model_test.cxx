#include "model.hxx"
#include <catch.hxx>

using namespace ge211;

struct Test_access {
    Model& model;
    explicit Test_access(Model&);

    // puts a value at a position
    void set_block(Model::Position pos, int val)
    {
        model.board[pos.y][pos.x] = val;
    }

    // clears the board; all positions set to 0
    void clear_board()
    {
        for (int y = 0; y < model.size; y++) {
            for (int x = 0; x < model.size; x++) {
                model.board[y][x] = 0;
            }
        }
    }

    // returns the number of nonzero blocks on the board
    int count_blocks()
    {
        int count = 0;
        for (int y = 0; y < model.size; y++) {
            for (int x = 0; x < model.size; x++) {
                if (model.board[y][x] != 0) {
                    count++;
                }
            }
        }
        return count;
    }

    // gets rid of the most recently spawned block
    void despawn() {
        set_block(model.get_new_spawn_pos(), 0);
    }
};

Test_access::Test_access(Model& model)
        : model(model)
{ }

TEST_CASE("move between two horizontal adjacent merges")
{
    Model model(0);
    Test_access t{model};

    // TEST 1: blocks of the same value merge when they "move into" each other.
    // all possible merges happen at the same time.
    // For ease of testing, we will assume no spawns occur.
    /* START
     * [ ][ ][ ][ ]
     * [ ][ ][ ][ ]
     * [ ][ ][ ][ ]
     * [2][2][4][4]
     *
     * move right
     *
     * END
     * [ ][ ][ ][ ]
     * [ ][ ][ ][ ]
     * [ ][ ][ ][ ]
     * [0][0][4][8]
     */

    // set up board
    t.clear_board();
    t.set_block({3, 3}, 4);
    t.set_block({2, 3}, 4);
    t.set_block({1, 3}, 2);
    t.set_block({0, 3}, 2);

    // go right
    model.play_move({1, 0});
    t.despawn();

    // check all positions
    for (int y = 0; y < model.get_size() - 1; y++) {
        for (int x = 0; x < model.get_size(); x++) {
            CHECK(model.get_val({x, y}) == 0);
        }
    }
    CHECK(model.get_val({0, 3}) == 0);
    CHECK(model.get_val({1, 3}) == 0);
    CHECK(model.get_val({2, 3}) == 4);
    CHECK(model.get_val({3, 3}) == 8);

    // merges happen in conjunction with normal block movement
    // all blocks that can move, move at the same time.
    /* START
     * [ ][4][ ][8]
     * [2][ ][16][ ]
     * [ ][4][ ][ ]
     * [ ][ ][4][8]
     *
     * move down
     *
     * END
     * [ ][ ][ ][ ]
     * [ ][ ][ ][ ]
     * [ ][ ][16][ ]
     * [2][8][4][16]
     */
    t.set_block({1, 0}, 4);
    t.set_block({3, 0}, 8);
    t.set_block({0, 1}, 2);
    t.set_block({2, 1}, 16);
    t.set_block({1, 2}, 4);

    model.play_move({0, 1});
    t.despawn();

    CHECK(model.get_val({0, 3}) == 2);
    CHECK(model.get_val({1, 3}) == 8);
    CHECK(model.get_val({2, 3}) == 4);
    CHECK(model.get_val({2, 2}) == 16);
    CHECK(model.get_val({3, 3}) == 16);

    for (int y = 0; y < model.get_size() - 2; y++) {
        for (int x = 0; x < model.get_size(); x++) {
            CHECK(model.get_val({x, y}) == 0);
        }
    }
    CHECK(model.get_val({0, 2}) == 0);
    CHECK(model.get_val({1, 2}) == 0);
    CHECK(model.get_val({3, 2}) == 0);
}

TEST_CASE("move between two vertical adjacent merges")
{
    // assumes no spawns occur
    /* START
     * [ ][ ][ ][2]
     * [ ][ ][ ][2]
     * [ ][ ][ ][4]
     * [ ][ ][ ][4]
     *
     * move up
     *
     * END
     * [ ][ ][ ][4]
     * [ ][ ][ ][8]
     * [ ][ ][ ][0]
     * [ ][ ][ ][0]
     */

    Model model(0);
    Test_access t{model};

    // set up board
    t.clear_board();
    t.set_block({3, 3}, 4);
    t.set_block({3, 2}, 4);
    t.set_block({3, 1}, 2);
    t.set_block({3, 0}, 2);

    // go up
    model.play_move({0, -1});

    // check all positions
    t.despawn();
    for (int y = 0; y < model.get_size(); y++) {
        for (int x = 0; x < model.get_size() - 1; x++) {
            CHECK(model.get_val({x, y}) == 0);
        }
    }
    CHECK(model.get_val({3, 3}) == 0);
    CHECK(model.get_val({3, 2}) == 0);
    CHECK(model.get_val({3, 1}) == 8);
    CHECK(model.get_val({3, 0}) == 4);
}

TEST_CASE("About to win") {
    Model model(0);
    Test_access t(model);

    // set 2048 block somewhere
    t.set_block({3, 3}, 2048);
    // play move to update get_game_over()
    model.play_move({1, 0});
    // check that get_game_over() returns 2, which means you've won the game
    CHECK(model.get_game_over() == 2);
}

TEST_CASE("Another about to win") {
    Model model(0);
    Test_access t(model);
    t.clear_board();
    CHECK(model.get_game_over() == 0);

    // check that score has nothing to do with win/lose
    t.set_block({3, 3}, 512);
    t.set_block({3, 2}, 512);
    t.set_block({2, 3}, 512);
    t.set_block({2, 2}, 512);
    model.play_move({0, 1});
    // check that even though score is 2048, game is still not over
    CHECK(model.get_score() == 2048);
    CHECK(model.get_game_over() == 0);

    t.clear_board();
    // set two adjacent 1024 blocks and see if game wins is triggered right
    // after they merge
    t.set_block({0, 0}, 1024);
    t.set_block({1, 0}, 1024);
    // go left
    model.play_move({-1, 0});
    t.despawn();
    CHECK(model.get_game_over() == 2);
}

TEST_CASE("About to win - run mode") {
    // sets up about to lose board with command argument 1
    Model model(2);
    Test_access t(model);

    // a left move wins the game
    CHECK(model.get_game_over() == 0);
    model.play_move({-1, 0});
    CHECK(model.get_game_over() == 2);
}

TEST_CASE("About to lose - run mode") {
    // sets up about to lose board with command argument 1
    Model model(1);
    // this sets the board up like this:
    /*
     * [ ? ][ 8 ][ 64][ 32]
     * [ 16][ 32][256][ 8 ]
     * [ 64][ 8 ][ ? ][ 16]
     * [ 4 ][ 32][512][ 64]
     *
     *    ? = something might have spawned here
     */
    Test_access t(model);

    // a left move ends the game
    CHECK(model.get_game_over() == 0);
    model.play_move({-1, 0});
    CHECK(model.get_game_over() == 1);
}

TEST_CASE("Score changes") {
    // assume no spawning

    Model model(0);
    Test_access t(model);

    // clear board
    t.clear_board();

    // when two blocks of value 2 merge, the score increases by 4
    CHECK(model.get_score() == 0);
    t.set_block({0, 0}, 2);
    t.set_block({0, 1}, 2);
    model.play_move({0, -1});
    CHECK(model.get_score() == 4);
    CHECK(model.get_val({0, 0}) == 4);

    // when two blocks of value 4 merge, score increases by 8
    t.clear_board();
    t.set_block({0, 0}, 4);
    t.set_block({3, 0}, 4);
    model.play_move({1, 0});
    CHECK(model.get_score() == 12);
    CHECK(model.get_val({3, 0}) == 8);

    // two merges happen at same time:
    // 8 merge with 8 increments score by 16
    // 4 merge with 4 increments score by 8
    t.clear_board();
    t.set_block({3, 0}, 8);
    t.set_block({3, 3}, 8);
    t.set_block({2, 0}, 4);
    t.set_block({2, 3}, 4);
    model.play_move({0, 1});
    CHECK(model.get_score() == 36);
    CHECK(model.get_val({3, 3}) == 16);
    CHECK(model.get_val({2, 3}) == 8);
}

TEST_CASE("A sample mini-game") {
    // test: multiple blocks on the board but only one merge occurs
    // test: whether all blocks move all the way down in direction directed
    Model model(0);
    Test_access t(model);

    // clear board
    t.clear_board();

    /* START
     * [ ][ ][ ][ ]
     * [ ][2][4][ ]
     * [ ][2][ ][4]
     * [ ][ ][ ][ ]
     *
     * move up
     *
     * END
     * [ ][4][4][4]
     * [ ][ ][ ][ ]
     * [ ][ ][ ][ ]
     * [ ][ ][ ][ ]
     */
    t.set_block({1, 1}, 2);
    t.set_block({1, 2}, 2);
    t.set_block({2, 1}, 4);
    t.set_block({3, 3}, 4);
    // go up
    model.play_move({0, -1});
    t.despawn();
    // checks
    for (int y = 1; y < model.get_size(); y++) {
        for (int x = 0; x < model.get_size(); x++) {
            CHECK(model.get_val({x, y}) == 0);
        }
    }
    CHECK(model.get_score() == 4);
    CHECK(model.get_val({0, 0}) == 0);
    CHECK(model.get_val({1, 0}) == 4);
    CHECK(model.get_val({2, 0}) == 4);
    CHECK(model.get_val({3, 0}) == 4);

    /*
     * move right
     *
     * END
     * [ ][ ][4][8]
     * [ ][ ][ ][ ]
     * [ ][ ][ ][ ]
     * [ ][ ][ ][ ]
     */
    // go right
    model.play_move({1, 0});
    t.despawn();
    // checks
    for (int y = 1; y < model.get_size(); y++) {
        for (int x = 0; x < model.get_size(); x++) {
            CHECK(model.get_val({x, y}) == 0);
        }
    }
    CHECK(model.get_score() == 12);
    CHECK(model.get_val({0, 0}) == 0);
    CHECK(model.get_val({1, 0}) == 0);
    CHECK(model.get_val({2, 0}) == 4);
    CHECK(model.get_val({3, 0}) == 8);

    // manually spawn new block
    /*
     * START
     * [4][ ][4][8]
     * [ ][ ][ ][ ]
     * [ ][ ][ ][ ]
     * [ ][ ][ ][ ]
     *
     * move left
     *
     * END
     * [8][8][ ][ ]
     * [ ][ ][ ][ ]
     * [ ][ ][ ][ ]
     * [ ][ ][ ][ ]
     */
    t.set_block({0, 0}, 4);
    // left
    model.play_move({-1, 0});
    t.despawn();
    // checks
    for (int y = 1; y < model.get_size(); y++) {
        for (int x = 0; x < model.get_size(); x++) {
            CHECK(model.get_val({x, y}) == 0);
        }
    }
    CHECK(model.get_score() == 20);
    CHECK(model.get_val({0, 0}) == 8);
    CHECK(model.get_val({1, 0}) == 8);
    CHECK(model.get_val({2, 0}) == 0);
    CHECK(model.get_val({3, 0}) == 0);

    // manually spawn new block
    /*
     * START
     * [8][8][ ][ ]
     * [ ][ ][ ][ ]
     * [4][ ][ ][ ]
     * [ ][ ][ ][ ]
     *
     * move down
     *
     * END
     * [8][8][ ][ ]
     * [ ][ ][ ][ ]
     * [4][ ][ ][ ]
     * [ ][ ][ ][ ]
     */
    t.set_block({0, 2}, 4);
    model.play_move({0, 1});
    t.despawn();
    /*
    * [ ][ ][ ][ ]
    * [ ][ ][ ][ ]
    * [8][ ][ ][ ]
    * [4][8][ ][ ]
    *
    */
    CHECK(model.get_score() == 20);
    for (int y = 0; y < model.get_size() - 2; y++) {
        for (int x = 0; x < model.get_size(); x++) {
            CHECK(model.get_val({x, y}) == 0);
        }
    }
    CHECK(model.get_val({0, 2}) == 8);
    CHECK(model.get_val({1, 2}) == 0);
    CHECK(model.get_val({2, 2}) == 0);
    CHECK(model.get_val({3, 2}) == 0);
    CHECK(model.get_val({0, 3}) == 4);
    CHECK(model.get_val({1, 3}) == 8);
    CHECK(model.get_val({2, 3}) == 0);
    CHECK(model.get_val({3, 3}) == 0);

    // no double merging in one move:
    /* START
     * [ ][ ][ ][ ]
     * [ ][ ][ ][4]
     * [ ][ ][ ][2]
     * [ ][ ][ ][2]
     *
     * move down
     *
     * END
     * [ ][ ][ ][ ]
     * [ ][ ][ ][ ]
     * [ ][ ][ ][4]
     * [ ][ ][ ][4]
     */
    t.clear_board();
    t.set_block({3, 1}, 4);
    t.set_block({3, 2}, 2);
    t.set_block({3, 3}, 2);
    // move down
    model.play_move({0, 1});
    t.despawn();
    // checks
    for (int y = 0; y < model.get_size(); y++) {
        for (int x = 0; x < model.get_size() - 1; x++) {
            CHECK(model.get_val({x, y}) == 0);
        }
    }
    CHECK(model.get_val({3, 0}) == 0);
    CHECK(model.get_val({3, 1}) == 0);
    CHECK(model.get_val({3, 2}) == 4);
    CHECK(model.get_val({3, 3}) == 4);
}

TEST_CASE("No blocks can move (but not game over)") {
    // when you try to move in a direction, but no blocks can move in that direction,
    // the board does not change. no blocks spawn.
    /* START
     * [2][4][8][ ]
     * [16][8][4][ ]
     * [8][ ][ ][ ]
     * [4][8][ ][ ]
     *
     * move left
     *
     * END
     * {same as start}
     */
    Model model(0);
    Test_access t(model);

    t.clear_board();

    t.set_block({0, 0}, 2);
    t.set_block({1, 0}, 4);
    t.set_block({2, 0}, 8);
    t.set_block({0, 1}, 16);
    t.set_block({1, 1}, 8);
    t.set_block({2, 1}, 4);
    t.set_block({0, 2}, 8);
    t.set_block({0, 3}, 4);
    t.set_block({1, 3}, 8);

    model.play_move({-1, 0});
    // check that all the blocks didn't move
    CHECK(model.get_val({0, 0}) == 2);
    CHECK(model.get_val({1, 0}) == 4);
    CHECK(model.get_val({2, 0}) == 8);
    CHECK(model.get_val({0, 1}) == 16);
    CHECK(model.get_val({1, 1}) == 8);
    CHECK(model.get_val({2, 1}) == 4);
    CHECK(model.get_val({0, 2}) == 8);
    CHECK(model.get_val({0, 3}) == 4);
    CHECK(model.get_val({1, 3}) == 8);
    // check that no new blocks spawned
    CHECK(model.get_val({3, 0}) == 0);
    CHECK(model.get_val({3, 1}) == 0);
    CHECK(model.get_val({1, 2}) == 0);
    CHECK(model.get_val({2, 2}) == 0);
    CHECK(model.get_val({3, 2}) == 0);
    CHECK(model.get_val({2, 3}) == 0);
    CHECK(model.get_val({3, 3}) == 0);
}

TEST_CASE("Spawn") {
    Model model(0);
    Test_access t(model);

    // check that blocks spawn when move is valid
    /* START
     * [ ][ ][ ][ ]
     * [ ][ ][2][ ]
     * [4][ ][ ][ ]
     * [ ][ ][ ][ ]
     *
     * move down
     * END
     * [ ][ ][ ][ ]
     * [ ][ ][ ][ ]
     * [ ][ ][ ][ ]
     * [4][ ][2][ ]
     *     + one new block somewhere
     */
    t.clear_board();
    t.set_block({2, 1}, 2);
    t.set_block({0, 2}, 4);
    CHECK(t.count_blocks() == 2);

    model.play_move({0, 1});

    CHECK(model.get_val({0, 3}) == 4);
    CHECK(model.get_val({2, 3}) == 2);
    CHECK(t.count_blocks() == 3); // additional block somewhere

    // check blocks spawn if move is valid
    /* START
     * [ ][2][ ][ ]
     * [4][ ][ ][ ]
     * [8][ ][ ][ ]
     * [ ][ ][ ][ ]
     *
     * move left
     *
     * END
     * [2][ ][ ][ ]
     * [4][ ][ ][ ]
     * [8][ ][ ][ ]
     * [ ][ ][ ][ ]
     *      + one new block somewhere
    */
    t.clear_board();
    t.set_block({1, 0}, 2);
    t.set_block({0, 1}, 4);
    t.set_block({0, 2}, 8);
    CHECK(t.count_blocks() == 3);
    model.play_move({-1, 0});
    CHECK(t.count_blocks() == 4);

    // check no blocks spawn if move is invalid (empty board)
    t.clear_board();
    CHECK(t.count_blocks() == 0);
    model.play_move({1, 0});
    CHECK(t.count_blocks() == 0);
}