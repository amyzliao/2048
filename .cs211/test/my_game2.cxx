// From the docs for ge211::Abstract_game

#include <ge211.hxx>

struct My_game2 : ge211::Abstract_game
{
    // Constructs a new game with the given cursor size.
    explicit My_game2(int cursor_size);

    ///
    /// Function members
    ///

    // Holds the most recent position of the mouse:
    ge211::Posn<int> last_mouse{0, 0};

    // Saves the most recent most position each time the
    // mouse moves:
    void on_mouse_move(ge211::Posn<int> mouse) override;

    // Whenever we need to redraw the screen, add the Circle_sprite
    // at the mouse position.
    void draw(ge211::Sprite_set& sprites) override;

    ///
    /// Data members
    ///

    // The circle sprite to render where the mouse is. This is
    // initialized with a radius and a color in the My_game2
    // constructor.
    ge211::Circle_sprite cursor;
};

My_game2::My_game2(int cursor_size)
        : cursor(cursor_size, ge211::Color::medium_blue())
{ }

void My_game2::on_mouse_move(ge211::Posn<int> mouse)
{
    last_mouse = mouse;
}

void My_game2::draw(ge211::Sprite_set& sprites)
{
    ge211::Posn<int> top_left = last_mouse.up_left_by(cursor.dimensions() / 2);
    sprites.add_sprite(cursor, top_left);
}

int main()
{
    My_game2 game(50);
    game.run();
}
