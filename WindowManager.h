#include <cmath>
#include <ctime>
#include <iostream>
#include <windows.h>
#include <windef.h>
#include <cassert>
#include <initializer_list>
#include <string>
#include <iomanip>
#include <ctime>
#include <sstream>
#include "Functors.h"

//in each window there is texture.
//2 текстуры в кнопки: одна фулл прозарчная другая фулл не прозрачная и менять прозрачность у первой

using std::string;

using std::rand;
using std::max;
using std::cerr;
using std::cout;

//string main_font = "montserrat";

static const char* img_close  = "img\\close.bmp";
static const char* img_close2 = "img\\close2.bmp";
static const char* img_hide   = "img\\hide.bmp";
static const char* img_hide2  = "img\\hide2.bmp";
static const char* img_scale  = "img\\scale.bmp";
static const char* img_scale2 = "img\\scale2.bmp";

static const char* img_file  = "img\\file.bmp";
static const char* img_file2 = "img\\file2.bmp";
static const char* img_help  = "img\\help.bmp";
static const char* img_help2 = "img\\help2.bmp";

static const char* img_palette   = "img\\palette.bmp";
static const char* img_menu_bar  = "img\\menu_bar.bmp";
static const char* img_canvas    = "img\\canvas.bmp";
static const char* img_back_font = "img\\back_font.bmp";

static const char* img_feather = "img\\feather.bmp";
static const char* img_eraser  = "img\\eraser.bmp";

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

enum FEATHER_MODE {
    MODE_ERASE = 1,
    MODE_DRAW   = 2
};

const int comp_x = 26;
const int comp_y = 23;

//-----------------------------------------TODO--------------------------------------------
//  добавить кастомную компрессию на менюшки 
//


const COLORREF black_c     = RGB(  0,   0,   0);
const COLORREF white_c     = RGB(255, 255, 255); 
const COLORREF canvas_c    = RGB( 85,  86, 179);
const COLORREF blue_c      = RGB( 85,  86, 179);
const COLORREF red_c       = RGB(  4,   2, 255);
const COLORREF grey_c      = RGB(153,  76,   0);
const COLORREF green_c     = RGB(  0, 128,   0);
const COLORREF dgrey_c     = RGB( 38,  38,  38);
const COLORREF lgrey_c     = RGB( 77,  77,  77);
const COLORREF llgrey_c    = RGB(137, 137, 137);
const COLORREF silver_c    = RGB(192, 192, 192);
const COLORREF mgrey_c     = RGB( 60,  60,  60);
const COLORREF yellow_c    = RGB(255, 255,   0);
const COLORREF brown_c     = RGB(153,  75,   0);
const COLORREF lavender_c  = RGB(230, 230, 250);
const COLORREF lime_c      = RGB(  0, 255,   0);
const COLORREF cyan_c      = RGB(  0, 255, 255);
const COLORREF navy_c      = RGB(  0,   0, 128);
const COLORREF purple_c    = RGB(128,   0, 128);
const COLORREF magenta_c   = RGB(255,   0, 255);
const COLORREF alice_b_c   = RGB(240, 248, 255);
const COLORREF honeydew_c  = RGB(240, 255, 240);
const COLORREF sand_br_c   = RGB(244, 164,  96);
const COLORREF coral_c     = RGB(255, 127,  80);


const int x_count_c = 5;
const int y_count_c = 4;

static COLORREF palette_colors[y_count_c][x_count_c] = {
    {black_c, mgrey_c, lgrey_c, silver_c, white_c},
    {red_c, lime_c, blue_c, yellow_c, cyan_c},
    {navy_c, magenta_c, purple_c, brown_c, green_c},
    {lavender_c, alice_b_c, honeydew_c, sand_br_c, coral_c}
};


class VFunctor {
  public:
    virtual ~VFunctor() {};
    virtual bool action() = 0;
};

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
    Feather(COLORREF color = black_c, int thickness = 2) : color(color), thickness(thickness), mode(MODE_DRAW) {};

    void setColor(COLORREF color) { Feather::color = color; };
    void setThickness(int thickness) { Feather::thickness = thickness; };
    void setMode(int mode) { Feather::mode = mode; };

    COLORREF getColor() const { return color; };
    int getThickness() const { return thickness; };
    int getMode() const { return mode; };

    //void setTheme();

  private:
    COLORREF color;
    int thickness;
    int mode;
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
    void drawText(double x, double y, const char* text, const char* font_name, int size_y, int size_x, COLORREF color = black_c);

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
    Texture();
    Texture(int x_size, int y_size, COLORREF color, int coord_x, int coord_y);
    Texture(int x_size, int y_size, const char* file_name, int coord_x, int coord_y);
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
    void setFunctor(VFunctor* functor) { delete ManagerWindow::functor; ManagerWindow::functor = functor; };
    
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

class PicWindow : public ManagerWindow {
  public:
    PicWindow();
    PicWindow(int x_size, int y_size, int coord_x, int coord_y, VFunctor* functor, const char* pic_name, ManagerWindow* parent = nullptr);
    PicWindow(int x_size, int y_size, int coord_x, int coord_y, VFunctor* functor, char* pic_name, ManagerWindow* parent = nullptr);
    
    void draw(Renderer* render) const override;

  private:
};

class BorderWindow : public ManagerWindow {
  public:
    BorderWindow();
    BorderWindow(int x_size, int y_size, COLORREF color, int coord_x, int coord_y, VFunctor* functor, Renderer* render,
                 ManagerWindow* parent = nullptr, COLORREF border_color = black_c, int thickness = 2);
    void draw(Renderer* render) const override;

  protected:
    int thickness;
    COLORREF border_color;
};

class ThicknessWindow : public BorderWindow {
  public:
    ThicknessWindow();
    ThicknessWindow(int x_size, int y_size, COLORREF color, int coord_x, int coord_y, VFunctor* functor, Renderer* render, Feather* feather,
                    ManagerWindow* parent = nullptr, COLORREF border_color = black_c, int thickness = 2);

    void draw(Renderer* render) const override;
  
  private:
    Feather* feather;
};

class CanvasWindow : public BorderWindow {
  public:
    CanvasWindow();
    CanvasWindow(int x_size, int y_size, COLORREF color, int coord_x, int coord_y, VFunctor* functor,
                 ManagerWindow* parent = nullptr, COLORREF border_color = black_c, int thickness = 2);

    void draw(Renderer* render);
  
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

class ClockWindow : public ManagerWindow {
  public:
    ClockWindow();
    ClockWindow(int x_size, int y_size, COLORREF color, int coord_x, int coord_y, VFunctor* functor,
                ManagerWindow* parent = nullptr, COLORREF border_color = black_c, int thickness = 2);

    void draw(Renderer* render) const override;

  private:
    void updateTime() {
        std::time_t time = std::time(nullptr);
        curr_time = *std::localtime(&time);
    };
    std::tm curr_time;
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
    PicWindow app_window;
    Feather feather;
    WindowMouse mouse;
    Renderer render;
};

class DummyFunctor : public VFunctor {
  public:
    DummyFunctor();
    virtual ~DummyFunctor() {};
    bool action() override { return true; };
};

class FeatherFunctor : public VFunctor {
    FeatherFunctor(Feather* feather) : feather(feather), new_color(feather->getColor()), new_thickness(feather->getThickness()) {};
    virtual ~FeatherFunctor() {};

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
    virtual ~DebugFunctorTrue() {};

    bool action() override { /*cout << "Click true on " << window << "\n";*/ return true; };

  private:
    ManagerWindow* window;
};

class DebugFunctorFalse : public VFunctor {
  public:
    DebugFunctorFalse();
    DebugFunctorFalse(ManagerWindow* window) : window(window) {};
    virtual ~DebugFunctorFalse() {};
    bool action() override { /*cout << "Click false on " << window << "\n";*/ return false; };

  private:
    ManagerWindow* window;
};

class ChangeColor : public VFunctor {
  public:
    ChangeColor() : feather(nullptr), color(black_c) {};
    ChangeColor(Feather* feather, COLORREF color) : feather(feather), color(color) {};
    virtual ~ChangeColor() {};

    bool action() override { feather->setColor(color); return true; };

  private:
    Feather* feather;
    COLORREF color;  
};

class ChangeMode : public VFunctor {
  public:
    ChangeMode() : feather(nullptr) { mode = MODE_DRAW; };

    ChangeMode(Feather* feather, int mode) : feather(feather), mode(mode) {};
    virtual ~ChangeMode() {};

    bool action() override { feather->setMode(mode); return true; };

  private:
    Feather* feather;
    int mode;
};

class ChangeThickness : public VFunctor {
  public:
    ChangeThickness() : feather(nullptr), thickness(0) {};
    ChangeThickness(Feather* feather, int thickness) : feather(feather), thickness(thickness) {};
    virtual ~ChangeThickness() {};

    bool action() override { feather->setThickness(thickness); return true; };

  private:
    Feather* feather;
    int thickness;  
};

class DrawFunctor : public VFunctor {
  public:
    DrawFunctor() : feather(nullptr), render(nullptr), window(nullptr), mouse(nullptr) {};
    DrawFunctor(ManagerWindow* window, Renderer* render, Feather* feather, WindowMouse* mouse) : feather(feather), render(render), window(window), mouse(mouse) {};
    virtual ~DrawFunctor() {};

    bool action() override {
        render->setWindow(window);
        switch (feather->getMode()) {
            case MODE_ERASE:
                render->drawCircle(mouse->getRelCoord().x, mouse->getRelCoord().y, 3, white_c, 1);        
                break;
            case MODE_DRAW:
                render->drawCircle(mouse->getRelCoord().x, mouse->getRelCoord().y, feather->getThickness() / 2, feather->getColor(), 1);
                break;
            default:
                cerr << "unsupported mod: " << feather->getMode() << " in draw" << "\n";
                break;
        }
        return true;
    };

  private:
    Feather* feather;
    Renderer* render;
    ManagerWindow* window;
    WindowMouse* mouse;
};

class CloseFunctor : public VFunctor {
  public:
    CloseFunctor();
    CloseFunctor(ManagerWindow* window) : window(window) {};
    virtual ~CloseFunctor() {};

    bool action() override { /*cout << "Click true on " << window << "\n";*/ return true; };

  private:
    ManagerWindow* window;
};

class HideFunctor : public VFunctor {
  public:
    HideFunctor();
    HideFunctor(ManagerWindow* window) : window(window) {};
    virtual ~HideFunctor() {};

    bool action() override { /*cout << "Click true on " << window << "\n";*/ return true; };

  private:
    ManagerWindow* window;
};

class FileFunctor : public VFunctor {
  public:
    FileFunctor();
    FileFunctor(ManagerWindow* window) : window(window) {};
    virtual ~FileFunctor() {};

    bool action() override { /*cout << "Click true on " << window << "\n";*/ return true; };

  private:
    ManagerWindow* window;
};

class HelpFunctor : public VFunctor {
  public:
    HelpFunctor();
    HelpFunctor(ManagerWindow* window) : window(window) {};
    virtual ~HelpFunctor() {};

    bool action() override { /*cout << "Click true on " << window << "\n";*/ return true; };

  private:
    ManagerWindow* window;
};

class MoveFunctor : public VFunctor {
  public:
    MoveFunctor();
    MoveFunctor(ManagerWindow* window) : window(window) {};
    virtual ~HelpFunctor() {};

    bool action() override { /*cout << "Click true on " << window << "\n";*/ return true; };

  private:
    ManagerWindow* window;
    WindowMouse* mouse;
};


RGBQUAD ToRGBQUAD(COLORREF color);

void PrintMousePos();
int NumOfDigits(int num);
void GetMouse(double& x, double& y, const Renderer& render);

RGBQUAD ToRGBQUAD(COLORREF color);
RGBQUAD ToRGBQUAD(BYTE red, BYTE green, BYTE blue);

PicWindow* MakeCanvas(int x_size, int y_size, int coord_x, int coord_y, ManagerWindow* parent,
                      Renderer* render, Feather* feather, WindowMouse* mouse);
PicWindow* MakeBasicMenu(int x_size, int y_size, int coord_x, int coord_y, ManagerWindow* parent, int comp_x);
PicWindow* MakePalette(int x_size, int y_size, int coord_x, int coord_y, ManagerWindow* parent, Renderer* render, Feather* feather);
PicWindow* MakeLayout(int x_size, int y_size, int coord_x, int coord_y, ManagerWindow* parent, int comp_x);