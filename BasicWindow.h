#pragma once
#include "BasicInfo.h"

class BasicWindow {
  public:
    BasicWindow();
    BasicWindow(int x_size, int y_size, COLORREF color);
    virtual ~BasicWindow() {};

    int getSizeX() const { return size.x; };
    int getSizeY() const { return size.y; };
    Pair<int> getSize() const { return size; };

    void setSizeX(int size_x) { size.x = size_x; };
    void setSizeY(int size_y) { size.y = size_y; };
    void setSize(Pair<int> new_size) { size = new_size; };

    HDC getHdc() const;
    int getType() const { return type; };
    COLORREF getColor() const;
    void setColor(COLORREF new_color) { color = new_color; };

  protected:
    int type;
    Pair<int> size;
    HDC screen;
    COLORREF color;
};

class Window : public BasicWindow {
  public:
    Window(int x_size, int y_size, COLORREF color = black_c);
  
    HWND getHwnd() { return hWnd; };

  private:
    HWND hWnd;
};

class Texture : public BasicWindow {
  public:
    Texture();
    Texture(int x_size, int y_size, COLORREF color, int coord_x, int coord_y);
    Texture(int x_size, int y_size, const char* file_name, int coord_x, int coord_y);
    Texture(const char* file_name, int coord_x, int coord_y);

    virtual ~Texture();
    
    int getCoordX() const { return coord.x; };
    int getCoordY() const { return coord.y; };
    Pair<int> getCoord() const { return coord; };
    RGBQUAD* getBuf() const { return screen_buf; };
    
    void setCoordX(int coord_x) { coord.x = coord_x; };
    void setCoordY(int coord_y) { coord.y = coord_y; };
    void setCoord(Pair<int> new_coord) { coord = new_coord; };

    void showOn(const BasicWindow* target) const;
    void showOn(const BasicWindow* target, int coord_x, int coord_y);

  protected:
    Pair<int> coord;
    RGBQUAD* screen_buf;
};

class Renderer {
  public:
    Renderer(BasicWindow* window, double min_x, double min_y, double max_x, double max_y);

    void setWindow(BasicWindow* new_window);
    BasicWindow* getWindow() const { return window; };
    
    COLORREF getPixel(double x, double y);

    void clear() const;
    void drawLine(double x_begin, double y_begin, double x_end, double y_end, COLORREF color = black_c, int thickness = 1) const;
    void setPixel(double x, double y, COLORREF color) const;
    void drawCircle(double x, double y, double r, COLORREF color = black_c,  COLORREF border_color = black_c, int thickness = 1) const;
    void drawRectangle(double x1, double y1, double x2, double y2, COLORREF color = black_c, int thinkness = 1) const;
    void drawFilledRectangle(double x1, double y1, double x2, double y2, COLORREF fill_color, COLORREF color, int thickness = 1) const;
    void drawRoundRect(double x1, double y1, double x2, double y2, double width, double height, COLORREF color = black_c, int thinkness = 1) const;
    void drawText(double x, double y, const char* text, const char* font_name, int size_y, int size_x, COLORREF color = black_c, int thickness = 100);

    int toPixelX(double coord) const;
    int toPixelY(double coord) const;
    
    double toCoordX(int x) const;
    double toCoordY(int y) const;

    double getScaleX() const { return scale.x; };
    double getScaleY() const { return scale.y; };  

    void calcScale();

    int checkX(double x) const;
    int checkY(double y) const;
    
    double getMaxX() const { return max.x; };    
    double getMinX() const { return min.x; };
    double getMaxY() const { return max.y; };    
    double getMinY() const { return min.y; };

    void setMaxX(double x) { max.x = x; };    
    void setMinX(double x) { min.x = x; };
    void setMaxY(double y) { max.y = y; };    
    void setMinY(double y) { min.y = y; };

  private:
    BasicWindow* window;
    Pair<double> scale;
    Pair<double> max;
    Pair<double> min;
};