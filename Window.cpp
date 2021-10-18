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

ManagerWindow::ManagerWindow(int x_size, int y_size, COLORREF color, int coord_x, int coord_y, VFunctor* functor, ManagerWindow* parent) :
    Texture(x_size, y_size, color, coord_x, coord_y), parent(parent) {
    if (functor == nullptr) {
        ManagerWindow::functor = nullptr;
    } else {
        ManagerWindow::functor = functor;
    }
    max_size = 100;
    count = 0;
    ManagerWindow::children = new ManagerWindow*[max_size];
}

ManagerWindow::~ManagerWindow() {
    for (int i = 0; i < count; ++i) {
        this->delLast();
    }
    delete[] children;
    delete functor;
};

void ManagerWindow::drawChilds(Renderer* renderer) const {
    //cerr << "trying draw childs for: [" << this << "]\n";
    for (int i = count - 1; i >= 0; --i) {
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

void ManagerWindow::delLast() {
    children[--count] = nullptr;
};

bool ManagerWindow::checkLeftClick(WindowMouse* mouse) {
    int x = mouse->getRelCoord().x;
    int y = mouse->getRelCoord().y;
    return (x > 0 && x < this->getSizeX()) && (y > 0 && y < this->getSizeY()) && (mouse->getState() & LEFT_CLICK != 0);
};

bool ManagerWindow::proceedClicks(WindowMouse* mouse) {
    mouse->setWindow(this);
    if (this->checkLeftClick(mouse)) {
        //cerr << "in window [" << this << "]\n";
        //mouse->printState();
        for (int i = count - 1; i >= 0; --i) {
            if (children[i]->proceedClicks(mouse)) {
                //mouse->setToParent();
                return true;
            }
        }
        bool result = this->action();
        mouse->setToParent();
        return result;
    }
    mouse->setToParent();
    return false;
};

bool ManagerWindow::action() {
    return functor->action();
};

BorderWindow::BorderWindow(int x_size, int y_size, COLORREF color, int coord_x, int coord_y, VFunctor* functor,
                           ManagerWindow* parent, COLORREF border_color, int thickness) : 
    ManagerWindow(x_size, y_size, color, coord_x, coord_y, functor, parent), border_color(border_color), thickness(thickness) {
    cerr << "constructed: [" << this << "]\n";
};
    
void BorderWindow::draw(Renderer* render) const {
    //cerr << "started drawing: [" << this << "]\n";
    render->setWindow(const_cast<BorderWindow*>(this));
    render->drawRectangle(0, 0, size.x, size.y, border_color, thickness);
    this->drawChilds(render);
    //cerr << "ended drawing: [" << this << "]\n";
};



CanvasWindow::CanvasWindow(int x_size, int y_size, COLORREF color, int coord_x, int coord_y, VFunctor* functor,
                           ManagerWindow* parent, COLORREF border_color, int thickness) : 
    BorderWindow(x_size, y_size, color, coord_x, coord_y, functor, parent, border_color, thickness) {};



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
