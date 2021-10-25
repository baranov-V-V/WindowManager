#include "WindowManager.h"

Renderer::Renderer(BasicWindow* window, double min_x, double min_y, double max_x, double max_y) :
    window(window), max(max_x, max_y), min(min_x, min_y) {
    this->calcScale();    
};

void Renderer::calcScale() {
    scale.x = double(window->getSizeX()) / double(max.x - min.x);
    scale.y = double(window->getSizeY()) / double(max.y - min.y); 
};

void Renderer::setWindow(BasicWindow* new_window) {
    window = new_window;
    this->setMaxX(window->getSizeX());    
    this->setMinX(0);
    this->setMaxY(window->getSizeY());    
    this->setMinY(0);
    this->calcScale();
};

int Renderer::toPixelX(double coord) const {
    return (coord - min.x) * scale.x;
};

int Renderer::toPixelY(double coord) const {
    return (coord - min.y) * scale.y;
};

double Renderer::toCoordX(int x) const {
    return double(x) / scale.x + min.x;
};

double Renderer::toCoordY(int y) const {
    return double(y) / scale.y + min.y;
};

ManagerWindow::ManagerWindow(int x_size, int y_size, int coord_x, int coord_y, COLORREF color, ManagerWindow* parent,
                             VFunctor* press_up_f, VFunctor* pointed_f, VFunctor* press_down_f) :
    Texture(x_size, y_size, color, coord_x, coord_y), parent(parent), base_img(x_size, y_size, color, 0, 0) {

    if (press_up_f == nullptr) {
        ManagerWindow::press_up_f = new DebugFunctorTrue(this);
    } else {
        ManagerWindow::press_up_f = press_up_f;
    }
    
    if (press_down_f == nullptr) {
        ManagerWindow::press_down_f = new DebugFunctorTrue(this);
    } else {
        ManagerWindow::press_down_f = press_down_f;
    }

    if (press_up_f == nullptr) {
        ManagerWindow::pointed_f = new DebugFunctorTrue(this);
    } else {
        ManagerWindow::pointed_f = pointed_f;
    }

    is_clicked = false;
    is_pointed = false;
    max_size = 100;
    count = 0;
    ManagerWindow::children = new ManagerWindow*[max_size];
}

ManagerWindow::~ManagerWindow() {
    for (int i = 0; i < count; ++i) {
        this->delLast();
    }
    delete[] children;
    delete pointed_f;
    delete press_down_f;
    delete press_up_f;
};

void ManagerWindow::drawChilds(Renderer* renderer) const {
    //cerr << "trying draw childs for: [" << this << "]\n";
    for (int i = 0; i < count; ++i) {
        children[i]->draw(renderer);
        children[i]->showOn(this);
    }
};

void ManagerWindow::addChild(ManagerWindow* window) {
    if (count < max_size) {
        children[count++] = window;
    } else {
        std::cerr << "Overflow!!" << std::endl;
    }
};

void ManagerWindow::delChild(ManagerWindow* window) {
    int pos = 0;
    for (int i = 0; i < count; ++i) {
        if (children[i] == window) {
            for (int j = i; j < count - 1; ++j) {
                children[j] = children[j + 1];   
            }
            count--;
            delete window;
            break;
        }
    }
};

void ManagerWindow::makeFirst(ManagerWindow* window) {
    for (int i = 0; i < count; ++i) {
        if (children[i] == window) {
            for (int j = i; j < count - 1; ++j) {
                children[j] = children[j + 1];   
            }
            children[count - 1] = window;
            break;
        }
    }
};

void ManagerWindow::delLast() {
    children[--count] = nullptr;
};

bool ManagerWindow::checkLeftClick(WindowMouse* mouse) {
    return this->checkPointed(mouse) && (mouse->getState() & LEFT_CLICK != 0);
};

bool ManagerWindow::checkPointed(WindowMouse* mouse) {
    int x = mouse->getRelCoord().x;
    int y = mouse->getRelCoord().y;
    return (x > 0 && x < this->getSizeX()) && (y > 0 && y < this->getSizeY());
}

bool ManagerWindow::proceedPressDown(WindowMouse* mouse) {
    mouse->setWindow(this);

    if (this->checkPointed(mouse)) {
        for (int i = count - 1; i >= 0; --i) {
            if (children[i]->proceedPressDown(mouse)) {
                return true;
            }
        }
        is_clicked = true;
        //cout << "action down!" << "\n";
        bool result = press_down_f->action();
        mouse->setToParent();
        return result;
    }
    mouse->setToParent();
    return false;
};

bool ManagerWindow::proceedPressUp(WindowMouse* mouse) {
    mouse->setWindow(this);

    if (this->checkPointed(mouse)) {
        for (int i = count - 1; i >= 0; --i) {
            if (children[i]->proceedPressUp(mouse)) {
                return true;
            }
        }
        bool result = false;
        if (is_clicked) {
            //cout << "action up!" << "\n";
            result = press_up_f->action();
            is_clicked = false;
        }
        mouse->setToParent();
        return result;
    }
    is_clicked = false;
    mouse->setToParent();
    return false;
};

bool ManagerWindow::proceedPointed(WindowMouse* mouse) {
    this->markPointedWindows(mouse);
    this->proceedPointedWindows(mouse);

    return true;
};

void ManagerWindow::markPointedWindows(WindowMouse* mouse) {
    mouse->setWindow(this);
    
    for (int i = count - 1; i >= 0; --i) {
        children[i]->markPointedWindows(mouse);
    }

    if (this->checkPointed(mouse)) {
        is_pointed = true;
    } else {
        if (is_pointed) {
            is_pointed = false;
            if (pointed_f != nullptr) {
                pointed_f->action();
            }
        }
        is_pointed = false;
    }
    
    mouse->setToParent();
};
    
bool ManagerWindow::proceedPointedWindows(WindowMouse* mouse) {
    mouse->setWindow(this);

    for (int i = count - 1; i >= 0; --i) {
        if(children[i]->proceedPointedWindows(mouse)) {
            mouse->setToParent();
            return true;
        };
    }
    
    if (is_pointed) {
        if (pointed_f != nullptr) {
            if(pointed_f->action()) {
                mouse->setToParent();
                return true;
            };
        }
    }

    mouse->setToParent();
    return false;
};

BorderWindow::BorderWindow(int x_size, int y_size, int coord_x, int coord_y, COLORREF color, COLORREF border_color, int thickness, Renderer* render,
                 ManagerWindow* parent, VFunctor* press_up_f, VFunctor* pointed_f, VFunctor* press_down_f) : 
    ManagerWindow(x_size, y_size, coord_x, coord_y, color, parent, press_up_f, pointed_f, press_down_f), border_color(border_color), thickness(thickness) {
    render->setWindow(this);
    render->drawRectangle(0, 0, x_size, y_size, border_color, thickness);
    //cerr << "constructed: [" << this << "]\n";
};

    
void BorderWindow::draw(Renderer* render) const {
    //cerr << "started drawing: [" << this << "]\n";
    render->setWindow(const_cast<BorderWindow*>(this));
    //render->drawRectangle(0, 0, size.x, size.y, border_color, thickness); CHANGE!!!
    this->drawChilds(render);
    //cerr << "ended drawing: [" << this << "]\n";
};

ClockWindow::ClockWindow(int x_size, int y_size, int coord_x, int coord_y, COLORREF color, ManagerWindow* parent,
                         VFunctor* press_up_f, VFunctor* pointed_f, VFunctor* press_down_f) : 
    ManagerWindow(x_size, y_size, coord_x, coord_y, color, parent, press_up_f, pointed_f, press_down_f) {
    this->updateTime();
    //cerr << "constructed: [" << this << "]\n";
};

void ClockWindow::draw(Renderer* render) const {
    ClockWindow* nc_this = const_cast<ClockWindow*>(this);
    
    char buf[24];
    std::strftime(buf, 1024, "%H:%M:%S", &curr_time);
    
    render->setWindow(nc_this);
    render->drawRectangle(0, 0, size.x, size.y, color, 1);
    nc_this->updateTime();
    
    render->drawText(size.x / 20, size.y / 6, buf, "Helvetica", 2 * size.y / 3, size.y / 4, silver_c);
    this->drawChilds(render);
};

int WindowMouse::getState() const {
    return state;
};

void WindowMouse::printState() const{
    cout << "    Mouse\n" << "abs_coord_x: " << abs_coord.x << " abs_coord_y: " << abs_coord.y << "\n";
    cout << "rel_coord_x: " << rel_coord.x << " rel_coord_y: " << rel_coord.y << "\n";
    cout << "state: " << state << "\n";
    //cout << "window: " << window << "\n";
};

void WindowMouse::setWindow(ManagerWindow* new_window) {
    if (new_window->getParent() != window) {
        assert("In WindowMouse::setWindow parent of new window is not current window");
    }
    rel_coord.x = rel_coord.x - new_window->getCoordX();
    rel_coord.y = rel_coord.y - new_window->getCoordY();
    window = new_window;
}; 
        // only if new window is child of current window
void WindowMouse::setToParent() {
    if (window->getParent() != nullptr) {
        rel_coord.x = rel_coord.x + window->getCoordX();
        rel_coord.y = rel_coord.y + window->getCoordY();
        window = window->getParent();
    }
};

PicWindow::PicWindow(int x_size, int y_size, int coord_x, int coord_y, const char* pic_name, ManagerWindow* parent,
                     VFunctor* press_up_f, VFunctor* pointed_f, VFunctor* press_down_f, bool need_redraw) :
    ManagerWindow(x_size, y_size, coord_x, coord_y, 0, parent, press_up_f, pointed_f, press_down_f) {
    Texture pic(x_size, y_size, pic_name, 0, 0);
    pic.showOn(this);
    pic.showOn(&base_img);
};

PicWindow::PicWindow(int x_size, int y_size, int coord_x, int coord_y, char* pic_name, ManagerWindow* parent,
                     VFunctor* press_up_f, VFunctor* pointed_f, VFunctor* press_down_f, bool need_redraw) :
    ManagerWindow(x_size, y_size, coord_x, coord_y, 0, parent, press_up_f, pointed_f, press_down_f), need_redraw(need_redraw) {
    Texture pic(x_size, y_size, pic_name, 0, 0);
    pic.showOn(this);
    pic.showOn(&base_img);
};
    
void PicWindow::draw(Renderer* render) const {
    render->setWindow(const_cast<PicWindow*>(this));
    if (need_redraw) {
        base_img.showOn(this);
    }
    this->drawChilds(render);
};

ThicknessWindow::ThicknessWindow(int x_size, int y_size, int coord_x, int coord_y, COLORREF color, COLORREF border_color, int thickness, Feather* feather, Renderer* render,
                                 ManagerWindow* parent, VFunctor* press_up_f, VFunctor* pointed_f, VFunctor* press_down_f) : 
    BorderWindow(x_size, y_size, coord_x, coord_y, color, border_color, thickness, render, parent, press_up_f, pointed_f, press_down_f), feather(feather) {
    //cerr << "constructed: [" << this << "]\n";
};

void ThicknessWindow::draw(Renderer* render) const {
    render->setWindow(const_cast<ThicknessWindow*>(this));
    render->drawRectangle(0, 0, size.x, size.y, border_color, thickness);
    render->drawLine(6, size.y / 2, size.x - 6, size.y / 2, feather->getColor(), feather->getThickness());
    this->drawChilds(render);
};

CanvasWindow::CanvasWindow(int x_size, int y_size, int coord_x, int coord_y, char* name, ManagerWindow* parent,
                           Renderer* render, Feather* feather, WindowMouse* mouse) :
    PicWindow(x_size, y_size, coord_x, coord_y, img_canvas, parent), name(name), on_display(true) {
    
    DrawFunctor* canvas_functor = new DrawFunctor(this, render, feather, mouse);
    MakeFirst* make_first = new MakeFirst(this);
    this->setPressDown(make_first);
    this->setPointed(canvas_functor);

    int menu_x = x_size;
    int menu_y = 5 + y_size / 15;

    PicWindow* menu = MakeBasicMenu(menu_x, menu_y, 0, 0, this, 10);

    render->setWindow(menu);
    render->drawText(menu_x /2 - strlen(name) * menu_y / 4, menu_y / 6, name, "Helvetica", 2 * menu_y / 3, menu_y / 4, silver_c);

    MakeMovable(menu, this, mouse);

    this->addChild(menu);
};