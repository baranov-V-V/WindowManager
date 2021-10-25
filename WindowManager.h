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
static const char* img_view  = "img\\view.bmp";
static const char* img_view2 = "img\\view2.bmp";

static const char* img_palette   = "img\\palette.bmp";
static const char* img_menu_bar  = "img\\menu_bar.bmp";
static const char* img_canvas    = "img\\canvas.bmp";
static const char* img_back_font = "img\\back_font.bmp";

static const char* img_feather  = "img\\feather.bmp";
static const char* img_eraser   = "img\\eraser.bmp";
static const char* img_feather2 = "img\\feather2.bmp";
static const char* img_eraser2  = "img\\eraser2.bmp";

static const char* img_arrow_up = "img\\feather.bmp";
static const char* img_arrow_down = "img\\eraser.bmp";

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

    bool operator== (const Pair<T>& rhs) {
        return this->x == rhs.x && this->y == rhs.y;
    }

    bool operator!= (const Pair<T>& rhs) {
        return (this->x != rhs.x) || (this->y != rhs.y);
    }

    Pair<int> operator- (const Pair<T>& rhs) {
        Pair<T> result(this->x - rhs.x, this->y - rhs.y);
        return result;
    }

    Pair<int> operator+ (const Pair<T>& rhs) {
        Pair<T> result(this->x + rhs.x, this->y + rhs.y);
        return result;
    }

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

enum GLOW_STATE {
    STATE_DEFAULT = 1,
    STATE_GLOWING  = 2
};

const int comp_x = 26;
const int comp_y = 23;

enum ACTION_TYPE {
    TYPE_NONE = 0,
    TYPE_CLICKABLE = 1,
    TYPE_GLOWABLE = 2,
};

const int MAX_THICKNESS = 20;
const int MIN_THICKNESS = 2;


#define IS_CLICKABLE(type) (type & TYPE_CLICKABLE)
#define IS_GLOWABLE(type) (type & TYPE_GLOWABLE)

//-----------------------------------------TODO--------------------------------------------
/*
   if (pointer_f != nullptr) -- FIXXX BUG

  добавить поле изменений в PicWindow чтобы не перекладываться base_img по кд.
*/

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
    bool isLeftClick() const { return state & LEFT_CLICK; };
    
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
    virtual ~BasicWindow() {};

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
    virtual ~Texture();
    
    int getCoordX() const { return coord.x; };
    int getCoordY() const { return coord.y; };
    Pair<int> getCoord() const { return coord; };
    
    void setCoordX(int coord_x) { coord.x = coord_x; };
    void setCoordY(int coord_y) { coord.y = coord_y; };
    void setCoord(Pair<int> new_coord) { coord = new_coord; };

    void showOn(const BasicWindow* target) const;

  protected:
    Pair<int> coord;
    RGBQUAD* screen_buf;
};

class ManagerWindow : public Texture {
  public:
    ManagerWindow();
    ManagerWindow(int x_size, int y_size, int coord_x, int coord_y, COLORREF color, ManagerWindow* parent = nullptr,
                  VFunctor* press_up_f = nullptr, VFunctor* pointed_f = nullptr, VFunctor* press_down_f = nullptr);
    virtual ~ManagerWindow();

    //general part
    virtual void draw(Renderer* render) const = 0;
    void updateBasicImg(const Texture& new_img) { new_img.showOn(&base_img); };
    Texture& getBasicImg() { return base_img; };

    //!mouse press part
    bool isPointed() const { return is_pointed; };
    bool isClicked() const { return is_clicked; };

    bool checkPointed(WindowMouse* mouse); 
    bool checkLeftClick(WindowMouse* mouse);   //nullptr if it doesn't have click on it, coords of mouse on window if has click on it.
    //bool proceedMouseEv(WindowMouse* mouse);    //go and check all children
    
    bool proceedPointed(WindowMouse* mouse);
    bool proceedPressDown(WindowMouse* mouse);
    bool proceedPressUp(WindowMouse* mouse);
    
    void setPointed(VFunctor* functor) { delete ManagerWindow::pointed_f; ManagerWindow::pointed_f = functor; };
    void setPressUp(VFunctor* functor) { delete ManagerWindow::press_up_f; ManagerWindow::press_up_f = functor; };
    void setPressDown(VFunctor* functor) { delete ManagerWindow::press_down_f; ManagerWindow::press_down_f = functor; };

    //manager part
    ManagerWindow* getParent() const { return parent; };
    void drawChilds(Renderer* render) const;
    void addChild(ManagerWindow* window);
    void makeFirst(ManagerWindow* window);
    ManagerWindow* getChild(int pos) const { return children[pos]; };
    int getCount() const { return count; };
    void delChild(ManagerWindow* window);
    void delLast();

  private:
    void markPointedWindows(WindowMouse* mouse);
    bool proceedPointedWindows(WindowMouse* mouse);

    ManagerWindow* parent;
    ManagerWindow** children;
    int count;
    int max_size;
    VFunctor* pointed_f;
    VFunctor* press_up_f;
    VFunctor* press_down_f;

  protected:
    int window_type;
    bool is_clicked;
    bool is_pointed;
    Texture base_img;
};

class PicWindow : public ManagerWindow {
  public:
    PicWindow();
    PicWindow(int x_size, int y_size, int coord_x, int coord_y, const char* pic_name, ManagerWindow* parent = nullptr,
              VFunctor* press_up_f = nullptr, VFunctor* pointed_f = nullptr, VFunctor* press_down_f = nullptr, bool need_redraw = false);
    PicWindow(int x_size, int y_size, int coord_x, int coord_y, char* pic_name, ManagerWindow* parent = nullptr,
              VFunctor* press_up_f = nullptr, VFunctor* pointed_f = nullptr, VFunctor* press_down_f = nullptr, bool need_redraw = false);
    void draw(Renderer* render) const override;

  private:
    bool need_redraw;
};

class BorderWindow : public ManagerWindow {
  public:
    BorderWindow();
    BorderWindow(int x_size, int y_size, int coord_x, int coord_y, COLORREF color, COLORREF border_color, int thickness, Renderer* render,
                 ManagerWindow* parent = nullptr, VFunctor* press_up_f = nullptr, VFunctor* pointed_f = nullptr, VFunctor* press_down_f = nullptr);
    void draw(Renderer* render) const override;

  protected:
    int thickness;
    COLORREF border_color;
};

class ThicknessWindow : public BorderWindow {
  public:
    ThicknessWindow();
    ThicknessWindow(int x_size, int y_size, int coord_x, int coord_y, COLORREF color, COLORREF border_color, int thickness, Feather* feather, Renderer* render,
                    ManagerWindow* parent = nullptr, VFunctor* press_up_f = nullptr, VFunctor* pointed_f = nullptr, VFunctor* press_down_f = nullptr);

    void draw(Renderer* render) const override;
  
  private:
    Feather* feather;
};

class CanvasWindow : public PicWindow {
  public:
    CanvasWindow();
    CanvasWindow(int x_size, int y_size, int coord_x, int coord_y, char* name, ManagerWindow* parent,
                 Renderer* render, Feather* feather, WindowMouse* mouse);

    void draw(Renderer* render);
    void hide() { on_display = false; };
    void show() { on_display = true;  };

  private:
    char* name;
    bool on_display;
};

/*
class PaletteWindow : public BorderWindow {
  public:
    PaletteWindow();
    PaletteWindow(int x_size, int y_size, int coord_x, int coord_y, COLORREF color, COLORREF border_color, int thickness, Feather* feather,
                    ManagerWindow* parent = nullptr, VFunctor* press_up_f = nullptr, VFunctor* pointed_f = nullptr, VFunctor* press_down_f = nullptr);

  private:

};
*/

/*
class MenuWindow : public BorderWindow {
  public:
    MenuWindow();
    MenuWindow(int x_size, int y_size, COLORREF color, int coord_x, int coord_y, VFunctor* functor,
               ManagerWindow* parent = nullptr, COLORREF border_color = black_c, int thickness = 2);

  private:

};
*/

class ClockWindow : public ManagerWindow {
  public:
    ClockWindow();
    ClockWindow(int x_size, int y_size, int coord_x, int coord_y, COLORREF color, ManagerWindow* parent = nullptr,
                VFunctor* press_up_f = nullptr, VFunctor* pointed_f = nullptr, VFunctor* press_down_f = nullptr);

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
    void stop() { on_run = false; };

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
    bool on_run;
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

class StopAppFunctor : public VFunctor {
  public:
    StopAppFunctor();
    StopAppFunctor(App* app) : app(app) {};
    virtual ~StopAppFunctor() {};

    bool action() override { app->stop(); exit(0); return true; };

  private:
    App* app;
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

class IncThickness : public VFunctor {
  public:
    IncThickness() : feather(nullptr) {};
    IncThickness(Feather* feather) : feather(feather) {};
    virtual ~IncThickness() {};

    bool action() override { 
        if (feather->getThickness() < MAX_THICKNESS) {
            feather->setThickness(feather->getThickness() + 1);  
        }
        return true;
    };

  private:
    Feather* feather;  
};

class DecThickness : public VFunctor {
  public:
    DecThickness() : feather(nullptr) {};
    DecThickness(Feather* feather) : feather(feather) {};
    virtual ~DecThickness() {};

    bool action() override { 
        if (feather->getThickness() > MIN_THICKNESS) {
            feather->setThickness(feather->getThickness() - 1);  
        }
        return true;
    };

  private:
    Feather* feather;  
};

class DrawFunctor : public VFunctor {
  public:
    DrawFunctor() : feather(nullptr), render(nullptr), window(nullptr), mouse(nullptr) {};
    DrawFunctor(ManagerWindow* window, Renderer* render, Feather* feather, WindowMouse* mouse) : feather(feather), render(render), window(window), mouse(mouse) {};
    virtual ~DrawFunctor() {};

    bool action() override {
        //cout << "in draw!" << "\n";
        render->setWindow(window);
        if (mouse->getState() & LEFT_CLICK) {
            //cout << "in draw2!" << "\n";
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
        }
        return true;
    };

  private:
    Feather* feather;
    Renderer* render;
    ManagerWindow* window;
    WindowMouse* mouse;
};

class CloseCanvasFunctor : public VFunctor {
  public:
    CloseCanvasFunctor();
    CloseCanvasFunctor(CanvasWindow* window) : window_to_close(window) {};
    virtual ~CloseCanvasFunctor() {};

    bool action() override {
        ManagerWindow* parent = window_to_close->getParent();
        assert(parent);
        parent->delChild(window_to_close);
        return true;
    };

  private:
    CanvasWindow* window_to_close;
};

class HideCanvasFunctor : public VFunctor {
  public:
    HideCanvasFunctor();
    HideCanvasFunctor(CanvasWindow* window) : window_to_hide(window) {};
    virtual ~HideCanvasFunctor() {};

    bool action() override {
        window_to_hide->hide();
        return true;
    };

  private:
    CanvasWindow* window_to_hide;
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

class ViewFunctor : public VFunctor {
  public:
    ViewFunctor();
    ViewFunctor(ManagerWindow* window) : window(window) {};
    virtual ~ViewFunctor() {};

    bool action() override { /*cout << "Click true on " << window << "\n";*/ return true; };

  private:
    ManagerWindow* window;
};

class  MoveFunctor : public VFunctor {
  public:
    MoveFunctor();
    MoveFunctor(ManagerWindow* window, WindowMouse* mouse) : move_window(window), mouse(mouse), on_move(false), old_coord(0, 0) {};
    virtual ~MoveFunctor() {};

    void startMove() { on_move = true; old_coord = mouse->getAbsCoord(); };
    void endMove() { on_move = false; };

    bool action() override {
        if (on_move) {
            move_window->setCoord(move_window->getCoord() + (mouse->getAbsCoord() - old_coord));
            old_coord = mouse->getAbsCoord();
        }

        return true;
    };

  private:
    ManagerWindow* move_window;
    WindowMouse* mouse;
    Pair<int> old_coord;
    bool on_move;
};

class StartMove : public VFunctor {
  public:
    StartMove();
    StartMove(MoveFunctor* move_f, ManagerWindow* move_window) : move_f(move_f), move_window(move_window) {};
    virtual ~StartMove() {};

    bool action() override { move_window->getParent()->makeFirst(move_window); move_f->startMove(); return true; };

  private:
    MoveFunctor* move_f;
    ManagerWindow* move_window;
};

class EndMove : public VFunctor {
  public:
    EndMove();
    EndMove(MoveFunctor* move_f) : move_f(move_f) {};
    virtual ~EndMove() {};

    bool action() override { move_f->endMove(); return true; };

  private:
    MoveFunctor* move_f;
};

class GlowPicFunctor : public VFunctor {
  public:
    GlowPicFunctor() {};
    GlowPicFunctor(PicWindow* window, const char* glowing_name) : glow_window(window),
        default_wnd(window->getSizeX(), window->getSizeY(), window->getColor(), 0, 0),
        glowing_wnd(window->getSizeX(), window->getSizeY(), glowing_name, 0, 0) {
        window->getBasicImg().showOn(&default_wnd);
        curr_state = STATE_DEFAULT;
    };

    virtual ~GlowPicFunctor() {};

    bool action() override {
        //cout << "In glow!!" << "\n";
        if (glow_window->isPointed() && curr_state != STATE_GLOWING) {
            //cout << "changed to glow!" << "\n";
            glowing_wnd.showOn(glow_window);
            glow_window->updateBasicImg(glowing_wnd);
            curr_state = STATE_GLOWING;
        }
        if (!glow_window->isPointed() && curr_state == STATE_GLOWING) {
            //cout << "changed to default!" << "\n";
            default_wnd.showOn(glow_window);
            glow_window->updateBasicImg(default_wnd);
            curr_state = STATE_DEFAULT;
        }
        return true;
    };

  private:
    PicWindow* glow_window;
    Texture default_wnd;
    Texture glowing_wnd;
    int curr_state;
};

class MakeFirst : public VFunctor {
  public:
    MakeFirst();
    MakeFirst(ManagerWindow* window) : window(window) {};
    virtual ~MakeFirst() {};

    bool action() override { window->getParent()->makeFirst(window); return true; };

  private:
    ManagerWindow* window;
};

RGBQUAD ToRGBQUAD(COLORREF color);

void PrintMousePos();
int NumOfDigits(int num);
void GetMouse(double& x, double& y, const Renderer& render);

RGBQUAD ToRGBQUAD(COLORREF color);
RGBQUAD ToRGBQUAD(BYTE red, BYTE green, BYTE blue);
/*
CanvasWindow* MakeCanvas(int x_size, int y_size, int coord_x, int coord_y, ManagerWindow* parent,
                         Renderer* render, Feather* feather, WindowMouse* mouse);
*/
PicWindow* MakeBasicMenu(int x_size, int y_size, int coord_x, int coord_y, ManagerWindow* parent, int comp_x);
PicWindow* MakePalette(int x_size, int y_size, int coord_x, int coord_y, ManagerWindow* parent, Renderer* render, Feather* feather, WindowMouse* mouse);
PicWindow* MakeLayout(int x_size, int y_size, int coord_x, int coord_y, ManagerWindow* parent, int comp_x);
void MakeMovable(ManagerWindow* activate_wnd, ManagerWindow* move_wnd, WindowMouse* mouse);