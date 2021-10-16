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

ManagerWindow::ManagerWindow(int x_size, int y_size, COLORREF color, int coord_x, int coord_y, Vfunctor* functor, ManagerWindow* parent) :
    Texture(x_size, y_size, color, coord_x, coord_y), parent(parent), functor(nullptr) {
    max_size = 100;
    count = 0;
    ManagerWindow::children = new ManagerWindow*[max_size];
}

ManagerWindow::~ManagerWindow() {
    for (int i = 0; i < count; ++i) {
        this->delLast();
    }
    delete[] children;
};

void ManagerWindow::drawAll(Renderer* renderer) const {
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

void ManagerWindow::delLast() {
    children[--count] = nullptr;
};

Pair<int>* checkPress() {
    return nullptr;
};

ManagerWindow* getPress() {
    return nullptr;
};

bool ManagerWindow::action() {
    functor->action();
};

BorderWindow::BorderWindow(int x_size, int y_size, COLORREF color, int coord_x, int coord_y, Vfunctor* functor,
                           ManagerWindow* parent, COLORREF border_color, int thickness) : 
    ManagerWindow(x_size, y_size, color, coord_x, coord_y, functor, parent), border_color(border_color), thickness(thickness) {};
    
void BorderWindow::draw(Renderer* render) const {
    this->drawAll(render);
    render->setWindow(const_cast<BorderWindow*>(this));
    render->drawRectangle(0, 0, size.x, size.y, border_color, thickness);
};