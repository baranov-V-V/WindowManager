#include "gui_core/widgets/Slider.h"
#include "gui_core/actions/Functors.h"
#include "event/Events.h"
#include "config/SkinsConfig.h"

SliderBasicX::SliderBasicX(int size_x, int size_y, int coord_x, int coord_y, float min_val, float max_val, VSliderAction* action) :
    VSlider(min_val, max_val, action), 
    BorderWindow(size_x, size_y, coord_x, coord_y, dgrey_c, lgrey_c, 1, Renderer::getInstance(), nullptr) {
    
    this->setRedraw(true);
    bar = new BorderWindow(size_x / 7, size_y - 2, 1, 1, lgrey_c, lgrey_c, 1, Renderer::getInstance(), this);
    min_bar = 1;
    max_bar = size_x - size_x / 7 - 1;
    this->addChild(bar);

    MoveBarX* move = new MoveBarX(this);
    StartMove* start_move_bar = new StartMove(move);
    EndMove* end_move = new EndMove(move);

    bar->setPressDown(start_move_bar);
    bar->setPressUp(end_move);
    bar->setPointed(move);
};

SliderBasicX::SliderBasicX(int size_x, int size_y, int coord_x, int coord_y, float min_val, float max_val, int thumb_width, VSliderAction* action) :
    VSlider(min_val, max_val, action), 
    BorderWindow(size_x, size_y, coord_x, coord_y, dgrey_c, lgrey_c, 1, Renderer::getInstance(), nullptr) {
    
    this->setRedraw(true);
    bar = new BorderWindow(thumb_width, size_y - 2, 1, 1, lgrey_c, lgrey_c, 1, Renderer::getInstance(), this);
    min_bar = 1;
    max_bar = size_x - thumb_width - 1;
    this->addChild(bar);

    MoveBarX* move = new MoveBarX(this);
    StartMove* start_move_bar = new StartMove(move);
    EndMove* end_move = new EndMove(move);

    bar->setPressDown(start_move_bar);
    bar->setPressUp(end_move);
    bar->setPointed(move);
};

void SliderBasicX::place(int new_coord) {
    if (new_coord < min_bar) {
        new_coord = min_bar;
    }
    if (new_coord > max_bar) {
        new_coord = max_bar;
    }
    bar->setCoordX(new_coord);
    this->setValue(this->calcValue(new_coord));
}

SliderX::SliderX(int size_x, int size_y, int coord_x, int coord_y, float min_val, float max_val, float delta, VSliderAction* action) :
    VSlider(min_val, max_val, action), delta(delta),
    InvisibleWindow(size_x, size_y, coord_x, coord_y) {
    
    this->addChild(new PicWindow(0, 0, img_arrow_left, this, new MoveSliderBarLeft(this)));
    this->addChild(new PicWindow(size_x - size_y, 0, img_arrow_right, this, new MoveSliderBarRight(this)));
    BorderWindow* bar_window = new BorderWindow(size_x - 2 * size_y, size_y, size_y, 0, dgrey_c, lgrey_c, 1, Renderer::getInstance(), nullptr, nullptr, nullptr, new PlaceSliderBarOnClickX(this));
    bar_window->setRedraw(true);
    this->addChild(bar_window);

    bar = new BorderWindow(size_x / 7, size_y - 2, 1, 1, lgrey_c, lgrey_c, 1, Renderer::getInstance(), bar_window);
    min_bar = 1;
    max_bar = (size_x - 2 * size_y) - size_x / 7 - 1;
    bar_window->addChild(bar);

    MoveBarX* move = new MoveBarX(this);
    StartMove* start_move_bar = new StartMove(move);
    EndMove* end_move = new EndMove(move);

    bar->setPressDown(start_move_bar);
    bar->setPressUp(end_move);
    bar->setPointed(move);
};

void SliderX::place(int new_coord) {
    if (new_coord < min_bar) {
        new_coord = min_bar;
    }
    if (new_coord > max_bar) {
        new_coord = max_bar;
    }
    bar->setCoordX(new_coord);
    this->setValue(this->calcValue(new_coord));
}