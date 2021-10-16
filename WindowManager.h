#include <cmath>
#include <ctime>
#include <iostream>
#include <windows.h>
#include <windef.h>
#include <initializer_list>
#include <string>
#include "Functors.h"

using std::string;

using std::rand;
using std::max;

const double dt = 0.03;

const double vi_x = 5;
const double vi_y = 5;
const double vi_z = 10;

const double li_x = 5;
const double li_y = 5;
const double li_z = 0;

enum OBJECT_TYPES {
    NOT_SPECIFIED = -1,
    TYPE_CIRCLE = 0,
    TYPE_RECT = 1,
    TYPE_SPHERE = 2,
    TYPE_COUNT = 3
};

enum WINDOW_TYPES {
    TYPE_WINDOW = 1,
    TYPE_TEXTURE = 2,
};

class Window;
class Renderer;
class MainWindow;
class Frame;
class BasicWindow;
class Graph;
class Vfunctor;

template <typename T>
class Pair {
  public:
    Pair() = default;
    Pair(T x, T y) : x(x), y(y) {};
    T x;
    T y;
};

enum MOUSE_STATES {
    NONE_CLICK = 0,
    LEFT_CLICK = 1,
    RIGHT_CLICK = 2
};

//-----------------------------------------TODO--------------------------------------------
// 
//


const COLORREF black_c = RGB(  0,   0,   0);
const COLORREF white_c = RGB(255, 255, 255);   

class Mouse {
  public:
    getInfo() const;

  private:
    int state;
    Pair<int> coord;
};

class Feather {
  public:
    Feather(COLORREF color = black_c, int thickness = 2) : color(color), thickness(thickness) {};

    void setColor(COLORREF color) { Feather::color = color; };
    void setThickness(int thickness) { Feather::thickness = thickness; };

    COLORREF getColor() const { return color; };
    int getThickness() const { return thickness; };

    //void setTheme();

  private:
    COLORREF color;
    int thickness;
    //int type;
};

class Renderer {
  public:
    Renderer(BasicWindow* window, double min_x, double min_y, double max_x, double max_y);

    void setWindow(BasicWindow* new_window);
    BasicWindow* getWindow() const { return window; };
    
    void clear() const;
    void drawLine(double x_begin, double y_begin, double x_end, double y_end, COLORREF color = black_c, int thickness = 1) const;
    void setPixel(double x, double y, COLORREF color) const;
    void drawCircle(double x, double y, double r, COLORREF color = black_c, int thickness = 1) const;
    void drawRectangle(double x1, double y1, double x2, double y2, COLORREF color = black_c, int thinkness = 1) const;
    void drawRoundRect(double x1, double y1, double x2, double y2, double width, double height, COLORREF color = black_c, int thinkness = 1) const;

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

/*
class Manager {
  public:
    Manager(int max_count);
    ~Manager();

    void addFigure(PhysShape* figure);
    void delFigure(const PhysShape* figure);
    void delLast();

    PhysShape* get_figure(int pos) const { return figures[pos]; };
    int getCount() const { return count; };

    void drawAll(const Renderer& renderer) const;
    void moveAll(double time) const;

  private:
    int count;
    int size;
    PhysShape** figures;
};
*/

class BasicWindow {
  public:
    BasicWindow();
    BasicWindow(int x_size, int y_size, COLORREF color);
  
    int getSizeX() const { return size.x; };
    int getSizeY() const { return size.y; };

    HDC getHdc() const;
    int getType() const { return type; };
    COLORREF getColor() const;

  protected:
    int type;
    Pair<int> size;
    HDC screen;
    COLORREF color;
};

class Window : public BasicWindow {
  public:
    Window(int x_size, int y_size, COLORREF color = black_c);
};

class Texture : public BasicWindow {
  public:
    Texture(int x_size, int y_size, COLORREF color, int coord_x, int coord_y);
    ~Texture();
    
    int getCoordX() const { return coord.x; };
    int getCoordY() const { return coord.y; };
    
    void showOn(const BasicWindow* target) const;

  protected:
    Pair<int> coord;
    RGBQUAD* screen_buf;
};

class ManagerWindow : public Texture {
  public:
    ManagerWindow();
    ManagerWindow(int x_size, int y_size, COLORREF color, int coord_x, int coord_y, Vfunctor* functor, ManagerWindow* parent = nullptr);
    ~ManagerWindow();

    //general part
    virtual void draw(Renderer* render) const = 0;
    
    //!mouse press part
    Pair<int>* checkPress();   //nullptr if it doesn't have click on it, coords of mouse on window if has click on it.
    ManagerWindow* getPress(); //go from end and check all children
    bool action();
    
    //manager part
    void drawAll(Renderer* render) const;
    void addChild(ManagerWindow* window);
    void addChild(std::initializer_list<ManagerWindow*> windows);
    ManagerWindow* getChild(int pos) const { return children[pos]; };
    int getCount() const { return count; };
    void delChild(ManagerWindow* window);
    void delLast();

  private:
    ManagerWindow* parent;
    ManagerWindow** children;
    int count;
    int max_size;

    VFunctor* functor;
};

class BorderWindow : public ManagerWindow {
  public:
    BorderWindow();
    BorderWindow(int x_size, int y_size, COLORREF color, int coord_x, int coord_y, Vfunctor* functor,
                 ManagerWindow* parent = nullptr, COLORREF border_color = black_c, int thickness = 2);
    void draw(Renderer* render) const override;

  private:
    int thickness;
    COLORREF border_color;
};

class CanvasWindow : public BorderWindow {
  public:
    CanvasWindow();
    CanvasWindow(int x_size, int y_size, COLORREF color, int coord_x, int coord_y, Vfunctor* functor,
                 ManagerWindow* parent = nullptr, COLORREF border_color = black_c, int thickness = 2);

    void draw_feather(Feather* feather, Mouse* mouse);


  private:

};

class PaletteWindow : public BorderWindow {
  public:

  private:

};

class MenuWindow : public BorderWindow {
  public:
    
  private:

};

class TextWindow : public BorderWindow {
  public:
    
  private:

};

class App {
  public:
    App(int app_x, int app_y);
    App(int app_x, int app_y, const string& skin_name);
    
    void run();

  private:
    void initWindows() const;
    void proceedPress() const;
    void sleep(int millisec) const;


    Pair<int> app_size;
    Window users_window;
    BorderWindow app_window;
    Feather feather;
};

RGBQUAD ToRGBQUAD(COLORREF color);


void PrintMousePos();
int NumOfDigits(int num);
void GetMouse(double& x, double& y, const Renderer& render);

RGBQUAD ToRGBQUAD(COLORREF color);
RGBQUAD ToRGBQUAD(BYTE red, BYTE green, BYTE blue);