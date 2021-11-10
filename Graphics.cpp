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
    hWnd = txCreateWindow(x_size, y_size);
    screen = txDC();
    type = TYPE_WINDOW;
    
    txSetFillColor(color, screen);
    txRectangle(0, 0, x_size, y_size, screen);
    //cout << hWnd << "\n";

    HINSTANCE hInst;
    hInst = GetModuleHandle(NULL); //(HINSTANCE)GetWindowLong(app.users_window.getHwnd(), -6);
        if ((hInst))
        {
            //printf("Successful");
        }
    
    //HINSTANCE hInst = (HINSTANCE) GetWindowLongA(hWnd, GWLP_HINSTANCE); //
    //assert(hInst != 0);

    //HCURSOR cursor = LoadCursor(NULL, IDC_ARROW);
    //assert(cursor != NULL);

    //CopyCursor(cursor);

    //SetSystemCursor(cursor , 32515);

    //SetClassLong(hWnd, GCL_HCURSOR, (LONG) cursor);
    //SetCursor(cursor);
};

Texture::Texture(int x_size, int y_size, COLORREF color, int coord_x, int coord_y) :
    BasicWindow(x_size, y_size, color), coord(coord_x, coord_y) {
    type = TYPE_TEXTURE;
    screen_buf = nullptr;
    screen = txCreateDIBSection(x_size, y_size, &screen_buf);
    
    txSetFillColor(color, screen);
    txRectangle(0, 0, x_size, y_size, screen);
}

Texture::Texture(int x_size, int y_size, const char* file_name, int coord_x, int coord_y) :
    BasicWindow(x_size, y_size, 0), coord(coord_x, coord_y) {
    type = TYPE_TEXTURE;
    screen_buf = nullptr;
    screen = txLoadImage(file_name, x_size, y_size);
    assert(screen);
};

Texture::~Texture() {
    txDeleteDC(screen);
}

void Texture::showOn(const BasicWindow* target) const {
    txBitBlt(target->getHdc(), coord.x, coord.y, size.x, size.y, this->getHdc());
}

void Texture::showOn(const BasicWindow* target, int coord_x, int coord_y) {
    txBitBlt(target->getHdc(), coord_x, coord_y, size.x, size.y, this->getHdc());
};

void Renderer::drawLine(double x_begin, double y_begin, double x_end, double y_end, COLORREF color, int thickness) const {
    txSetColor(color, thickness, window->getHdc());
    txLine(this->toPixelX(x_begin), this->toPixelY(y_begin),
           this->toPixelX(x_end),   this->toPixelY(y_end), window->getHdc());
};

void Renderer::setPixel(double x, double y, COLORREF color) const {
    txSetPixel(this->toPixelX(x), this->toPixelY(y), color, window->getHdc());
};

void Renderer::drawCircle(double x, double y, double r, COLORREF color, COLORREF border_color, int thickness) const {
    //cerr << "drawing on [" << window << "]\n";
    //cerr << "x: " << x << " y: " << y << " r: " << r << " thick: " << thickness << " color: " << color << "\n";
    txSetColor(border_color, thickness, window->getHdc());
    txSetFillColor(color, window->getHdc());
    txEllipse(this->toPixelX(x - r), this->toPixelY(y + r),
              this->toPixelX(x + r), this->toPixelY(y - r), window->getHdc());
};

void Renderer::drawRectangle(double x1, double y1, double x2, double y2, COLORREF color, int thickness) const {
    txSetColor(color, thickness, window->getHdc());
    txRectangle(this->toPixelX(x1), this->toPixelY(y1),
                this->toPixelX(x2), this->toPixelY(y2), window->getHdc());
};

void Renderer::drawFilledRectangle(double x1, double y1, double x2, double y2, COLORREF fill_color, COLORREF color, int thickness) const {
    txSetColor(color, thickness, window->getHdc());
    txSetFillColor(fill_color, window->getHdc());
    txRectangle(this->toPixelX(x1), this->toPixelY(y1),
                this->toPixelX(x2), this->toPixelY(y2), window->getHdc());
};

void Renderer::drawRoundRect(double x1, double y1, double x2, double y2, double width, double height, COLORREF color, int thickness) const {
    txSetColor(color, thickness, window->getHdc());
    Win32::RoundRect(window->getHdc(), this->toPixelX(x1), this->toPixelY(y1), 
                     this->toPixelX(x2), this->toPixelY(y2), width * this->getScaleX(), height * this->getScaleY());
};

void Renderer::drawText(double x, double y, const char* text, const char* font_name, int size_y, int size_x, COLORREF color, int thickness) {
    txSetColor(color, 1, window->getHdc());
    assert(txFontExist(font_name));
    txSelectFont(font_name, size_y, size_x, FW_DONTCARE, false, false, false, 0, window->getHdc());
    txTextOut(x, y, text, window->getHdc());
};

void Renderer::clear() const {
    txClear(window->getHdc());
}

void WindowMouse::update() {
    this->printPos();

    int dx = abs_coord.x - rel_coord.x;
    int dy = abs_coord.y - rel_coord.y;

    abs_coord.x = txMouseX();
    abs_coord.y = txMouseY();
    state = txMouseButtons();

    if (window->getParent() != nullptr) {
        assert("Wrong call of Mouse::update()");
    }

    rel_coord.x = abs_coord.x;
    rel_coord.y = abs_coord.y;
}

void App::sleep(int millisec) const {
    txSleep(millisec);
}

