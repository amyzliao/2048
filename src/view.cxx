#include "view.hxx"
#include <vector>
#include <cmath>
#include <string>

using Color = ge211::Color;
using Font = ge211::Font;
using Sprite_set = ge211::Sprite_set;

View::View(Model const& model)
        : model_(model),
          line_sprite_vert(Dimensions(grid_line_thick,
                                      initial_window_dimensions()
                                      .height - top_margin),
                           line_color),
          line_sprite_hor(Dimensions(initial_window_dimensions().width,
                                     grid_line_thick),
                          line_color),
          border_sprite_vert(Dimensions(border_line_thick,
                                        initial_window_dimensions().height -
                                        top_margin),
                             line_color),
          border_sprite_hor(Dimensions(initial_window_dimensions().width,
                                       border_line_thick),
                            line_color),
          score_text("SCORE:", score_font),
          new_game_button(new_game_button_dims, new_game_color),
          new_game_text("NEW GAME", new_game_font),
          lost_screen(Dimensions(initial_window_dimensions().width,
                                      initial_window_dimensions().height - top_margin),
                       lost_screen_color),
          won_screen(Dimensions(initial_window_dimensions().width,
                                 initial_window_dimensions().height - top_margin),
                      won_screen_color),
          lost_text("GAME OVER", game_over_font),
          won_text("YOU WIN!", game_over_font),
          game_instr_text("placeholder", game_instr_font)
{
    // initialize block colors. index 0 = block 0.
    block_colors.push_back(Color {181, 165, 152}); // 0 - light brown/grey
    block_colors.push_back(Color {243, 207, 198}); // 2 - millennial pink
    block_colors.push_back(Color {248, 200, 220}); // 4 - pastel pink
    block_colors.push_back(Color {255, 192, 203}); // 8 - pink
    block_colors.push_back(Color {250, 160, 160}); // 16 - Pastel red
    block_colors.push_back(Color {248, 131, 121}); // 32 - Coral pink
    block_colors.push_back(Color {245, 130, 91}); // 64 - Salmon-ish
    block_colors.push_back(Color {255, 141, 59}); // 128 - Coral
    block_colors.push_back(Color {242, 154, 40}); // 256 - Cadmiun orange
    block_colors.push_back(Color {255, 172, 28}); // 512 - Bright orange
    block_colors.push_back(Color {255, 192, 0}); // 1024 - Golden Yellow
    block_colors.push_back(Color {71, 123, 255}); // 2048 - blue

    // initialize block sprites and moving block sprites. index 0 = block 0.
    for (int i = 0; i < 12; i++) {
        block_sprites.push_back(ge211::Rectangle_sprite({sqlen, sqlen},
                                                       block_colors[i]));
        // note that moving blocks are smaller than normal blocks to fit within the grid lines
        moving_block_sprites.push_back(ge211::Rectangle_sprite({sqlen - grid_line_thick,
                                                                sqlen - grid_line_thick},
                                                             block_colors[i]));
    }

    // initialize text sprites. index 0 = block 2, index 1 = block 4, etc.
    for (int j = 1; j < 12; j++) {
        int val = pow(2, j);
        block_text_sprites.push_back(ge211::Text_sprite(std::to_string(val),
                                                      block_font));
    }
}


void
View::draw(ge211::Sprite_set& set)
{
    /// STACKING ORDER
    int base_z = 1; // instructions, new game, score, blocks
    int block_cover_z = base_z + 5; // empty block to cover the base block
    int lines_z = block_cover_z + 5;
    int moving_block_z = lines_z + 5;
    int game_over_z = moving_block_z + 10;

    // add game instructions!
    ge211::Text_sprite::Builder builder(game_instr_font);
    // because the game instructions are bit long, we need to wrap it so that there is a margin between
    // the instructions and the edge of the game window
    builder.word_wrap(initial_window_dimensions().width - 20);
    // the actual game instructions:
    builder.add_message("HOW TO PLAY: Use your arrow keys to move the tiles. "
                        "Tiles with the same number merge into one. "
                        "Add them up to reach 2048!");
    builder.color(Color {255, 230, 223});
    // building the instructions sprite with our customizations from before:
    game_instr_text.reconfigure(builder);
    set.add_sprite(game_instr_text, {10, 10}, 3);

    // add new game button and its text centered within the button
    Position ngb_pos = get_ngb_pos()[0];
    set.add_sprite(new_game_button, ngb_pos, base_z);
    Position ngt_pos {ngb_pos.x + 8, ngb_pos.y - 1};
    set.add_sprite(new_game_text, ngt_pos, base_z + 1);

    // add score sprite
    set.add_sprite(score_text, score_text_pos, base_z);
    // where the score value comes from the current score of the game
    score_val = ge211::Text_sprite(std::to_string(model_.get_score()),
                                   score_font);
    set.add_sprite(score_val, score_val_pos, base_z);

    // add block sprites
    // if the value of the board position is zero, add an empty block.
    // if the value of the board position is nonzero, add corresponding block
    // by taking the log2() of the value to get the index of block_sprites.
    for (int i = 0; i < model_.get_size(); i++) {
        for (int j = 0; j < model_.get_size(); j++) {
            Position board_pos = Position(i,j);
            Position screen_pos = board_to_screen(board_pos);
            int value = model_.get_val(board_pos);
            if (value == 0) {
                set.add_sprite(block_sprites[0], screen_pos, base_z);
            } else {
                double block_index = log2(value);
                double text_index = block_index - 1;
                set.add_sprite(block_sprites[int(block_index)], screen_pos,
                               base_z);
                Position screen_text_pos = board_to_screen_text(board_pos, value);
                set.add_sprite(block_text_sprites[int(text_index)],
                               screen_text_pos,
                               base_z + 1);
            }
        }
    }

    // animation
    for (int i = 0; i < int(model_.get_moving_blocks().size()); i++) {
        // if blocks are moving, draw moving block, cover previous end block, cover newly
        // spawned block.
        if (model_.get_moving_blocks()[i].get_is_moving()) {
            double block_index = log2(model_.get_moving_blocks()[i].get_val());
            double text_index = block_index - 1;
            // draw moving block + text
            set.add_sprite(moving_block_sprites[int(block_index)],
                           board_to_screen_a(model_.get_moving_blocks()[i].get_curr()),
                           moving_block_z);
            Position screen_text_pos = board_to_screen_text_a(model_.get_moving_blocks()[i].get_curr(),
                                                              model_.get_moving_blocks()[i].get_val());
            set.add_sprite(block_text_sprites[int(text_index)],
                           screen_text_pos,
                           moving_block_z + 1);
            // cover new end block
            if (model_.get_moving_blocks()[i].get_end_val() == 0) {
                set.add_sprite(block_sprites[0],
                               board_to_screen(model_.get_moving_blocks()[i].get_end().into<int>()),
                               block_cover_z + 2);
            } else {
                set.add_sprite(block_sprites[int(block_index)],
                               board_to_screen(model_.get_moving_blocks()[i].get_end().into<int>()),
                               block_cover_z);
                set.add_sprite(block_text_sprites[int(text_index)],
                               board_to_screen_text(model_.get_moving_blocks()[i].get_end().into<int>(),
                                                    model_.get_moving_blocks()[i].get_val()),
                               block_cover_z + 1);
            }
            // cover newly spawned block
            set.add_sprite(block_sprites[0],
                           board_to_screen(model_.get_new_spawn_pos()),
                           block_cover_z);
        }
    }

    // vertical lines
    int vx = sqlen - 3;
    for (int i = 0; i < model_.get_size() - 1; i++) {
        set.add_sprite(line_sprite_vert,
                       Position(vx + i * sqlen, top_margin),
                       lines_z);
    }
    // horizontal lines
    int vy =  top_margin + sqlen - 3;
    for (int i = 0; i < model_.get_size() - 1; i++) {
        set.add_sprite(line_sprite_hor,
                       Position(0, vy + i * sqlen),
                       lines_z);
    }
    // borders
    set.add_sprite(border_sprite_vert, Position(0,top_margin), lines_z);
    set.add_sprite(border_sprite_hor, Position(0,top_margin), lines_z);
    set.add_sprite(border_sprite_vert,
                   Position(initial_window_dimensions().width - 3,top_margin), lines_z);
    set.add_sprite(border_sprite_hor,
                   Position(0,initial_window_dimensions().height - 3), lines_z);

    // game_over screen
    int lost_text_y = int(double(top_margin) * 1.8);
    int lost_text_x = int(double(initial_window_dimensions().width) * 0.17);
    int won_text_y = lost_text_y;
    int won_text_x = int(double(initial_window_dimensions().width) * 0.25);
    // if you lost, display game over text
    if (model_.get_game_over() == 1) {
        set.add_sprite(lost_screen, Position{0, top_margin}, game_over_z);
        set.add_sprite(lost_text, Position{lost_text_x, lost_text_y}, game_over_z + 1);
    } else if (model_.get_game_over() == 2) { // if you win, display you win text
        set.add_sprite(won_screen, Position{0, top_margin}, game_over_z);
        set.add_sprite(won_text, Position{won_text_x, won_text_y}, game_over_z + 1);
    }
}

View::Dimensions
View::initial_window_dimensions() const
{
    // x dimensions are the width of each block * number of columns
    int x = sqlen * model_.get_size();
    // y dimensions are the length of each block * number of rows + the top margin
    int y = sqlen * model_.get_size() + top_margin;
    return Dimensions(x, y);
}

std::string
View::initial_window_title() const
{
    return "2048!";
}

View::Position
View::board_to_screen(Model::Position pos) const
{
    return {sqlen * pos.x, top_margin + sqlen * pos.y};
}

View::Position
View::board_to_screen_text(Model::Position pos, int val) const
{
    Position textpos = board_to_screen(pos);
    // all blocks have same y-offset
    double y_offset = double(sqlen) * 0.18;
    textpos.y += int(y_offset);
    // x-offset depends on number of digits of val
    if (val < 16) {
        // one-digit
        double x_offset = double(sqlen) * 0.4;
        textpos.x += int(x_offset);
    } else if (val < 128) {
        // two-digit
        double x_offset = double(sqlen) * 0.3;
        textpos.x += int(x_offset);
    } else if (val < 1024) {
        // three-digit
        double x_offset = double(sqlen) * 0.2;
        textpos.x += int(x_offset);
    } else {
        // four-digit
        double x_offset = double(sqlen) * 0.108;
        textpos.x += int(x_offset);
    }
    return textpos;
}

View::Position
View::board_to_screen_a(ge211::Posn<float> pos) const
{
    float x = float(sqlen) * pos.x + float(border_line_thick);
    float y = top_margin + float(sqlen) * pos.y + float(border_line_thick);
    return ge211::Posn<float> {x, y}.into<int>();
}

View::Position
View::board_to_screen_text_a(ge211::Posn<float> pos, int val) const
{
    Position textpos = board_to_screen_a(pos);
    textpos.x -= border_line_thick;
    textpos.y -= border_line_thick;
    // all blocks have same y-offset
    double y_offset = double(sqlen) * 0.18;
    textpos.y += int(y_offset);
    // x-offset depends on number of digits of val
    if (val < 16) {
        // one-digit
        double x_offset = double(sqlen) * 0.4;
        textpos.x += int(x_offset);
    } else if (val < 128) {
        // two-digit
        double x_offset = double(sqlen) * 0.3;
        textpos.x += int(x_offset);
    } else if (val < 1024) {
        // three-digit
        double x_offset = double(sqlen) * 0.2;
        textpos.x += int(x_offset);
    } else {
        // four-digit
        double x_offset = double(sqlen) * 0.108;
        textpos.x += int(x_offset);
    }
    return textpos;
}

std::vector<View::Position>
View::get_ngb_pos() const {
    Position top_left {initial_window_dimensions().width - 110,
                       score_text_pos.y + 5};
    Position bottom_right {top_left.x + new_game_button_dims.width,
                           top_left.y + new_game_button_dims.height};
    std::vector<Position> r;
    r.push_back(top_left);
    r.push_back(bottom_right);
    return r;
}
