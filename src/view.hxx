#pragma once

#include "model.hxx"
#include <vector>

class View
{
public:
    // View positions and dimensions will use 'int' coordinates
    using Position = ge211::Posn<int>;
    using Dimensions = ge211::Dims<int>;
    using Color = ge211::Color;
    using Font = ge211::Font;

    /// CONSTRUCTOR
    // constructs a view that knows about the given model
    explicit View(Model const&);

    /// DRAW
    void draw(ge211::Sprite_set& set);

    /// WINDOW SETUP
    // returns the initial dimensions of the game window
    Dimensions initial_window_dimensions() const;
    // writes the title of the game window
    std::string initial_window_title() const;

    /// GETTERS
    // gets the screen positions of the new game button
    // item 0 is top left corner, item 1 is bottom right corner
    std::vector<Position> get_ngb_pos() const;

private:
    /// TOP-LEVEL PRIVATE MEMBER VARIABLES
    Model const& model_;
    // (length of) the width and height of one block
    static const int sqlen = 69;
    // height of the top margin of the screen
    // leaves space for game instructions, score, and new game button
    // to be displayed above the board.
    static const int top_margin = sqlen * 2;

    /// BOARD TO SCREEN CONVERTORS
    // takes a board position, returns the physical position (top-left corner)
    // of the corresponding block on the screen.
    Position
    board_to_screen(Model::Position board_pos) const;
    // takes a board position and block value, returns the physical position
    // (top-left corner) of the text that goes on that block. text is centered
    // in the block.
    Position
    board_to_screen_text(Model::Position, int) const;
    // takes a board position of a moving block, returns the physical
    // position (top-left corner) of the moving block.
    Position
    board_to_screen_a(ge211::Posn<float>) const;
    // takes a board position of a moving block, returns the physical
    // position (top-left corner) of the text that goes on the moving block.
    Position
    board_to_screen_text_a(ge211::Posn<float>, int) const;

    /// BLOCKS
    // font used on all blocks
    Font const block_font{"sans.ttf", 25};
    // dictionary: key = block value, value = block color
    // index 0 holds color for block value 0
    std::vector<Color> block_colors;
    // dictionary: key = block value, value = block sprite
    // index 0 holds block sprite for block value 0
    std::vector<ge211::Rectangle_sprite> block_sprites;
    // dictionary: key = block value, value = text sprite
    // index 0 holds text sprite for block value 2
    std::vector<ge211::Text_sprite> block_text_sprites;

    /// ANIMATION
    // these blocks are slightly smaller to account for the grid lines covering
    // up part of the blocks.
    std::vector<ge211::Rectangle_sprite> moving_block_sprites;

    /// GRID LINES
    // thickness of grid lines
    int grid_line_thick = 6;
    // thickness of border lines; half as thick as grid lines so that each
    // block is equally covered on each side.
    int border_line_thick = grid_line_thick / 2;
    // color of grid/border lines
    Color const line_color{117, 105, 95};
    // sprites for grid and border lines
    ge211::Rectangle_sprite const line_sprite_vert;
    ge211::Rectangle_sprite const line_sprite_hor;
    ge211::Rectangle_sprite const border_sprite_vert;
    ge211::Rectangle_sprite const border_sprite_hor;

    /// SCORE
    // the position of the word "SCORE: "
    Position const score_text_pos{int(10), int(double(top_margin) * 0.72)};
    // the position of the value of the score
    Position const score_val_pos{score_text_pos.x + 80, score_text_pos.y};
    // font of the score word and value
    Font const score_font{"sans.ttf", 20};
    // sprites for "SCORE: " and score value
    ge211::Text_sprite score_text;
    ge211::Text_sprite score_val;

    /// NEW GAME BUTTON
    // dimensions of the new game button
    Dimensions const new_game_button_dims{100, 25};
    // color of the new game button
    Color const new_game_color{207, 111, 33};
    // font of the word "NEW GAME" on the button
    Font const new_game_font{"sans.ttf", 15};
    // sprites for "NEW GAME" word, and rectangle button
    ge211::Rectangle_sprite const new_game_button;
    ge211::Text_sprite const new_game_text;

    /// GAME OVER
    // for lose: color of the semi-transparent screen that covers the board
    Color const lost_screen_color{56, 33, 7, 150};
    // for win: color of the semi-transparent screen that covers the board
    Color const won_screen_color{255, 135, 0, 100};
    // font of "GAME OVER" and "YOU WIN!" text
    Font const game_over_font{"sans.ttf", 30};
    // sprites for game over and you win screens, each composed of a
    // semi-transparent rectangle that covers the board and text that says
    // "GAME OVER" or "YOU WIN!"
    ge211::Rectangle_sprite const lost_screen;
    ge211::Rectangle_sprite const won_screen;
    ge211::Text_sprite const lost_text;
    ge211::Text_sprite const won_text;

    /// GAME INSTRUCTIONS
    // font of game instructions
    Font const game_instr_font{"sans.ttf", 13};
    // game instructions text sprite
    ge211::Text_sprite game_instr_text;
};