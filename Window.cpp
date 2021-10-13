#include "sphere.h"

int Frame::getSizeX() const {
    return size.x;
};

int Frame::getSizeY() const {
    return size.y;
};

int Texture::getCoordX() const {
    return coord.x;
};

int Texture::getCoordY() const {
    return coord.y;
};


VirtualWindow::VirtualWindow(int x_size, int y_size, COLORREF color) : Frame(x_size, y_size), color(color) {
    screen = nullptr;
    buf_screen = nullptr;
    type = 0;
};

HDC VirtualWindow::getHdc() const {
    return screen;
};

RGBQUAD* VirtualWindow::getBuf() const {
    return buf_screen;
};

COLORREF VirtualWindow::getColor() const {
    return color;
}

Renderer::Renderer(VirtualWindow* window, Coordinates* coord) :
    window(window), coordinates(coord) {
    this->calcScale();    
};

void Renderer::calcScale() {
    scale.x = double(window->getSizeX()) / double(coordinates->getMaxX() - coordinates->getMinX());
    scale.y = double(window->getSizeY()) / double(coordinates->getMaxY() - coordinates->getMinY()); 
};

void Renderer::setWindow(VirtualWindow* new_window) {
    window = new_window;
    Coordinates* coord = this->getCoordinates();
    scale.x = double(window->getSizeX()) / double(coord->getMaxX() - coord->getMinX());
    scale.y = double(window->getSizeY()) / double(coord->getMaxY() - coord->getMinY());    
};

int Renderer::toPixelX(double coord) const {
    return (coord - this->getCoordinates()->getMinX()) * scale.x;
};

int Renderer::toPixelY(double coord) const {
    return (coord - this->getCoordinates()->getMinY()) * scale.y;
};

double Renderer::toCoordX(int x) const {
    return double(x) / scale.x + this->getCoordinates()->getMinX();
};

double Renderer::toCoordY(int y) const {
    return double(y) / scale.y + this->getCoordinates()->getMinY();
};