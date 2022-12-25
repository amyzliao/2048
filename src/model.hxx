#pragma once

#include <ge211.hxx>
#include <vector>

using namespace ge211;

class Model
{
public:
    // direction of move
    using Direction = ge211::Posn<int>;
    // position of block
    using Position = ge211::Posn<int>;

    /// CONSTRUCTOR
    // makes a new game:
    // run_mode = 0, starts a normal game.
    // run_mode = 1, starts a game with nearly full board (to test lose functionality)
    // run_mode = 2, starts a game with two 1024 blocks (to test win functionality)
    explicit Model(int run_mode);

    /// GETTERS
    // gets value at a position in board[][]
    int get_val(Position) const;
    // gets size of square board (number of rows and columns)
    int get_size() const;
    // gets current score of the game
    int get_score() const;
    // gets the status of the game (returns game_over_status, 0 if moves are possible, 1 if lost, 2 if won)
    int get_game_over() const;

    /// GAMEPLAY CONTROLS
    // plays one move equivalent to pressing an arrow key
    void play_move(Direction);
    // clears board and returns to default setting of two randomly spawned blocks
    // with value 2 or 4
    void new_game();

    /// TEST WIN/LOSE
    // new game with two 1024 blocks on the board
    void test_win_game();
    // new game with almost full board
    void test_lose_game();

#ifdef CS211_TESTING
    // When this class is compiled for testing, members of a struct named
    // Test_access will be allowed to access private members of this class.
    friend struct Test_access;
#endif

private:
    /// TOP-LEVEL PRIVATE MEMBER VARIABLES
    // size of board (number of rows and columns)
    static const int size = 4;
    // 2D array which stores the state of the board. each bucket is a block, which
    // is an int that represents the value of the block (0, 2, 4, 8, 16, etc)
    // 0 means that space is empty.
    int board[size][size];
    // keeps track of the value of the score, the sum total of the values of all
    // blocks created by merging.
    int score;

    /// BLOCK SPAWNING
    // spawns the first block of value 2 in a random position on the board.
    void spawn_first();
    // spawns a block of value 2 or 4 in a random position on the board,
    // with a 75% chance of 2 and 25% chance of 4.
    void spawn();
    // returns a randomly picked empty position on the board.
    Position rand_empty_pos();

    /// BLOCK MOVEMENT
    // returns true if a block moved (either by normal movement or
    // merging), false otherwise
    bool move_block(Position, Direction);
    // merges two blocks
    void merge(Position, Position);
    // stores the positions of newly made blocks (via merging) in one turn.
    // purpose: if a block was made by merging, no other blocks can merge
    // with it during the same turn.
    std::vector<Position> new_merged;
    // returns true if Position is found in new_merged.
    bool already_merged(Position) const;

    /// GAME OVER STATUS
    // stores the state of the game (value of is_game_over)
    int game_over_status;
    // returns 0 if moves are possible, 1 if lost, 2 if won
    int is_game_over() const;
    // returns true if block at Position is able to merge with any other blocks.
    // checks to see if there is a block with the same value one block up, down,
    // left, or right of block at Position.
    bool merge_exists(Position) const;

    /// ANIMATION (PRIVATE)
    // stores the direction of the last successful move
    Position direction {0, 0};
    // the speed at which blocks move
    float speed = 20;
    // stores the velocity of the blocks (on_frame sets velocity based on direction)
    ge211::Dims<float> velocity {0, 0};
    // a moving_block is a block that is moving this turn
    struct moving_block
    {
    private:
        // private member variables
        bool is_moving = true; // is the block still moving or has it reached its end position?
        ge211::Posn<float> start; // starting board position of block
        ge211::Posn<float> end; // ending board position of block
        ge211::Posn<float> curr; // current board position of block
        int value; // value of the block; 2, 4, 8, 16, 32, etc...
        int end_val; // value of the previous block at end position (before this block moved there)
    public:
        // constructor
        moving_block(ge211::Posn<float> start,ge211::Posn<float> end, int val, int end_val)
                : start(start),
                  end(end),
                  curr(start),
                  value(val),
                  end_val(end_val)
        {}

        // getters
        bool get_is_moving() const {
            return is_moving;
        }
        ge211::Posn<float> get_start() const {
            return start;
        }
        ge211::Posn<float> get_end() const {
            return end;
        }
        ge211::Posn<float> get_curr() const {
            return curr;
        }
        int get_val() const {
            return value;
        }
        int get_end_val() const {
            return end_val;
        }

        // setters/updaters
        void increment_curr(ge211::Dims<float> inc) {
            curr += inc;
        }
        void set_is_moving(bool b) {
            is_moving = b;
        }
    };
    // stores all the moving blocks for one turn
    std::vector<moving_block> moving_blocks;
    // stores the board position of the newly spawned block each move
    Position new_spawn_pos {size + 10, size + 10};

public:
    /// ANIMATION (PUBLIC)
    // update each moving_block's current position based on dt and velocity
    void on_frame(double dt);
    // get the moving_blocks vector
    std::vector<moving_block> get_moving_blocks() const {
        return moving_blocks;
    }
    // get the position of the newly spawned block this move
    Position get_new_spawn_pos() const {
        return new_spawn_pos;
    }
};