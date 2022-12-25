#pragma once

#include "model.hxx"
#include "view.hxx"
#include <ge211.hxx>

class Controller : public ge211::Abstract_game
{
public:
    /// CONSTRUCTOR
    // int describes the run mode: 0 = normal, 1 = win, 2 = lose
    Controller(int);

    /// ANIMATION
    void on_frame(double dt) override;

protected:
    /// DELEGATE TO VIEW
    void draw(ge211::Sprite_set& set) override;
    View::Dimensions initial_window_dimensions() const override;
    std::string initial_window_title() const override;

    /// INTERACTIONS
    // move blocks using the arrow keys
    void on_key(ge211::Key) override;
    // restart the game by clicking new game button
    void on_mouse_down(ge211::Mouse_button, ge211::Posn<int>) override;

private:
    /// PRIVATE MEMBER VARIABLES
    Model model_;
    View view_;
};
