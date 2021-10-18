#include <cmath>
#include <ctime>
#include <iostream>
#include <windows.h>
#include <windef.h>
#include <cassert>
#include <initializer_list>
#include <string>
#include "Functors.h"

using std::string;

using std::rand;
using std::max;
using std::cerr;
using std::cout;

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
class ManagerWindow;
class Frame;
class BasicWindow;
class Graph;
class VFunctor;

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


const COLORREF black_c  = RGB(  0,   0,   0);
const COLORREF white_c  = RGB(255, 255, 255); 
const COLORREF canvas_c = RGB( 85,  86, 179);
const COLORREF blue_c   = RGB( 85,  86, 179);
const COLORREF red_c    = RGB(  4,   2, 255);
const COLORREF grey_c   = RGB( 153, 76,   0);
const COLORREF green_c  = RGB(  4, 135,   0);


class WindowMouse {
  public:
    WindowMouse(ManagerWindow* window) : window(window), abs_coord(0, 0), rel_coord(0, 0), state(NONE_CLICK) {};

    void update(); //call only if window is app_window!!
    
    int getState() const;
    void printState() const;
    
    Pair<int> getRelCoord() const { return rel_coord; }; 
    Pair<int> getAbsCoord() const { return abs_coord; };
    
    void setWindow(ManagerWindow* new_window);          // only if new window is child of current window
    void setToParent();

  private:
    int state;
    Pair<int> abs_coord;
    ManagerWindow* window;
    Pair<int> rel_coord;
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
    ManagerWindow(int x_size, int y_size, COLORREF color, int coord_x, int coord_y, VFunctor* functor, ManagerWindow* parent = nullptr);
    virtual ~ManagerWindow();

    //general part
    virtual void draw(Renderer* render) const = 0;
    
    //!mouse press part
    bool checkLeftClick(WindowMouse* mouse);   //nullptr if it doesn't have click on it, coords of mouse on window if has click on it.
    bool proceedClicks(WindowMouse* mouse);    //go from end and check all children
    bool action();
    void setFunctor(VFunctor* functor) { ManagerWindow::functor = functor; };
    
    //manager part
    ManagerWindow* getParent() const { return parent; };
    void drawChilds(Renderer* render) const;
    void addChild(ManagerWindow* window);
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
    BorderWindow(int x_size, int y_size, COLORREF color, int coord_x, int coord_y, VFunctor* functor,
                 ManagerWindow* parent = nullptr, COLORREF border_color = black_c, int thickness = 2);
    void draw(Renderer* render) const override;

  private:
    int thickness;
    COLORREF border_color;
};

class CanvasWindow : public BorderWindow {
  public:
    CanvasWindow();
    CanvasWindow(int x_size, int y_size, COLORREF color, int coord_x, int coord_y, VFunctor* functor,
                 ManagerWindow* parent = nullptr, COLORREF border_color = black_c, int thickness = 2);

    void draw_feather(Feather* feather, WindowMouse* mouse);


  private:

};

class PaletteWindow : public BorderWindow {
  public:
    PaletteWindow();
    PaletteWindow(int x_size, int y_size, COLORREF color, int coord_x, int coord_y, VFunctor* functor,
                  ManagerWindow* parent = nullptr, COLORREF border_color = black_c, int thickness = 2);

  private:

};

class MenuWindow : public BorderWindow {
  public:
    MenuWindow();
    MenuWindow(int x_size, int y_size, COLORREF color, int coord_x, int coord_y, VFunctor* functor,
               ManagerWindow* parent = nullptr, COLORREF border_color = black_c, int thickness = 2);

  private:

};

class ClockWindow : public BorderWindow {
  public:
    ClockWindow();
    ClockWindow(int x_size, int y_size, COLORREF color, int coord_x, int coord_y, VFunctor* functor,
               ManagerWindow* parent = nullptr, COLORREF border_color = black_c, int thickness = 2);

    void draw(Renderer* render) const override;

  private:
    time_t
};

class App {
  public:
    App(int app_x, int app_y);
    App(int app_x, int app_y, const string& skin_name);
    
    void run();

  private:
    void initWindows();
    void proceedMouseEvent();
    void sleep(int millisec) const;

    Pair<int> app_size;
    Window users_window;
    BorderWindow app_window;
    Feather feather;
    WindowMouse mouse;
    Renderer render;
};

class VFunctor {
  public:
    virtual bool action() = 0;
};

class DummyFunctor : public VFunctor {
  public:
    bool action() override { return true; };
};

class FeatherFunctor : public VFunctor {
    FeatherFunctor(Feather* feather) : feather(feather), new_color(feather->getColor()), new_thickness(feather->getThickness()) {};

    bool action() override { feather->setColor(new_color); feather->setThickness(new_thickness); return true; };
    void setColor(COLORREF color) { new_color = color; };
    void setThickness(int thinckness) { new_thickness = thinckness; };

  private:
    Feather* feather;
    COLORREF new_color;
    int new_thickness;  
};

class DebugFunctorTrue : public VFunctor {
  public:
    DebugFunctorTrue();
    DebugFunctorTrue(ManagerWindow* window) : window(window) {};
    bool action() override { cout << "Click true on " << window << "\n"; return true; };

  private:
    ManagerWindow* window;
};

class DebugFunctorFalse : public VFunctor {
  public:
    DebugFunctorFalse();
    DebugFunctorFalse(ManagerWindow* window) : window(window) {};
    bool action() override { cout << "Click false on " << window << "\n"; return false; };

  private:
    ManagerWindow* window;
};

class ChangeColor : public VFunctor {
  public:
    ChangeColor() : feather(nullptr), color(black_c) {};
    ChangeColor(Feather* feather, COLORREF color) : feather(feather), color(color) {};

    bool action() override { feather->setColor(color); return true; };

  private:
    Feather* feather;
    COLORREF color;  
};

class ChangeThickness : public VFunctor {
  public:
    ChangeThickness() : feather(nullptr), thickness(0) {};
    ChangeThickness(Feather* feather, int thickness) : feather(feather), thickness(thickness) {};

    bool action() override { feather->setThickness(thickness); return true; };

  private:
    Feather* feather;
    int thickness;  
};

class DrawFunctor : public VFunctor {
  public:
    DrawFunctor() : feather(nullptr), render(nullptr), window(nullptr), mouse(nullptr) {};
    DrawFunctor(ManagerWindow* window, Renderer* render, Feather* feather, WindowMouse* mouse) : feather(feather), render(render), window(window), mouse(mouse) {};

    bool action() override {
        render->setWindow(window);
        //render->drawCircle(mouse->getRelCoord().x, mouse->getRelCoord().y, feather->getThickness(), feather->getColor(), 1);
        render->setPixel(mouse->getRelCoord().x, mouse->getRelCoord().y, feather->getColor());
        return true;
    };

  private:
    Feather* feather;
    Renderer* render;
    ManagerWindow* window;
    WindowMouse* mouse;
};

RGBQUAD ToRGBQUAD(COLORREF color);

void PrintMousePos();
int NumOfDigits(int num);
void GetMouse(double& x, double& y, const Renderer& render);

RGBQUAD ToRGBQUAD(COLORREF color);
RGBQUAD ToRGBQUAD(BYTE red, BYTE green, BYTE blue);