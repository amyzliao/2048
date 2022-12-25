#include "controller.hxx"
#include <iostream>

Controller::Controller(int run_mode)
        : model_(run_mode),
          view_(model_)
{ }

void Controller::on_frame(double dt) {
    model_.on_frame(dt);
}

void
Controller::draw(ge211::Sprite_set& set)
{
    view_.draw(set);
}

View::Dimensions
Controller::initial_window_dimensions() const
{
    return view_.initial_window_dimensions();
}

std::string
Controller::initial_window_title() const
{
    return view_.initial_window_title();
}

void
Controller::on_key(ge211::Key key)
{
    // if the game is NOT over, play moves
    if (model_.get_game_over() == 0) {
        if (key == ge211::Key::left()) {
            model_.play_move({-1, 0});
        }
        else if (key == ge211::Key::right()) {
            model_.play_move({1, 0});
        }
        else if (key == ge211::Key::up()) {
            model_.play_move({0, -1});
        }
        else if (key == ge211::Key::down()) {
            model_.play_move({0, 1});
        }
    }
    // if the game is over, do nothing
}

void
Controller::on_mouse_down(ge211::Mouse_button, ge211::Posn<int> pos)
{
    if (pos.x > view_.get_ngb_pos()[0].x && pos.x < view_.get_ngb_pos()[1].x) {
        if (pos.y > view_.get_ngb_pos()[0].y && pos.y < view_.get_ngb_pos()[1].y) {
            model_.new_game();
        }
    }
}