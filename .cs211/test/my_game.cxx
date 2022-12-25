// From the docs for ge211::Abstract_game

#include <ge211.hxx>

const ge211::Dims<int> dimensions {300, 200};
const ge211::Color     color {200, 0, 0};     // red

struct My_game : ge211::Abstract_game
{
    void draw(ge211::Sprite_set& sprites) override;

    ge211::Rectangle_sprite rect{dimensions, color};
};

void My_game::draw(ge211::Sprite_set& sprites)
{
    sprites.add_sprite(rect, ge211::Posn<int>(100, 100));
}

int main()
{
    My_game().run();
}
