//graphics engine

#include "WindowManager.h"
#include "TXLib.h"

BasicWindow::BasicWindow(int x_size, int y_size, COLORREF color) :
    size(x_size, y_size), color(color) {};

HDC BasicWindow::getHdc() const {
    return screen;
};

COLORREF BasicWindow::getColor() const {
    return color;
}

Window::Window(int x_size, int y_size, COLORREF color) : BasicWindow(x_size, y_size, color) {    
    txCreateWindow(x_size, y_size);
    screen = txDC();
    type = TYPE_WINDOW;
    
    txSetFillColor(color, screen);
    txRectangle(0, 0, x_size, y_size, screen);
};

Texture::Texture(int x_size, int y_size, COLORREF color, int coord_x, int coord_y) :
    BasicWindow(x_size, y_size, color), coord(coord_x, coord_y) {
    type = TYPE_TEXTURE;
    screen_buf = nullptr;
    screen = txCreateDIBSection(x_size, y_size, &screen_buf);
    
    txSetFillColor(color, screen);
    txRectangle(0, 0, x_size, y_size, screen);
}

Texture::~Texture() {
    txDeleteDC(screen);
}

void Texture::showOn(const BasicWindow* target) const {
    txBitBlt(target->getHdc(), coord.x, coord.y, 0, 0, this->getHdc());
}

void Renderer::drawLine(double x_begin, double y_begin, double x_end, double y_end, COLORREF color, int thickness) const {
    txSetColor(color, thickness, window->getHdc());
    txLine(this->toPixelX(x_begin), this->toPixelY(y_begin),
           this->toPixelX(x_end),   this->toPixelY(y_end), window->getHdc());
};

void Renderer::setPixel(double x, double y, COLORREF color) const {
    txSetPixel(this->toPixelX(x), this->toPixelY(y), color, window->getHdc());
};

void Renderer::drawCircle(double x, double y, double r, COLORREF color, int thickness) const {
    cerr << "drawing on [" << window << "]\n";
    cerr << "x: " << x << " y: " << y << " r: " << r << " thick: " << thickness << " color: " << color << "\n";
    txSetColor(color, thickness, window->getHdc());
    //txSetFillColor(color, window->getHdc());
    txEllipse(this->toPixelX(x - r), this->toPixelY(y + r),
              this->toPixelX(x + r), this->toPixelY(y - r), window->getHdc());
};

void Renderer::drawRectangle(double x1, double y1, double x2, double y2, COLORREF color, int thickness) const {
    txSetColor(color, thickness, window->getHdc());
    txRectangle(this->toPixelX(x1), this->toPixelY(y1),
                this->toPixelX(x2), this->toPixelY(y2), window->getHdc());
};

void Renderer::drawRoundRect(double x1, double y1, double x2, double y2, double width, double height, COLORREF color, int thickness) const {
    txSetColor(color, thickness, window->getHdc());
    Win32::RoundRect(window->getHdc(), this->toPixelX(x1), this->toPixelY(y1), 
                     this->toPixelX(x2), this->toPixelY(y2), width * this->getScaleX(), height * this->getScaleY());
};

void Renderer::clear() const {
    txClear(window->getHdc());
}

void WindowMouse::update() {
    abs_coord.x = txMouseX();
    abs_coord.y = txMouseY();
    state = txMouseButtons();

    if (window->getParent() != nullptr) {
        assert("Wrong call of Mouse::update()");
    }

    rel_coord = abs_coord;
}

void App::sleep(int millisec) const {
    txSleep(millisec);
}

