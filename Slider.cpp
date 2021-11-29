#include "Slider.h"
#include "Functors.h"
#include "Events.h"

BasicSliderX::BasicSliderX(int size_x, int size_y, int coord_x, int coord_y, float min_val, float max_val, VSliderAction* action) : 
    min_val(min_val), max_val(max_val), value(min_val), action(action), bar(nullptr), 
    BorderWindow(size_x, size_y, coord_x, coord_y, dgrey_c, lgrey_c, 1, Renderer::getInstance(), nullptr) {
    
    this->setRedraw(true);
    bar = new BorderWindow(size_x / 7, size_y - 2, 1, 1, lgrey_c, lgrey_c, 1, Renderer::getInstance(), this);
    min_bar_x = 1;
    max_bar_x = size_x - size_x / 7 - 1;
    this->addChild(bar);

    MoveBarX* move = new MoveBarX(this);
    StartMove* start_move_bar = new StartMove(move);
    EndMove* end_move = new EndMove(move);

    bar->setPressDown(start_move_bar);
    bar->setPressUp(end_move);
    bar->setPointed(move);
};

BasicSliderX::BasicSliderX(int size_x, int size_y, int coord_x, int coord_y, float min_val, float max_val, int thumb_width, VSliderAction* action) :
    min_val(min_val), max_val(max_val), value(min_val), action(action), bar(nullptr), 
    BorderWindow(size_x, size_y, coord_x, coord_y, dgrey_c, lgrey_c, 1, Renderer::getInstance(), nullptr) {
    
    this->setRedraw(true);
    bar = new BorderWindow(thumb_width, size_y - 2, 1, 1, lgrey_c, lgrey_c, 1, Renderer::getInstance(), this);
    min_bar_x = 1;
    max_bar_x = size_x - thumb_width - 1;
    this->addChild(bar);

    MoveBarX* move = new MoveBarX(this);
    StartMove* start_move_bar = new StartMove(move);
    EndMove* end_move = new EndMove(move);

    bar->setPressDown(start_move_bar);
    bar->setPressUp(end_move);
    bar->setPointed(move);
};

void BasicSliderX::place(int new_coord) {
    if (new_coord < min_bar_x) {
        new_coord = min_bar_x;
    }
    if (new_coord > max_bar_x) {
        new_coord = max_bar_x;
    }
    bar->setCoordX(new_coord);
    this->setValue(this->calcValue(new_coord));
}