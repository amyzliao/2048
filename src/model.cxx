#include "model.hxx"
#include <vector>
#include <cstdlib>

using namespace ge211;

Model::Model(int run_mode)
    // Model does not directly initialize game_over_status, board, or score
    // because that is all handled in new_game/test_lose_game/test_win_game.
    // Model does not directly initialize moving_blocks, that happens in move_block.
{
    if (run_mode == 0) {
        new_game();
    } else if (run_mode == 1) {
        test_lose_game();
    } else if (run_mode == 2) {
        test_win_game();
    }
}

void
Model::new_game() {
    game_over_status = 0;
    for (int y = 0; y < size; y++) {
        for (int x = 0; x < size; x++) {
            // 0 means empty
            board[y][x] = 0;
        }
    }
    score = 0;
    spawn_first();
    spawn();
}

void
Model::spawn_first()
{
    // pick random empty position
    Position rand_pos = rand_empty_pos();
    // put a 2 there
    board[rand_pos.y][rand_pos.x] = 2;
}

void
Model::spawn()
{
    Position rand_pos = rand_empty_pos();
    // pick a value; 2 has 75% chance, 4 has 25% chance
    ge211::Random_source<int> rng_num(1, 4);
    int rand_val;
    int rand_num = rng_num.next();
    if (rand_num == 1) {
        rand_val = 4;
    } else {
        rand_val = 2;
    }
    // fill random position with value
    board[rand_pos.y][rand_pos.x] = rand_val;
    // update new_spawn_pos
    new_spawn_pos = rand_pos;
}

Model::Position
Model::rand_empty_pos()
{
    // store all empty positions in a vector
    std::vector<Model::Position> empties;
    for (int y = 0; y < size; y++) {
        for (int x = 0; x < size; x++) {
            if (board[y][x] == 0) {
                empties.push_back(Position(x, y));
            }
        }
    }
    // return random empty position
    ge211::Random_source<int> rng_pos(0, empties.size() - 1);
    int rand_index = rng_pos.next();
    return empties[rand_index];
}


int
Model::get_val(Position p) const
{
    return board[p.y][p.x];
}

int
Model::get_size() const
{
    return size;
}

int
Model::get_score() const
{
    return score;
}

int
Model::get_game_over() const
{
    return game_over_status;
}


void
Model::play_move(Direction dir)
{
    // before playing a move, we clear the moving_blocks and new_merged vector arrays
    // because new_merged stores all the new merges within a single move
    // and moving_blocks also stores all the blocks that move within a single move.
    // moving_blocks will be used for animation!
    moving_blocks.clear();
    new_merged.clear();
    bool moved = false;

    // when we're moving blocks in a certain direction, we want to move the blocks
    // that are closest to the nearest "wall" to move first
    // i.e. when we're moving right, the blocks to the rightmost column move first

    // if you're going right
    if (dir.x == 1) {
        // column order: 2, 1, 0
        for (int c = size - 2; c >= 0; c--) {
            // row order: 0, 1, 2, 3
            for (int r = 0; r < size; r++) {
                if (board[r][c] != 0) {
                    if (move_block({c, r}, dir)) {
                        moved = true;
                    }
                 }
            }
        }
    }
    // if you're going left
    else if (dir.x == -1) {
        // column order: 1, 2, 3
        for (int c = 1; c < size; c++) {
            // row order: 0, 1, 2, 3
            for (int r = 0; r < size; r++) {
                if (board[r][c] != 0) {
                    if (move_block({c, r}, dir)) {
                        moved = true;
                    }
                }
            }
        }
    }
    // if you're going up
    else if (dir.y == -1) {
        // row order: 1, 2, 3
        for (int y = 1; y < size; y++) {
            // column order: 0, 1, 2, 3
            for (int x = 0; x < size; x++) {
                if (board[y][x] != 0) {
                    if (move_block({x, y}, dir)) {
                        moved = true;
                    }
                }
            }
        }
    }
    // if you're going down
    else if (dir.y == 1) {
        // row order: 2, 1, 0
        for (int y = size - 2; y >= 0; y--) {
            // column order: 0, 1, 2, 3
            for (int x = 0; x < size; x++) {
                if (board[y][x] != 0) {
                    if (move_block({x, y}, dir)) {
                        moved = true;
                    }
                }
            }
        }
    }
    // if something moved, spawn a new block and record direction
    if (moved) {
        spawn();
        direction = dir;
    }
    // update game_over_status
    game_over_status = is_game_over();
}

bool
Model::move_block(Position start, Direction dir)
{
    bool moved = false;
    // val stores value that is about to be moved
    int val = board[start.y][start.x];
    int end_val;
    Position curr = start;
    Position next{curr.x + dir.x, curr.y + dir.y};

    // while still within the board, keep moving the board until you
    // hit an existing block.
    // in other words, when the value at that board position is not 0.
    // if you hit a block that has the same value and HAS NOT already
    // been merged within the same move, you merge
    while (next.x < size && next.x >= 0 && next.y < size && next.y >= 0) {
        if (board[next.y][next.x] == 0) {
            end_val = 0;
            board[next.y][next.x] = val;
            board[curr.y][curr.x] = 0;
            moved = true;
            curr = next;
            next.x += dir.x;
            next.y += dir.y;
        } else if (board[next.y][next.x] == val && not already_merged(next)) {
            end_val = val;
            merge(curr, next);
            curr = next;
            moved = true;
            break;
        } else {
            break;
        }
    }

    if (moved) {
        // curr now holds the final destination
        moving_blocks.push_back(moving_block(start.into<float>(),
                                            curr.into<float>(),
                                            val,
                                            end_val));
    }

    return moved;
}

void
Model::merge(Model::Position start, Model::Position end)
{
    int val = board[start.y][start.x];
    board[start.y][start.x] = 0;
    board[end.y][end.x] = val * 2;
    score += val * 2;
    new_merged.push_back(end);
}

bool
Model::already_merged(Model::Position pos) const
{
    for (Position merged_p : new_merged) {
        if (merged_p == pos) {
            return true;
        }
    }
    return false;
}

int
Model::is_game_over() const
{
    bool move_exists = false;
    // check all tiles
    for (int y = 0; y < size; y++) {
        for (int x = 0; x < size; x++) {
            // if this tile has value 2048, return 2
            if (board[y][x] == 2048) {
                return 2;
            }
            // if this tile is empty space, you've not lost
            if (board[y][x] == 0) {
                move_exists = true;
            // otherwise, if this tile can merge, you've not lost
            } else if (merge_exists({x, y})) {
                move_exists = true;
            }
        }
    }
    if (move_exists) {
        return 0;
    } else {
        return 1;
    }
}

bool
Model::merge_exists(Position pos) const
{
    // -1, 0, 1
    for (int i = -1; i < 2; i++) {
        // -1, 0, 1
        for (int j = -1; j < 2; j++) {
            // skip diagonal and zero directions
            if (abs(i) == abs(j)) {
                continue;
            }
            // if there are two blocks in the vertical or horizontal direction that have the same value,
            // a merge exists
            Position next = {pos.x + j, pos.y + i};
            if (next.x < size && next.x >= 0 && next.y < size && next.y >= 0) {
                int val1 = board[pos.y][pos.x];
                int val2 = board[next.y][next.x];
                if (val1 == val2) {
                    return true;
                }
            }
        }
    }
    return false;
}

void Model::on_frame(double dt) {
    // set velocity according to direction of move
    if (abs(direction.x) == 1) {
        velocity = {float(direction.x) * speed, 0};
    } else if (abs(direction.y) == 1) {
        velocity = {0, float(direction.y) * speed};
    }

    // if a block is moving, update its position
    // if it's at or past the end position, update is_moving to false
    for (int i = 0; i < int(moving_blocks.size()); i++) {
        moving_blocks[i].increment_curr(dt * velocity);
        if (direction.x == 1) {
            if (moving_blocks[i].get_curr().x >= moving_blocks[i].get_end().x) {
                moving_blocks[i].set_is_moving(false);
            }
        } else if (direction.x == -1) {
            if (moving_blocks[i].get_curr().x <= moving_blocks[i].get_end().x) {
                moving_blocks[i].set_is_moving(false);
            }
        } else if (direction.y == 1) {
            if (moving_blocks[i].get_curr().y >= moving_blocks[i].get_end().y) {
                moving_blocks[i].set_is_moving(false);
            }
        } else if (direction.y == -1) {
            if (moving_blocks[i].get_curr().y <= moving_blocks[i].get_end().y) {
                moving_blocks[i].set_is_moving(false);
            }
        }
    }
}

void
Model::test_win_game() {
    game_over_status = 0;
    for (int y = 0; y < size; y++) {
        for (int x = 0; x < size; x++) {
            // 0 means empty
            board[y][x] = 0;
        }
    }
    board[1][1] = 1024;
    board[1][2] = 1024;
    board[2][1] = 512;
    board[2][2] = 128;
    board[3][1] = 64;
    board[3][2] = 16;
    score = 0;
    spawn();
}

void
Model::test_lose_game() {
    game_over_status = 0;
    /*
     * [   ][ 8 ][ 64][ 32]
     * [ 16][ 32][256][ 8 ]
     * [ 64][ 8 ][   ][ 16]
     * [ 4 ][ 32][512][ 64]
     */
    board[0][0] = 0;
    board[0][1] = 8;
    board[0][2] = 64;
    board[0][3] = 32;
    board[1][0] = 16;
    board[1][1] = 32;
    board[1][2] = 256;
    board[1][3] = 8;
    board[2][0] = 64;
    board[2][1] = 8;
    board[2][2] = 0;
    board[2][3] = 16;
    board[3][0] = 4;
    board[3][1] = 32;
    board[3][2] = 512;
    board[3][3] = 64;
    score = 0;
    spawn();
}