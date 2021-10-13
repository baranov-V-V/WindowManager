//graphics engine

#include "sphere.h"

Window::Window(int x_size, int y_size, COLORREF color) : VirtualWindow(x_size, y_size, color) {    
    txCreateWindow(x_size, y_size);
    screen = txDC();
    buf_screen = txVideoMemory();
    type = TYPE_WINDOW;
    
    txSetFillColor(color, screen);
    txRectangle(0, 0, x_size, y_size, screen);
};

Texture::Texture(int x_size, int y_size, COLORREF color, int coord_x, int coord_y) :
    VirtualWindow(x_size, y_size, color), coord(coord_x, coord_y) {
    type = TYPE_TEXTURE;
    screen = txCreateDIBSection(x_size, y_size, &buf_screen);
    
    txSetFillColor(color, screen);
    txRectangle(0, 0, x_size, y_size, screen);
}

Texture::~Texture() {
    txDeleteDC(screen);
}

void Texture::show_on(const Window* window) const {
    txBitBlt(window->getHdc(), coord.x, coord.y, 0, 0, screen);
}

void Renderer::drawLine(double x_begin, double y_begin, double x_end, double y_end, COLORREF color, int thickness) const {
    txSetColor(color, thickness, window->getHdc());
    txLine(this->toPixelX(x_begin), window->getSizeY() - this->toPixelY(y_begin),
           this->toPixelX(x_end), window->getSizeY() -  this->toPixelY(y_end), window->getHdc());
};

void Renderer::setPixel(double x, double y, COLORREF color) const {
    txSetPixel(this->toPixelX(x), window->getSizeY() - this->toPixelY(y), color, window->getHdc());
};

void Renderer::drawCircle(double x, double y, double r, COLORREF color, int thickness) const {
    txSetColor(color, thickness, window->getHdc());
    txEllipse(this->toPixelX(x - r), window->getSizeY() - this->toPixelY(y + r),
              this->toPixelX(x + r), window->getSizeY() - this->toPixelY(y - r), window->getHdc());
};

void Renderer::drawRectangle(double x1, double y1, double x2, double y2, COLORREF color, int thickness) const {
    txSetColor(color, thickness, window->getHdc());
    txRectangle(this->toPixelX(x1), window->getSizeY() - this->toPixelY(y1),
                this->toPixelX(x2), window->getSizeY() - this->toPixelY(y2), window->getHdc());
};

void Renderer::drawRoundRect(double x1, double y1, double x2, double y2, double width, double height, COLORREF color, int thickness) const {
    txSetColor(color, thickness, window->getHdc());
    Win32::RoundRect(window->getHdc(), this->toPixelX(x1), window->getSizeY() - this->toPixelY(y1), this->toPixelX(x2),
              window->getSizeY() - this->toPixelY(y2), width * this->getScaleX(), height * this->getScaleY());
};

void Renderer::clear() const {
    txClear(window->getHdc());
}

void PrintMousePos() {
    fprintf(stderr, "x:%lg, y:%lg\n", txMouseX(), txMouseY());
}

void TextButton::draw(const Renderer& render) const {
    double x = 0, y = 0;
    double c_x = coord.x + size.x / 2, c_y = coord.y + size.y / 2;
    GetMouse(x, y, render);    
    
    int red = std::min(double(GetRValue(color)) * 1.5 + (255 - double(GetRValue(color)) * 1.5) / 200 * sqrt((c_x - x) * (c_x - x) + (c_y - y) * (c_y - y)), double(255));
    int green = std::min(double(GetGValue(color)) * 1.5 + (255 - double(GetGValue(color)) * 1.5) / 300 * sqrt((c_x - x) * (c_x - x) + (c_y - y) * (c_y - y)), double(255));
    int blue = std::min(double(GetBValue(color)) * 1.5 + (255 - double(GetBValue(color)) * 1.5) / 400 * sqrt((c_x - x) * (c_x - x) + (c_y - y) * (c_y - y)), double(255));

    txSetFillColor(RGB(red, green, blue), render.getWindow()->getHdc());
    render.drawRoundRect(coord.x, coord.y, coord.x + size.x, coord.y + size.y, size.x / 4, size.y / 3, color, 4);

    txSelectFont("Comic Sans MS", size.y / 2, size.x / len * 2 / 3, FW_DONTCARE, false, false, false, 0, render.getWindow()->getHdc());
    txTextOut(coord.x + size.x / 6, render.getWindow()->getSizeY() - (coord.y + size.y * 3 / 4), text, render.getWindow()->getHdc());
};

void GetMouse(double& x, double& y, const Renderer& render) {
    x = txMouseX();
    y = render.getWindow()->getSizeY() - txMouseY();  
    if (render.getWindow()->getType() == TYPE_TEXTURE) {
        x -= ((Texture*)render.getWindow())->getCoordX();
        y += ((Texture*)render.getWindow())->getCoordY();
    }
}

bool Button::checkBoundary(const Renderer& render) const {
    double x = 0, y = 0;
    GetMouse(x, y, render);
    return (coord.x < x && (coord.x + size.x) > x) && (coord.y < y && (coord.y + size.y) > y);
};

bool Button::checkPress(const Renderer& render) const {
    unsigned int press = txMouseButtons();  
    return this->checkBoundary(render) && (press & 1);
}

void PhysCircle::draw(const Renderer& renderer) const {
    VirtualWindow* window = renderer.getWindow();
    Coordinates* coordinates = renderer.getCoordinates(); 
    RGBQUAD* win_buf = window->getBuf();
    RGBQUAD place_color = ToRGBQUAD(color);

    int x1 = renderer.toPixelX(coord.x - radius);
    int x2 = renderer.toPixelX(coord.x + radius);
    int y1 = renderer.toPixelY(coord.y + radius);
    int y2 = renderer.toPixelY(coord.y - radius);
    
    for(int iy = y2; iy < y1; ++iy) {
        for(int ix = x1; ix < x2; ++ix) {
            int buf_pos = ix + iy * window->getSizeX();
            if (this->checkBound(renderer.toCoordX(ix), renderer.toCoordY(iy)) &&
                (buf_pos < window->getSizeX() * window->getSizeY()) && buf_pos >= 0) {
                win_buf[buf_pos] = place_color;
            }
        }
    }

};

void PhysRect::draw(const Renderer& renderer) const {
    VirtualWindow* window = renderer.getWindow();
    Coordinates* coordinates = renderer.getCoordinates(); 
    RGBQUAD* win_buf = window->getBuf();
    RGBQUAD place_color = ToRGBQUAD(color);

    int x1 = renderer.toPixelX(std::max(coord1.x, coordinates->getMinX()));
    int y1 = renderer.toPixelY(std::max(coord1.y, coordinates->getMinY()));
    int x2 = renderer.toPixelX(std::min(coord2.x, coordinates->getMaxX()));
    int y2 = renderer.toPixelY(std::min(coord2.y, coordinates->getMaxY()));

    for(int iy = y1; iy < y2; ++iy) {
        for(int ix = x1; ix < x2; ++ix) {
            int buf_pos = ix + iy * window->getSizeX();
            if ((buf_pos < (window->getSizeX() * window->getSizeY())) && buf_pos >= 0) {
                win_buf[buf_pos] = place_color;
            }
        }
    }
};

void Graph::draw(const Renderer& render) const {
    for(int i = 0; i < dots_count - 1; ++i) {
        double y1 = std::min(dots[i], render.getCoordinates()->getMaxY() * 13 / 15);
        double y2 = std::min(dots[i + 1], render.getCoordinates()->getMaxY() * 13 / 15);
        render.drawLine(i, y1, i + 1, y2, TX_RED, 2);
    }
};

void Graph::drawAxis(const Renderer& render) {
    VirtualWindow* target = render.getWindow();
    Coordinates* coord = render.getCoordinates();

    double size_x = target->getSizeX();
    double size_y = target->getSizeY();
    HDC hdc = target->getHdc();

    const int edge_thickness = 2;
    const int axis_thickness = 2;

    double font_x = size_x / 60;
    double font_y = size_y / 15;

    txSetColor(TX_BLACK, axis_thickness, hdc);
    txSelectFont("Comic Sans MS", font_y, font_x, FW_DONTCARE, false, false, false, 0, hdc);

    txTextOut(size_x * 1 / 3, font_y * 1 / 2, "Particles", hdc);     //max_y    

    txSetColor(TX_BLACK, edge_thickness, hdc);
    txSetFillColor(target->getColor(), hdc);
    txRectangle(0, font_y * 2, target->getSizeX(), target->getSizeY(), hdc);


    txSelectFont("Comic Sans MS", font_y * 2, font_x * 2, FW_DONTCARE, false, false, false, 0, hdc);
    //!numbers
    char num_buff[4] = {};
    txTextOut(size_x - edge_thickness * 4 - font_x * 4.5, edge_thickness * 2 + font_y * 2, itoa(coord->getMaxY(), num_buff, 10), hdc);      //max_y    
    txTextOut(size_x - edge_thickness * 4 - font_x * 1.5, size_y - edge_thickness - font_y * 2, itoa(coord->getMinY(), num_buff, 10), hdc); //min_y    
};

void PhysSphere::draw(const Renderer& renderer) const {
    VirtualWindow* window = renderer.getWindow();
    Coordinates* coordinates = renderer.getCoordinates(); 
    RGBQUAD* win_buf = window->getBuf();

    int x1 = renderer.toPixelX(coord.x - radius);
    int x2 = renderer.toPixelX(coord.x + radius);
    int y1 = renderer.toPixelY(coord.y + radius);
    int y2 = renderer.toPixelY(coord.y - radius);

    for(int iy = y2; iy < y1; ++iy) {
        for(int ix = x1; ix < x2; ++ix) {
            int buf_pos = ix + iy * window->getSizeX();
            if (!this->checkBound(renderer.toCoordX(ix), renderer.toCoordY(iy))) {
                continue;
            }
            double x = renderer.toCoordX(ix) - coord.x;
            double y = renderer.toCoordY(iy) - coord.y; 

            if (radius * radius - x * x - y * y < 0) {
                continue;
            }
            vec3d normal(x, y, sqrt(radius * radius - x * x - y * y));
            vec3d n_norm = ~normal;
            vec3d v_norm = ~(view_pos - normal);
            vec3d lm_norm = ~(light_pos - normal);
            vec3d rm_norm = 2 * (lm_norm * n_norm) * n_norm - lm_norm;

            double k_diff = n_norm * lm_norm;
            double product = rm_norm * v_norm;
            
            if (k_diff < 0) {
                k_diff = 0;
            }
            if (product < 0) {
                product = 0;
            }
            double k_spec = pow(product, 30);
            
            vec_c sphere_color = ToVec(color);
            double red = ambient_intense.getX() * sphere_color.getX() + k_diff * light_intense.getX() * sphere_color.getX() + k_spec * light_intense.getX();
            double green = ambient_intense.getY() * sphere_color.getY() + k_diff * light_intense.getY() * sphere_color.getY() + k_spec * light_intense.getY();
            double blue = ambient_intense.get_z() * sphere_color.get_z() + k_diff * light_intense.get_z() * sphere_color.get_z() + k_spec * light_intense.get_z();
            if ((buf_pos < window->getSizeX() * window->getSizeY()) && buf_pos >= 0) {
                win_buf[buf_pos] = ToRGBQUAD(CorrectColor(red) * 255, CorrectColor(green) * 255, CorrectColor(blue) * 255);
            }
        }
    }

};

double CorrectColor(double color) {
    assert(isfinite(color));
    if (color >= 1) {
        color = 1;
    }
    if (color <= 0) {
        color = 0;
    }
    return color;
}

