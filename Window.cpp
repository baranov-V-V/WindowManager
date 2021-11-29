#include <iostream>
#include <cassert>
#include <cmath>

#include "Window.h"
#include "Functors.h"
#include "Tools.h"
#include "App.h"

Renderer::Renderer() : window(nullptr), max(0, 0), min(0, 0) {};

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

DisplayManager::DisplayManager(Renderer* render) : canvas_menu(nullptr), render(render) { assert(render); };
DisplayManager::~DisplayManager() {};

void DisplayManager::updateCanvasMenu() {
    bool state = canvas_menu->getShow();
    InvisibleWindow* new_menu = MakeCanvasMenu(canvas_menu->getCoordX(), canvas_menu->getCoordY(), this, render, canvas_menu->getParent());
    ManagerWindow* parent = canvas_menu->getParent();
    parent->delChild(canvas_menu);
    parent->addChild(new_menu);
    canvas_menu = new_menu;
    canvas_menu->setShow(state);
};

void DisplayManager::addWindow(CanvasWindow* window) {
    windows.insert(window);
};

void DisplayManager::showWindow(CanvasWindow* window) {
    auto it = windows.find(window);
    if (it != windows.end()) {
        (*it)->setShow(true);
    }
};

void DisplayManager::hideWindow(CanvasWindow* window) {
    auto it = windows.find(window);
    if (it != windows.end()) {
        (*it)->setShow(false);
    }
};

void DisplayManager::delWindow(CanvasWindow* window) {
    if (windows.find(window) != windows.end()) {
        windows.erase(window);
    }
};

void DisplayManager::hideAll() {
    for (auto it = windows.begin(); it != windows.end(); ++it) {
        (*it)->setShow(false);
    }
};

void DisplayManager::showAll() {
    for (auto it = windows.begin(); it != windows.end(); ++it) {
        (*it)->setShow(true);
    }
};

void DisplayManager::invertShowMenu() {
    if (!canvas_menu->getShow()) {
        canvas_menu->getParent()->makeFirst(canvas_menu);
    }
    canvas_menu->setShow(!canvas_menu->getShow());
    //std::cout << "iverted\n";
};


ManagerWindow::ManagerWindow(int x_size, int y_size, int coord_x, int coord_y, COLORREF color, ManagerWindow* parent,
                             VFunctor* press_up_f, VFunctor* pointed_f, VFunctor* press_down_f) :
    Texture(x_size, y_size, color, coord_x, coord_y), parent(parent) {

    if (press_up_f == nullptr) {
        functors[EVENT_MOUSE_RELEASED_LC] = new DebugFunctorTrue(this);
    } else {
        functors[EVENT_MOUSE_RELEASED_LC] = press_up_f;
    }
    
    if (press_down_f == nullptr) {
        functors[EVENT_MOUSE_PRESSED_LC] = new DebugFunctorTrue(this);
    } else {
        functors[EVENT_MOUSE_PRESSED_LC] = press_down_f;
    }

    if (pointed_f == nullptr) {
        functors[EVENT_MOUSE_MOVE] = new DebugFunctorTrue(this);
    } else {
        functors[EVENT_MOUSE_MOVE] = pointed_f;
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
    
    for (auto it : functors) {
        delete it.second;
    }
};

void ManagerWindow::setPointed(VFunctor* functor) {
    //delete ManagerWindow::pointed_f;
    auto it = functors.find(EVENT_MOUSE_MOVE);
    if (it != functors.end()) {
        delete it->second;
    }
    functors[EVENT_MOUSE_MOVE] = functor;
    //ManagerWindow::pointed_f = functor;
};
void ManagerWindow::setPressUp(VFunctor* functor) {
    //delete ManagerWindow::press_up_f;
    auto it = functors.find(EVENT_MOUSE_RELEASED_LC);
    if (it != functors.end()) {
        delete it->second;
    }
    functors[EVENT_MOUSE_RELEASED_LC] = functor;
    //ManagerWindow::press_up_f = functor;
};
void ManagerWindow::setPressDown(VFunctor* functor) {
    //delete ManagerWindow::press_down_f;
    auto it = functors.find(EVENT_MOUSE_PRESSED_LC);
    if (it != functors.end()) {
        delete it->second;
    }
    functors[EVENT_MOUSE_PRESSED_LC] = functor;
    //ManagerWindow::press_down_f = functor;
};
void ManagerWindow::setFunctor(EventType type, VFunctor* functor) {
   auto it = functors.find(type);
    if (it != functors.end()) {
        delete it->second;
    }
    functors[type] = functor; 
}

VFunctor* ManagerWindow::getPointed() const {
    auto it = functors.find(EVENT_MOUSE_MOVE);
    if (it != functors.end()) {
        return it->second;
    }
    return nullptr;
    //return pointed_f;
};
VFunctor* ManagerWindow::getPressUp() const { 
    auto it = functors.find(EVENT_MOUSE_RELEASED_LC);
    if (it != functors.end()) {
        return it->second;
    }
    return nullptr;
    //return press_up_f;
};
VFunctor* ManagerWindow::getPressDown() const {
    auto it = functors.find(EVENT_MOUSE_PRESSED_LC);
    if (it != functors.end()) {
        return it->second;
    }
    return nullptr;
    //return press_down_f;
};
VFunctor* ManagerWindow::getFunctor(EventType type) const {
    /*
    if (type == EVENT_MOUSE_PRESSED_RC) {
        std::cout << "Have functors for events:\n";
        for (auto it = functors.begin(); it != functors.end(); ++it) {
            std::cout << it->first << " ";
        }
        std::cout << "\n";
    }
    */
   
    auto it = functors.find(type);
    if (it != functors.end()) {
        return it->second;
    }
    return nullptr;
}

void ManagerWindow::drawChilds(Renderer* renderer) const {
    //cerr << "trying draw childs for: [" << this << "]\n";
    for (int i = 0; i < count; ++i) {
        children[i]->draw(renderer);
    }
};

void ManagerWindow::addChild(ManagerWindow* window) {
    if (count < max_size) {
        children[count++] = window;
        window->setParent(this);
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

void ManagerWindow::showOnTexture(const Texture* target) const {
    RGBQUAD* target_buf = target->getBuf();
    RGBQUAD* source_buf = this->getBuf();
    RGBQUAD pixel = {};

    int x1 = 0;
    int x2 = this->getSizeX();
    int y1 = 0;
    int y2 = this->getSizeY();
    
    for(int ix = x1; ix < x2; ++ix) {
        for(int iy = y1; iy < y2; ++iy) {
            if (this->hitTest(ix, iy)) {
                pixel = source_buf[ix + (-iy + this->getSizeY()) * this->getSizeX()];
                int target_pos = ix + coord.x + (-iy - coord.y + target->getSizeY()) * target->getSizeX();
                if ((target_pos <= target->getSizeX() * target->getSizeY()) && target_pos >= 0) {
                    //cout << "old_x: " << ix << " old_y: " << iy << "\n";
                    //cout << "new_x: " << ix + << "new_y: " << iy << "\n";
                    target_buf[target_pos] = pixel;
                }
            }
        }
    }
};

bool ManagerWindow::checkLeftClick(WindowMouse* mouse) {
    return this->checkPointed(mouse) && (mouse->getState() & LEFT_CLICK != 0);
};

bool ManagerWindow::hitTest(double x, double y) const {
    return (x > 0 && x < this->getSizeX()) && (y > 0 && y < this->getSizeY());
};

Pair<int> ManagerWindow::toRelCoord(const Pair<int>& abs_coord) {
    Pair<int> rel_coord = abs_coord;
    ManagerWindow* parent = this;

    while(parent != nullptr) {
        rel_coord = rel_coord - parent->getCoord();
        parent = parent->getParent();
    }

    //std::cout << "rel_coord_x: " << rel_coord.x << " rel_coord_y: " << rel_coord.y << "\n";

    return rel_coord;
};

bool ManagerWindow::checkPointed(const Pair<int>& abs_coord) {
    Pair<int> rel_coord = this->toRelCoord(abs_coord);
    return hitTest(rel_coord.x, rel_coord.y);
}

bool ManagerWindow::checkPointed(WindowMouse* mouse) {
    int x = mouse->getRelCoord().x;
    int y = mouse->getRelCoord().y;
    return hitTest(x, y);
}

EventState ManagerWindow::processEvent(const Event& event) {
    if (event.getType() == EVENT_MOUSE_PRESSED_LC && this->checkPointed(event.getData().abs_coord)) {
        //std::cout << "in window [" << this << "]\n";
        //std::cout << "child count: " << count << "\n";
    }
    
    if (!is_shown) {
        //std::cout << "not shown\n";
        return EVENT_NEW;
    }

    if (event.getType() == EVENT_MOUSE_PRESSED_LC && this->checkPointed(event.getData().abs_coord)) {
        is_clicked = true;
    }

    if (event.getType() == EVENT_MOUSE_MOVE) {
        if (this->checkPointed(event.getData().mouse_data.start)) {
            is_pointed = true;
        } else {
            if (is_pointed == true) {
                is_pointed = false;
                this->getPointed()->action(event.getData());
            }
            is_pointed = false;
        }
    }

    EventState result = EVENT_NEW;

    for (int i = count - 1; i >= 0; --i) {
        if (event.getType() == EVENT_MOUSE_PRESSED_LC && this->checkPointed(event.getData().abs_coord)) {
            //std::cout << "giving from [" << this << "] to child [" << children[i] << "]\n";
        }
        result = children[i]->processEvent(event);

        if (result == EVENT_DONE) {
            if (event.getType() == EVENT_MOUSE_RELEASED_LC) {
                is_clicked = false;
            }
            //std::cout << "Event done!\n";
            return EVENT_DONE;
        }
        if (result == EVENT_IN_PROCESS) {
            //std::cout << "Event in progress!\n";
            break;
        }
    }

    switch(event.getType()) {
        case EVENT_MOUSE_MOVE:
            
            if (is_pointed) {
                if (this->getPointed() != nullptr) {
                    if (this->getPointed()->action(event.getData())) {
                        result = EVENT_DONE;
                    } else {
                        result = EVENT_IN_PROCESS; 
                    }
                }
            } /*else if (is_pointed) {
                is_pointed = false;
                if (this->getPointed() != nullptr) {
                    if (this->getPointed()->action(event.getData())) {
                        result = EVENT_DONE;
                    } else {
                        result = EVENT_IN_PROCESS; 
                    }
                }
                is_pointed = false;
            } */

            break;

        case EVENT_MOUSE_PRESSED_LC:

            if (this->checkPointed(event.getData().abs_coord)) {
                if (this->getPressDown() != nullptr) {
                    if (this->getPressDown()->action(event.getData())) {
                        result = EVENT_DONE;
                    } else {
                        result = EVENT_IN_PROCESS; 
                    }
                }
            }

            break;

        case EVENT_MOUSE_RELEASED_LC:

            if (this->checkPointed(event.getData().abs_coord)) {
                if (is_clicked && this->getPressUp() != nullptr) {
                    if (this->getPressUp()->action(event.getData())) {
                        result = EVENT_DONE;
                    } else {
                        result = EVENT_IN_PROCESS; 
                    };
                    //std::cout << "in release if\n";
                }
                //std::cout << "released result: " << result << "\n";
            }
            is_clicked = false;

            break;
        
        case EVENT_MOUSE_PRESSED_RC:

            if (this->checkPointed(event.getData().abs_coord)) {
                if (this->getFunctor(EVENT_MOUSE_PRESSED_RC) != nullptr) {
                    if (this->getFunctor(EVENT_MOUSE_PRESSED_RC)->action(event.getData())) {
                        result = EVENT_DONE;
                    } else {
                        result = EVENT_IN_PROCESS; 
                    };
                    //std::cout << "in release if\n";
                } else {
                    //std::cout << "didn't find functor for rc\n";
                }
                //std::cout << "released result: " << result << "\n";
            }

            break;

        default:
            std::cout << "no such event in processEvent: " << event.getType() << "\n";
    };   

    return result;
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
    if (!is_shown) {
        return;
    }

    render->setWindow(const_cast<BorderWindow*>(this));
    if (need_redraw) {
        render->drawRectangle(0, 0, size.x, size.y, border_color, thickness);
    }
    this->drawChilds(render);
    if (this->getParent() != nullptr) {
        this->showOn(this->getParent());
    }
    //cerr << "ended drawing: [" << this << "]\n";
};

ClockWindow::ClockWindow(int x_size, int y_size, int coord_x, int coord_y, COLORREF color, ManagerWindow* parent,
                         VFunctor* press_up_f, VFunctor* pointed_f, VFunctor* press_down_f) : 
    ManagerWindow(x_size, y_size, coord_x, coord_y, color, parent, press_up_f, pointed_f, press_down_f) {
    this->updateTime();
    //cerr << "constructed: [" << this << "]\n";
};

void ClockWindow::draw(Renderer* render) const {
    if (!is_shown) {
        return;
    }
    
    ClockWindow* nc_this = const_cast<ClockWindow*>(this);
    
    char buf[24];
    std::strftime(buf, 1024, "%H:%M:%S", &curr_time);
    
    render->setWindow(nc_this);
    render->drawRectangle(0, 0, size.x, size.y, color, 1);
    nc_this->updateTime();
    
    render->drawText(size.x / 20, size.y / 6, buf, "Helvetica", 2 * size.y / 3, size.y / 4, silver_c);
    this->drawChilds(render);
    if (this->getParent() != nullptr) {
        this->showOn(this->getParent());
    }
};

int WindowMouse::getState() const {
    return state;
};

void WindowMouse::printState() const{
    std::cout << "    Mouse\n" << "abs_coord_x: " << abs_coord.x << " abs_coord_y: " << abs_coord.y << "\n";
    std::cout << "rel_coord_x: " << rel_coord.x << " rel_coord_y: " << rel_coord.y << "\n";
    std::cout << "state: " << state << "\n";
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
    ManagerWindow(x_size, y_size, coord_x, coord_y, 0, parent, press_up_f, pointed_f, press_down_f), base_img(x_size, y_size, pic_name, 0, 0) {
    PicWindow::need_redraw = need_redraw;
    Texture pic(x_size, y_size, pic_name, 0, 0);
    pic.showOn(this);
};

PicWindow::PicWindow(int x_size, int y_size, int coord_x, int coord_y, char* pic_name, ManagerWindow* parent,
                     VFunctor* press_up_f, VFunctor* pointed_f, VFunctor* press_down_f, bool need_redraw) :
    ManagerWindow(x_size, y_size, coord_x, coord_y, 0, parent, press_up_f, pointed_f, press_down_f), base_img(x_size, y_size, pic_name, 0, 0) {
    PicWindow::need_redraw = need_redraw;
    Texture pic(x_size, y_size, pic_name, 0, 0);
    pic.showOn(this);
};

PicWindow::PicWindow(int coord_x, int coord_y, char* pic_name, ManagerWindow* parent,
                     VFunctor* press_up_f, VFunctor* pointed_f, VFunctor* press_down_f, bool need_redraw) : base_img(pic_name, 0, 0),
    ManagerWindow(base_img.getSizeX(), base_img.getSizeY(), coord_x, coord_y, 0, parent, press_up_f, pointed_f, press_down_f) {
    PicWindow::need_redraw = need_redraw;
    base_img.showOn(this);
};
    
void PicWindow::draw(Renderer* render) const {
    assert(render);
    
    if (!is_shown) {
        return;
    }
    
    render->setWindow(const_cast<PicWindow*>(this));
    
    if (need_redraw) {
        base_img.showOn(this);
    }
    
    this->drawChilds(render);

    if (this->getParent()) {
        this->showOn(this->getParent());
    }
};

ThicknessWindow::ThicknessWindow(int x_size, int y_size, int coord_x, int coord_y, COLORREF color, COLORREF border_color, int thickness, ToolManager* tools, Renderer* render,
                                 ManagerWindow* parent, VFunctor* press_up_f, VFunctor* pointed_f, VFunctor* press_down_f) : 
    BorderWindow(x_size, y_size, coord_x, coord_y, color, border_color, thickness, render, parent, press_up_f, pointed_f, press_down_f), tools(tools) {
    //cerr << "constructed: [" << this << "]\n";
};

void ThicknessWindow::draw(Renderer* render) const {
    if (!is_shown) {
        return;
    }
    
    render->setWindow(const_cast<ThicknessWindow*>(this));
    render->drawRectangle(0, 0, size.x, size.y, border_color, thickness);
    ToolFeather* feather = reinterpret_cast<ToolFeather*>(tools->operator[](TOOL_FEATHER));
    render->drawLine(6, size.y / 2, size.x - 6, size.y / 2, feather->getColor(), feather->getThickness());
    this->drawChilds(render);
    if (this->getParent()) {
        this->showOn(this->getParent());
    }
};

InvisibleWindow::InvisibleWindow(int x_size, int y_size, int coord_x, int coord_y, ManagerWindow* parent,
                                 VFunctor* press_up_f, VFunctor* pointed_f, VFunctor* press_down_f, bool need_redraw) :
    ManagerWindow(x_size, y_size, coord_x, coord_y, 0, parent, press_up_f, pointed_f, press_down_f) {
};
    
void InvisibleWindow::draw(Renderer* render) const {
    if (!is_shown) {
        return;
    }
    //render->setWindow(this->getParent());
    //render->drawFilledRectangle(coord.x, coord.y, coord.x + size.x, coord.y + size.y, black_c, black_c, 1);

    for (int i = 0; i < this->getCount(); ++i) {
        this->getChild(i)->draw(render);
        render->setWindow(this->getParent());
        if (this->getParent()) {
            //cout << "size_x: " << this->getChild(i)->getSizeX() << " size_y: " << this->getChild(i)->getSizeY() << "\n";
            this->getChild(i)->showOn(this->getParent(), this->getChild(i)->getCoordX() + coord.x, this->getChild(i)->getCoordY() + coord.y);
        } else {
            std::cout << "cannot draw invsible window";
        }
    }
    
};

TextButtonWindow::TextButtonWindow(int x_size, int y_size, int coord_x, int coord_y, COLORREF color, COLORREF border_color, int thickness,
                                   COLORREF text_color, const char* text, const char* font_name, int ch_size_x, int ch_size_y, int align, Renderer* render,
                                   ManagerWindow* parent, VFunctor* press_up_f, VFunctor* pointed_f, VFunctor* press_down_f) : 
    BorderWindow(x_size, y_size, coord_x, coord_y, color, border_color, thickness, render, parent, press_up_f, pointed_f, press_down_f), 
    text(text), font_name(font_name), ch_size_x(ch_size_x), ch_size_y(ch_size_y), align(align), text_color(text_color) {
    
    assert(text);
    assert(font_name);

    need_redraw = true;
    this->draw(render);
    need_redraw = false;
};

void TextButtonWindow::draw(Renderer* render) const {
    if (!is_shown) {
        return;
    }

    render->setWindow(const_cast<TextButtonWindow*>(this));
    if (need_redraw) {
        render->drawFilledRectangle(0, 0, size.x, size.y, color, border_color, thickness);
        
        int text_size = strlen(text) * ch_size_x;
        int x_ident = ch_size_x;
        int y_ident = (this->getSizeY() - ch_size_y) / 2;
        switch (align) {
            case ALIGN_LEFT:
                render->drawText(x_ident, y_ident, text, font_name, ch_size_y, ch_size_x, text_color);       
                break;

            case ALIGN_MID:
                render->drawText(this->getSizeX() / 2 - text_size / 2, y_ident, text, font_name, ch_size_y, ch_size_x, text_color);       
                break;
                
            case ALIGN_RIGHT:
                render->drawText(this->getSizeX() - text_size - x_ident, y_ident, text, font_name, ch_size_y, ch_size_x, text_color);       
                break;

            default:
                fprintf(stderr, "Not correct align [%d] in button with label <%s>\n", align, text);
                break;
        }
        const_cast<TextButtonWindow*>(this)->setRedraw(false);
    }
    if (this->getParent() != nullptr) {
        this->showOn(this->getParent());
    }
};

CanvasWindow::CanvasWindow(int x_size, int y_size, int coord_x, int coord_y, char* name, ManagerWindow* parent,
                           Renderer* render, App* app, const char* pic_name) :
    InvisibleWindow(x_size + 2 * grab_len, y_size + 2 * grab_len, coord_x - grab_len, coord_y - grab_len, parent), name(name),
    base_img(x_size, y_size, white_c, 0, 0) {
    need_redraw = false;

    int menu_x = x_size;
    int menu_y = 25;

    int but_x = close_button_x;
    PicWindow* menu = MakeBasicMenu(menu_x, menu_y, grab_len, grab_len, this, app, but_x);

    BorderWindow* canvas = new BorderWindow(max_canvas_x, max_canvas_y, grab_len, menu_y + grab_len, white_c, white_c, 1, render, this);
    canvas->setSize({x_size, y_size - menu_y});
    canvas->setRedraw(false);
    
    DrawFunctor* draw_f = new DrawFunctor(canvas, render, app, app->getToolManager());
    StartDraw* start_draw = new StartDraw(draw_f);
    EndDraw* end_draw = new EndDraw(draw_f);
    
    canvas->setPressDown(start_draw);
    canvas->setPressUp(end_draw);
    canvas->setPointed(draw_f);

    render->setWindow(menu);
    int text_size = strlen(name);
    int text_y = 4 * menu_y / 5;
    int text_x = text_y / 3;
    /*
    if (text_size < menu_x - 2 * but_x) {
        int ident_y = (menu_y - text_y) / 2;
        render->drawText(menu_x / 2 - 30 * 2 - text_size / 2, ident_y, name, "Helvetica", text_y, text_x, silver_c);
    }
    */

    TextButtonWindow* button_reset  = new TextButtonWindow(7 * (text_x + 1), menu_y, 0, 0, mgrey_c, mgrey_c, 1, silver_c, "Clear", "Helvetica", text_x + 1, text_y - 2, ALIGN_LEFT, render, menu);
    GlowBorderFunctor* glow_button_reset = new GlowBorderFunctor(button_reset, lgrey_c, lgrey_c);
    ClearCanvas* clear_f = new ClearCanvas(canvas, render);
    button_reset->setPointed(glow_button_reset);
    button_reset->setPressUp(clear_f);
    menu->addChild(button_reset);

    render->setWindow(menu);
    if (menu->getSizeX() / 2 - text_x * text_size > button_reset->getSizeX()) {
        int ident_y = (menu->getSizeY() - text_y) / 2;
        render->drawText(menu->getSizeX() / 2 - text_x * text_size, ident_y, name, "Helvetica", text_y, text_x, silver_c);
    }

    MakeMovable(menu, this, app);

    if (pic_name != nullptr) {
        Texture pic(x_size, y_size, pic_name, 0, 0);
        pic.showOn(&base_img);
        pic.showOn(this);
    }

    this->addChild(canvas);
    this->addChild(menu);
};

GraphWindow::GraphWindow(int x_size, int y_size, int coord_x, int coord_y, ManagerWindow* parent, Renderer* render,
                         COLORREF backgroud_c, COLORREF border_c, COLORREF line_c, COLORREF net_c) :
    BorderWindow(x_size, y_size, coord_x, coord_y, backgroud_c, border_c, 2, render, parent),
    line_color(line_c), net_color(net_c), left_dot_pos(y_size), right_dot_pos(0) {
    need_redraw = true;
    this->draw(render);
};


void GraphWindow::draw(Renderer* render) const {
    if (!is_shown) {
        return;
    }
    
    render->setWindow(const_cast<GraphWindow*>(this));

    if (need_redraw) {
        render->drawFilledRectangle(0, 0, size.x, size.y, color, border_color, thickness);
        int delta = 15;
        for (int i = delta; i < size.y; i += delta) {     
            render->drawLine(thickness, i, size.x - thickness, i, net_color, 1); //horizontal lines
        }
        for (int i = delta; i < size.x; i += delta) {
            render->drawLine(i, thickness, i, size.y - thickness, net_color, 1); //vertical lines
        }

        int radius = 3;
        int left_x  = 0;
        int right_x = size.x - (thickness);
        //render->drawCircle(left_x , left_dot_pos , radius, black_c);
        //render->drawCircle(right_x, right_dot_pos, radius, black_c);
        render->drawLine(left_x, left_dot_pos - thickness, right_x, right_dot_pos, line_color, 2);
        const_cast<GraphWindow*>(this)->setRedraw(false);
    }
    if (this->getParent()) {
        this->showOn(this->getParent());
    }
};

RoundWindow::RoundWindow(int radius, int coord_x, int coord_y, COLORREF color, COLORREF border_color, int thickness, Renderer* render,
                         ManagerWindow* parent, VFunctor* press_up_f, VFunctor* pointed_f, VFunctor* press_down_f) :
    ManagerWindow(2 * radius, 2 * radius, coord_x, coord_y, color, parent, press_up_f, pointed_f, press_down_f), radius(radius), border_color(border_color), thickness(thickness) {
    need_redraw = true;
    this->draw(render);
    need_redraw = false;
};

void RoundWindow::draw(Renderer* render) const {
    if (!is_shown) {
        return;
    }
    
    render->setWindow(const_cast<RoundWindow*>(this));

    if (need_redraw) {
        render->drawCircle(radius, radius, radius, color, border_color, thickness);
        this->drawChilds(render);
    }

    this->showOnTexture(this->getParent());
};

bool RoundWindow::hitTest(double x, double y) const {
    return ((radius - x) * (radius - x) + (radius - y) * (radius - y)) < (radius * radius);
};

DedWindow::DedWindow(int radius, int size_x, int size_y, int coord_x, int coord_y, COLORREF color, COLORREF border_color, int thickness, Renderer* render,
                     ManagerWindow* parent, VFunctor* press_up_f, VFunctor* pointed_f, VFunctor* press_down_f) :
    ManagerWindow(size_x, size_y, coord_x, coord_y, color, parent, press_up_f, pointed_f, press_down_f), radius(radius), border_color(border_color),  thickness(thickness) {
    need_redraw = true;
    this->draw(render);
    need_redraw = false;
}

void DedWindow::draw(Renderer* render) const {
    if (!is_shown) {
        return;
    }
    
    render->setWindow(const_cast<DedWindow*>(this));

    if (need_redraw) {
        render->drawFilledRectangle(0, 0, size.x, size.y, color, border_color, thickness);
        this->drawChilds(render);
    }

    if (this->getParent()) {
        this->showOnTexture(this->getParent());
    }
};

bool DedWindow::hitTest(double x, double y) const {
    bool out_of_circle = ((size.x / 2 - x) * (size.x / 2 - x) + (size.y / 2 - y) * (size.y / 2 - y)) > (radius * radius);
    bool in_round_rect = pow((y - double(size.y) / 2) / (double(size.y) / 2), 8) + pow((x - double(size.x) / 2) / (double(size.x) / 2), 12) < 1;
    return out_of_circle && in_round_rect;
}
