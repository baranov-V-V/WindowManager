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

DisplayManager::DisplayManager(int max_count) {
    
};

DisplayManager::~DisplayManager() {
    for (int i = 0; i < count; ++i) {
        this->delLast();
    }
    delete[] windows;
};

void DisplayManager::addWindow(ManagerWindow* window) {
    if (count < size) {
        windows[count++] = window;
    } else {
        std::cerr << "Overflow!!" << std::endl;
    }
};

void DisplayManager::showWindow(ManagerWindow* window) {
    int pos = 0;
    for (int i = 0; i < count; ++i) {
        if (windows[i] == window) {
            
        }
    }
};

void DisplayManager::hideWindow(ManagerWindow* window) {
    int pos = 0;
    for (int i = 0; i < count; ++i) {
        if (windows[i] == window) {
            for (int j = i; j < count - 1; ++j) {
                windows[j] = windows[j + 1];   
            }
        }
    }
};

void DisplayManager::delWindow(ManagerWindow* window) {
    int pos = 0;
    for (int i = 0; i < count; ++i) {
        if (windows[i] == window) {
            for (int j = i; j < count - 1; ++j) {
                windows[j] = windows[j + 1];   
            }
            count--;
            break;
        }
    }
};

void DisplayManager::delLast() {
    windows[--count] = nullptr;
};

ManagerWindow::ManagerWindow(int x_size, int y_size, int coord_x, int coord_y, COLORREF color, ManagerWindow* parent,
                             VFunctor* press_up_f, VFunctor* pointed_f, VFunctor* press_down_f) :
    Texture(x_size, y_size, color, coord_x, coord_y), parent(parent) {

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
                mouse->setToParent();
                return true;
            }
        }
        is_clicked = true;
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
                mouse->setToParent();
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
    Pair<int> rel_coord_old = mouse->getRelCoord();
    this->markPointedWindows(mouse);
    mouse->setRelCoord(rel_coord_old);
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
    if (need_redraw) {
        render->drawRectangle(0, 0, size.x, size.y, border_color, thickness);
    }
    this->drawChilds(render);
    this->showOn(this->getParent());
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
    this->showOn(this->getParent());
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
    
void PicWindow::draw(Renderer* render) const {
    render->setWindow(const_cast<PicWindow*>(this));
    
    if (need_redraw) {
        base_img.showOn(this);
    }
    
    this->drawChilds(render);

    if (this->getParent()) {
        this->showOn(this->getParent());
    }
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
    this->showOn(this->getParent());
};

InvisibleWindow::InvisibleWindow(int x_size, int y_size, int coord_x, int coord_y, ManagerWindow* parent,
                                 VFunctor* press_up_f, VFunctor* pointed_f, VFunctor* press_down_f, bool need_redraw) :
    ManagerWindow(x_size, y_size, coord_x, coord_y, 0, parent, press_up_f, pointed_f, press_down_f) {
};
    
void InvisibleWindow::draw(Renderer* render) const {
    
    for (int i = 0; i < this->getCount(); ++i) {
        this->getChild(i)->draw(render);
        render->setWindow(this->getParent());
        this->getChild(i)->showOn(this->getParent(), this->getChild(i)->getCoordX() + coord.x, this->getChild(i)->getCoordY() + coord.y);
    }
    
};

TextButtonWindow::TextButtonWindow(int x_size, int y_size, int coord_x, int coord_y, COLORREF color, COLORREF border_color, int thickness,
                                   COLORREF text_color, const char* text, const char* font_name, int ch_size_x, int ch_size_y, int align, Renderer* render,
                                   ManagerWindow* parent, VFunctor* press_up_f, VFunctor* pointed_f, VFunctor* press_down_f) : 
    BorderWindow(x_size, y_size, coord_x, coord_y, color, border_color, thickness, render, parent, press_up_f, pointed_f, press_down_f), 
    text(text), font_name(font_name), ch_size_x(ch_size_x), ch_size_y(ch_size_y), align(align), text_color(text_color) {
    need_redraw = true;
    this->draw(render);
    need_redraw = false;
};

void TextButtonWindow::draw(Renderer* render) const {
    
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
    this->showOn(this->getParent());
};

CanvasWindow::CanvasWindow(int x_size, int y_size, int coord_x, int coord_y, char* name, ManagerWindow* parent,
                           Renderer* render, Feather* feather, WindowMouse* mouse, const char* pic_name) :
    BorderWindow(x_size, y_size, coord_x, coord_y, white_c, mgrey_c, 1, render, parent), name(name), on_display(true),
    base_img(x_size, y_size, white_c, 0, 0) {
    
    need_redraw = false;

    DrawFunctor* canvas_functor = new DrawFunctor(this, render, feather, mouse);
    MakeFirst* make_first = new MakeFirst(parent);
    this->setPressDown(make_first);
    this->setPointed(canvas_functor);

    int menu_x = x_size;
    int menu_y = 25;

    int but_x = 30;
    PicWindow* menu = MakeBasicMenu(menu_x, menu_y, 0, 0, this, but_x);

    render->setWindow(menu);
    int text_size = strlen(name) * menu_y / 4;
    if (text_size < menu_x - 2 * but_x) {
        int text_y = 4 * menu_y / 5;
        int text_x = text_y / 3;
        int ident_y = (menu_y - text_y) / 2;
        render->drawText(menu_x / 2 - 30 * 2 - text_size / 2, ident_y, name, "Helvetica", text_y, text_x, silver_c);
        TextButtonWindow* button_reset  = new TextButtonWindow(7 * (text_x + 1), menu_y, 0, 0, mgrey_c, mgrey_c, 1, silver_c, "Clear", "Helvetica", text_x + 1, text_y - 2, ALIGN_LEFT, render, menu);
        GlowBorderFunctor* glow_button_reset = new GlowBorderFunctor(button_reset, lgrey_c, lgrey_c);
        ClearCanvas* clear_f = new ClearCanvas(this, render);
        button_reset->setPointed(glow_button_reset);
        button_reset->setPressUp(clear_f);
        menu->addChild(button_reset);
    }


    MakeMovable(menu, parent, mouse);

    if (pic_name != nullptr) {
        Texture pic(x_size, y_size, pic_name, 0, 0);
        pic.showOn(&base_img);
        pic.showOn(this);
    }

    this->addChild(menu);
};

CanvasWindow::CanvasWindow(int x_size, int y_size, int coord_x, int coord_y, CanvasWindow* window, Renderer* render, Feather* feather, WindowMouse* mouse) :
    BorderWindow(x_size, y_size, coord_x, coord_y,
    white_c, mgrey_c, 1, render, window->getParent()), name(name), on_display(true),
    base_img(window->getBaseImg().getSizeX(), window->getBaseImg().getSizeY(), white_c, 0, 0) {
    
    need_redraw = false;

    DrawFunctor* canvas_functor = new DrawFunctor(this, render, feather, mouse);
    MakeFirst* make_first = new MakeFirst(this->getParent());
    this->setPressDown(make_first);
    this->setPointed(canvas_functor);

    int menu_x = this->getSizeX();
    int menu_y = 25;

    int but_x = 30;
    PicWindow* menu = MakeBasicMenu(menu_x, menu_y, 0, 0, this, but_x);

    render->setWindow(menu);
    int text_size = strlen(name) * menu_y / 4;
    if (text_size < menu_x - 2 * but_x) {
        int text_y = 4 * menu_y / 5;
        int text_x = text_y / 3;
        int ident_y = (menu_y - text_y) / 2;
        render->drawText(menu_x / 2 - 30 * 2 - text_size / 2, ident_y, name, "Helvetica", text_y, text_x, silver_c);
    }

    MakeMovable(menu, window->getParent(), mouse);

    window->showOn(this);
    window->getBaseImg().showOn(&base_img);

    this->addChild(menu);
};

void CanvasWindow::draw(Renderer* render) const {
    render->setWindow(const_cast<CanvasWindow*>(this));
    
    this->drawChilds(render);

    if (this->getParent()) {
        this->showOn(this->getParent());
    } else {
        cout << "no parent\n";
    }
};

GraphWindow::GraphWindow(int x_size, int y_size, int coord_x, int coord_y, ManagerWindow* parent, Renderer* render,
                         COLORREF backgroud_c, COLORREF border_c, COLORREF line_c, COLORREF net_c) :
    BorderWindow(x_size, y_size, coord_x, coord_y, backgroud_c, border_c, 2, render, parent),
    line_color(line_c), net_color(net_c), left_dot_pos(y_size), right_dot_pos(0) {
    need_redraw = true;
    this->draw(render);
};


void GraphWindow::draw(Renderer* render) const {
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

    this->showOn(this->getParent());
};
