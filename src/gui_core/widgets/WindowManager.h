#include <cmath>
#include <ctime>
#include <iostream>
#include <algorithm>
#include <windows.h>
#include <windef.h>
#include <cassert>
#include <initializer_list>
#include <string>
#include <iomanip>
#include <ctime>
#include <sstream>
#include <queue>
#include "Functors.h"

//in each window there is texture.

using std::string;

using std::rand;
using std::max;
using std::cerr;
using std::cout;
using std::queue;

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
static const char* img_canvas    = "img\\canvas2.bmp";
static const char* img_back_font = "img\\back_font.bmp";

static const char* img_feather  = "img\\feather.bmp";
static const char* img_eraser   = "img\\eraser.bmp";
static const char* img_feather2 = "img\\feather2.bmp";
static const char* img_eraser2  = "img\\eraser2.bmp";

static const char* img_arrow_up    = "img\\arrow_up.bmp";
static const char* img_arrow_down  = "img\\arrow_down.bmp";
static const char* img_arrow_left  = "img\\arrow_left.bmp";
static const char* img_arrow_right = "img\\arrow_right.bmp";

static const char* img_arrow_up2    = "img\\arrow_up2.bmp";
static const char* img_arrow_down2  = "img\\arrow_down2.bmp";
static const char* img_arrow_left2  = "img\\arrow_left2.bmp";
static const char* img_arrow_right2 = "img\\arrow_right2.bmp";

enum WINDOW_TYPES {
    TYPE_WINDOW = 1,
    TYPE_TEXTURE = 2,
};

enum RESIZE_DIRECTIONS {
    DIRECTION_NONE = 0,
    DIRECTION_LEFT = 1,
    DIRECTION_RIGHT = 2,
    DIRECTION_UP = 4,
    DIRECTION_DOWN = 8,
};

#define CLEAR_DIR(direction)     (direction = direction & DIRECTION_NONE)

#define SET_DIR_LEFT(direction)  (direction = direction | DIRECTION_LEFT)
#define SET_DIR_RIGHT(direction) (direction = direction | DIRECTION_RIGHT)
#define SET_DIR_UP(direction)    (direction = direction | DIRECTION_UP)
#define SET_DIR_DOWN(direction)  (direction = direction | DIRECTION_DOWN)

#define IS_DIR_LEFT(direction)  (direction & DIRECTION_LEFT)
#define IS_DIR_RIGHT(direction) (direction & DIRECTION_RIGHT)
#define IS_DIR_UP(direction)    (direction & DIRECTION_UP)
#define IS_DIR_DOWN(direction)  (direction & DIRECTION_DOWN)

const int grab_len = 5; //in pixels

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

enum TEXT_BUTTON_ALIGN {
    ALIGN_LEFT = 0,
    ALIGN_MID = 1,
    ALIGN_RIGHT = 2,
};

enum BASIC_TOOLS {
    TOOL_FEATHER = 0,
    TOOL_ERASER = 1
};

const int MAX_THICKNESS = 20;
const int MIN_THICKNESS = 2;

const int close_button_x = 30;
const int menu_size_y = 25;

#define IS_CLICKABLE(type) (type & TYPE_CLICKABLE)
#define IS_GLOWABLE(type)  (type & TYPE_GLOWABLE)

//-----------------------------------------TODO--------------------------------------------
/*
   if (pointer_f != nullptr) -- FIXXX BUG
  ???????? ???? ????????? ? PicWindow ????? ?? ??????????????? base_img ?? ??.
*/

const COLORREF palette_c   = RGB( 37,  37,  38);
const COLORREF black_c     = RGB(  0,   0,   0);
const COLORREF white_c     = RGB(255, 255, 255); 
const COLORREF canvas_c    = RGB( 85,  86, 179);
const COLORREF blue_c      = RGB( 85,  86, 179);
const COLORREF red_c       = RGB(  4,   2, 255);
const COLORREF grey_c      = RGB(153,  76,   0);
const COLORREF green_c     = RGB(  0, 128,   0);
const COLORREF dgrey_c     = RGB( 38,  38,  38);
const COLORREF lgrey_c     = RGB( 83,  83,  83);
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

class WindowMouse;
class Feather;
class Renderer;
class DisplayManager;
class App;

class BasicWindow;
class Window;
class Texture;

class ManagerWindow;
class InvisibleWindow;
class PicWindow;
class BorderWindow;
class ThicknessWindow;
class TextButtonWindow;
class CanvasWindow;
class ClockWindow;

class DummyFunctor;
class FeatherFunctor;
class DebugFunctorTrue;
class InvFunctorTrue;
class StopAppFunctor;
class DebugFunctorFalse;
class ChangeColor;
class ChangeMode;
class IncThickness;
class DecThickness;
class DrawFunctor;
class CloseCanvasFunctor;
class HideCanvasFunctor;
class FileFunctor;
class HelpFunctor;
class ViewFunctor;
class MoveFunctor;
class StartMove;
class EndMove;
class RecalcThickness;
class PlaceBar;
class MoveBarLeft;
class MoveBarRight;
class MoveBarUp;
class MoveBarDown;
class PlaceBarOnClickX;
class PlaceBarOnClickY;
class MoveBarRandomX;
class MoveBarRandomY;
class GlowPicFunctor;
class GlowBorderFunctor;
class MakeFirst;

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
PicWindow* MakeBasicMenu(int x_size, int y_size, int coord_x, int coord_y, ManagerWindow* parent, int but_x = 30);
PicWindow* MakePalette(int x_size, int y_size, int coord_x, int coord_y, ManagerWindow* parent, Renderer* render, Feather* feather, WindowMouse* mouse, App* app);
PicWindow* MakeLayout(int x_size, int y_size, int coord_x, int coord_y, ManagerWindow* parent, int comp_x, Renderer* render, Feather* feather, WindowMouse* mouse, App* app);
void MakeMovable(ManagerWindow* activate_wnd, ManagerWindow* move_wnd, WindowMouse* mouse, App* app);
void WriteCanvasName(CanvasWindow* canvas_layer, Renderer* render);

InvisibleWindow* MakeResizeCanvas(int size_x, int size_y, int coord_x, int coord_y, char* name, ManagerWindow* parent, Renderer* render, Feather* feather, WindowMouse* mouse, App* app);
InvisibleWindow* MakeStaticCanvas(int size_x, int size_y, int coord_x, int coord_y, char* name, ManagerWindow* parent, Renderer* render, Feather* feather, WindowMouse* mouse, App* app);
void ReplaceFunctors(ManagerWindow* lhs, ManagerWindow* rhs);
void ResizeCanvasWindow(CanvasWindow* canvas_layer, Renderer* render, Pair<int> new_size, Pair<int> new_coord);
BorderWindow* MakeGraphWindow(int size_x, int size_y, int coord_x, int coord_y, ManagerWindow* parent, Renderer* render, WindowMouse* mouse, App* app);

class Renderer {
  public:
    Renderer(BasicWindow* window, double min_x, double min_y, double max_x, double max_y);

    void setWindow(BasicWindow* new_window);
    BasicWindow* getWindow() const { return window; };
    
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
    void setRelCoord(Pair<int> coord) { rel_coord = coord; };
    bool isLeftClick() const { return state & LEFT_CLICK; };
    
    void printPos() { /*POINT pt; GetCursorPos(&pt); std::cout << "x: " << pt.x << " y : " << pt.y << "\n";*/ };

    void setWindow(ManagerWindow* new_window);          // only if new window is child of current window
    void setToParent();

  private:
    int state;
    Pair<int> abs_coord;
    ManagerWindow* window;
    Pair<int> rel_coord;
};

class VTool {
  public:
    VTool() : color(black_c) {};
    VTool(COLORREF color) : color(color) {};
    virtual ~VTool() {};
    
    virtual void ProceedPressUp(Texture* target, Renderer* render, int x, int y) {};
    virtual void ProceedPressDown(Texture* target, Renderer* render, int x, int y) = 0;
    virtual void ProceedMove(Texture* target, Renderer* render, int dx, int dy) {};

    void setColor(COLORREF color) { VTool::color = color; };
    COLORREF getColor() const { return color;}

  private:
  
  protected:
    COLORREF color;
};

class ToolManager {
  public:
    ToolManager();
    ~ToolManager();

    size_t getCount() const { return tools.size(); };
    VTool* getCurrTool() const { return tools[curr_tool]; };
    void setCurrTool(int tool_index) { curr_tool = tool_index; };

    void addTool(VTool* tool);
    void delTool(VTool* tool);
    void delLast();

    VTool* operator[] (int index) { return tools[index]; };
    const VTool* operator[] (int index) const { return tools[index]; };
  
    void setNext() {
        if (curr_tool != tools.size() - 1) {
            curr_tool++;
        } else {
            curr_tool = 0;
        }
    };
    void setPrev() {
        if (curr_tool != 0) {
            curr_tool--;
        } else {
            curr_tool = tools.size() - 1;
        }
    };

  private:
    std::vector<VTool*> tools;

    size_t curr_tool;
};

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

class Feather : public VTool {
  public:
    Feather(COLORREF color = black_c, int thickness = 2) : VTool(color), thickness(thickness) {};
    virtual ~Feather() {};

    virtual void ProceedPressDown(Texture* target, Renderer* render, int x, int y) override {
        old_coord.x = x;
        old_coord.y = y;
    };
    
    virtual void ProceedMove(Texture* target, Renderer* render, int dx, int dy) {
        render->setWindow(reinterpret_cast<BasicWindow*>(target));
        if (old_coord.x + dx > target->getSizeX() || old_coord.y + dy > target->getSizeY()) {
            old_coord.x += dx;
            old_coord.y += dy;
            return;
        }

        render->drawLine(old_coord.x, old_coord.y, old_coord.x + dx, old_coord.y + dy, color, thickness);
        
        old_coord.x += dx;
        old_coord.y += dy;
    };
    
    void setThickness(int thickness) { Feather::thickness = thickness; };
    int getThickness() const { return thickness; };

    //void setTheme();
  private:
    Pair<int> old_coord;
    int thickness;
};

class Eraser : public VTool {
  public:
    Eraser() : VTool(white_c), thickness(5) {};
    virtual ~Eraser() {};

    virtual void ProceedPressDown(Texture* target, Renderer* render, int x, int y) override {
        old_coord.x = x;
        old_coord.y = y;
    };
    
    virtual void ProceedMove(Texture* target, Renderer* render, int dx, int dy) {
        render->setWindow(reinterpret_cast<BasicWindow*>(target));
        if (old_coord.x + dx > target->getSizeX() || old_coord.y + dy > target->getSizeY()) {
            old_coord.x += dx;
            old_coord.y += dy;
            return;
        }

        render->drawLine(old_coord.x, old_coord.y, old_coord.x + dx, old_coord.y + dy, white_c, thickness);
        
        old_coord.x += dx;
        old_coord.y += dy;
    };
    
    int getThickness()  const { return thickness; };

  private:
    Pair<int> old_coord;
    int thickness;
};

class RectTool : public VTool {
  public:
    RectTool() : VTool(black_c) {};
    virtual ~RectTool() {};

    virtual void ProceedPressDown(Texture* target, Renderer* render, int x, int y) override {
        old_coord.x = x;
        old_coord.y = y;
        new_coord = old_coord;
    };
    
    virtual void ProceedMove(Texture* target, Renderer* render, int dx, int dy) { 
        new_coord.x += dx;
        new_coord.y += dy;
    };

    virtual void ProceedPressUp(Texture* target, Renderer* render, int x, int y) {
        if (new_coord.x > 0 && new_coord.x < target->getSizeX() && new_coord.y > 0 && new_coord.y < target->getSizeY()) {
            render->setWindow(reinterpret_cast<BasicWindow*>(target));
            render->drawFilledRectangle(old_coord.x, old_coord.y, new_coord.x, new_coord.y, color, color, 1);
        }
    };

  private:
    Pair<int> old_coord;
    Pair<int> new_coord;
};

class ManagerWindow : public Texture {
  public:
    ManagerWindow();
    ManagerWindow(int x_size, int y_size, int coord_x, int coord_y, COLORREF color, ManagerWindow* parent = nullptr,
                  VFunctor* press_up_f = nullptr, VFunctor* pointed_f = nullptr, VFunctor* press_down_f = nullptr);
    virtual ~ManagerWindow();

    //general part
    virtual void draw(Renderer* render) const = 0;
    void showOnTexture(const Texture* target) const;
    void setRedraw(bool state) { need_redraw = state; };
    bool isRedraw() const { return need_redraw; };

    //!mouse press part
    bool isPointed() const { return is_pointed; };
    bool isClicked() const { return is_clicked; };

    virtual bool hitTest(double x, double y) const;
    bool checkPointed(WindowMouse* mouse); 
    bool checkLeftClick(WindowMouse* mouse);   //nullptr if it doesn't have click on it, coords of mouse on window if has click on it.
    
    bool proceedPointed(WindowMouse* mouse);
    bool proceedPressDown(WindowMouse* mouse);
    bool proceedPressUp(WindowMouse* mouse);
    
    void setPointed(VFunctor* functor);
    void setPressUp(VFunctor* functor);
    void setPressDown(VFunctor* functor);

    VFunctor* getPointed();
    VFunctor* getPressUp();
    VFunctor* getPressDown();

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

  protected:
    bool need_redraw = false;
    int window_type;
    bool is_clicked;
    bool is_pointed;
};

class InvisibleWindow : public ManagerWindow {
  public:
    InvisibleWindow();
    InvisibleWindow(int x_size, int y_size, int coord_x, int coord_y, ManagerWindow* parent = nullptr,
                    VFunctor* press_up_f = nullptr, VFunctor* pointed_f = nullptr, VFunctor* press_down_f = nullptr, bool need_redraw = false);
    
    void draw(Renderer* render) const override;

  private:
};

class PicWindow : public ManagerWindow {
  public:
    PicWindow();
    PicWindow(int x_size, int y_size, int coord_x, int coord_y, const char* pic_name, ManagerWindow* parent = nullptr,
              VFunctor* press_up_f = nullptr, VFunctor* pointed_f = nullptr, VFunctor* press_down_f = nullptr, bool need_redraw = false);
    PicWindow(int x_size, int y_size, int coord_x, int coord_y, char* pic_name, ManagerWindow* parent = nullptr,
              VFunctor* press_up_f = nullptr, VFunctor* pointed_f = nullptr, VFunctor* press_down_f = nullptr, bool need_redraw = false);
    
    void draw(Renderer* render) const override;
    Texture* getBaseImg() { return &base_img; };

  private:
    Texture base_img;
};

class BorderWindow : public ManagerWindow {
  public:
    BorderWindow();
    BorderWindow(int x_size, int y_size, int coord_x, int coord_y, COLORREF color, COLORREF border_color, int thickness, Renderer* render,
                 ManagerWindow* parent = nullptr, VFunctor* press_up_f = nullptr, VFunctor* pointed_f = nullptr, VFunctor* press_down_f = nullptr);
    void draw(Renderer* render) const override;

    COLORREF getBorderColor() const { return border_color; };
    void setBorderColor(COLORREF new_color) { border_color = new_color; };

  protected:
    int thickness;
    COLORREF border_color;
};

class ThicknessWindow : public BorderWindow {
  public:
    ThicknessWindow();
    ThicknessWindow(int x_size, int y_size, int coord_x, int coord_y, COLORREF color, COLORREF border_color, int thickness, ToolManager* tools, Renderer* render,
                    ManagerWindow* parent = nullptr, VFunctor* press_up_f = nullptr, VFunctor* pointed_f = nullptr, VFunctor* press_down_f = nullptr);

    void draw(Renderer* render) const override;
  
  private:
    ToolManager* tools;
};

class TextButtonWindow : public BorderWindow {
  public:
    TextButtonWindow();
    TextButtonWindow(int x_size, int y_size, int coord_x, int coord_y, COLORREF color, COLORREF border_color, int thickness,
                     COLORREF text_color, const char* text, const char* font_name, int ch_size_x, int ch_size_y, int align, Renderer* render,
                     ManagerWindow* parent = nullptr, VFunctor* press_up_f = nullptr, VFunctor* pointed_f = nullptr, VFunctor* press_down_f = nullptr);

    void draw(Renderer* render) const override;
  
  private:
    COLORREF text_color;
    const char* text;
    const char* font_name;
    int ch_size_x;
    int ch_size_y;
    int align;
};

class CanvasWindow : public InvisibleWindow {
  public:
    CanvasWindow();
    CanvasWindow(int x_size, int y_size, int coord_x, int coord_y, char* name, ManagerWindow* parent,
                 Renderer* render, Feather* feather, WindowMouse* mouse, App* app, const char* pic_name = nullptr);
    //CanvasWindow(int x_size, int y_size, int coord_x, int coord_y, CanvasWindow* window, Renderer* render, Feather* feather, WindowMouse* mouse);

    void hide() { on_display = false; };
    void show() { on_display = true;  };
    /*void clear(Renderer* render) {
        render->setWindow(this);
        render->drawRectangle(0, 0, size.x, size.y, border_color, thickness);
        base_img.showOn(this);
    }*/
    const Texture& getBaseImg() { return base_img; };

    char* getName() const { return name; };

  private:
    char* name;
    bool on_display;
    Texture base_img;
};

class GraphWindow : public BorderWindow {
  public:
    GraphWindow();
    GraphWindow(int x_size, int y_size, int coord_x, int coord_y, ManagerWindow* parent, Renderer* render,
                COLORREF backgroud_c = white_c, COLORREF border_c = dgrey_c, COLORREF line_c = black_c, COLORREF net_c = lgrey_c);

    void draw(Renderer* render) const override;

    void setLeftDot (int new_coord) { left_dot_pos  = new_coord; };
    void setRightDot(int new_coord) { right_dot_pos = new_coord; };

    int getLeftDot  () const { return left_dot_pos ; };
    int getRightDot () const { return right_dot_pos; };

  private:
    int left_dot_pos;
    int right_dot_pos;
    COLORREF line_color;
    COLORREF net_color;
};

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

class RoundWindow : public ManagerWindow {
  public:
    RoundWindow();
    RoundWindow(int radius, int coord_x, int coord_y, COLORREF color, COLORREF border_color, int thickness, Renderer* render,
                ManagerWindow* parent = nullptr, VFunctor* press_up_f = nullptr, VFunctor* pointed_f = nullptr, VFunctor* press_down_f = nullptr);

    void draw(Renderer* render) const override;
    bool hitTest(double x, double y) const override;

  private:
    int radius;
    int thickness;
    COLORREF border_color;
};

class DedWindow : public ManagerWindow {
  public:
    DedWindow();
    DedWindow(int radius, int size_x, int size_y, int coord_x, int coord_y, COLORREF color, COLORREF border_color, int thickness, Renderer* render,
              ManagerWindow* parent = nullptr, VFunctor* press_up_f = nullptr, VFunctor* pointed_f = nullptr, VFunctor* press_down_f = nullptr);

    void draw(Renderer* render) const override;
    bool hitTest(double x, double y) const override;

  private:
    int radius;
    int thickness;
    COLORREF border_color;
};

class DisplayManager {
  public:
    DisplayManager(int max_count = 100);
    ~DisplayManager();

    void addWindow(ManagerWindow* window);
    void showWindow(ManagerWindow* window);
    void hideWindow(ManagerWindow* window);
    int getCount() const { return count; };
    void delWindow(ManagerWindow* window);
    void delLast();

  private:
    ManagerWindow** windows;
    int count;
    int size;
};

class App {
  public:
    App(int app_x, int app_y);
    App(int app_x, int app_y, const string& skin_name);
    
    void run();
    void stop() { on_run = false; };

    void setActiveWindow(ManagerWindow* window) { active_window = window; };
    ManagerWindow* getActiveWindow(ManagerWindow* window) const { return active_window; };

    ToolManager* getToolManager() { return &tool_manager; };

  public:
    void initWindows();
    void initBasicTools();
    void proceedMouseEvent();
    void sleep(int millisec) const;

    Pair<int> app_size;
    Window users_window;
    PicWindow app_window;
    Feather feather;
    WindowMouse mouse;
    Renderer render;
    bool on_run;

    std::queue<VFunctor*> events_queue;
    
    ToolManager tool_manager;
    ManagerWindow* active_window = nullptr;
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

class InvFunctorTrue : public VFunctor {
  public:
    InvFunctorTrue() {};
    virtual ~InvFunctorTrue() {};

    bool action() override { cout << "Click true on invs_wnd\n"; return true; };

  private:
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
    ChangeColor() : tools(nullptr), color(black_c) {};
    ChangeColor(ToolManager* tools, COLORREF color) : tools(tools), color(color) {};
    virtual ~ChangeColor() {};

    bool action() override { 
        for (int i = 0; i < tools->getCount(); ++i) {
            tools->operator[](i)->setColor(color);
        }
      return true;
    };

  private:
    ToolManager* tools;
    COLORREF color;  
};

class ChangeToolTrue : public VFunctor {
  public:
    ChangeToolTrue() : tools(nullptr) { tool_index = 0; };
    ChangeToolTrue(ToolManager* tools, int tool_index) : tools(tools), tool_index(tool_index) {};
    virtual ~ChangeToolTrue() {};

    bool action() override { tools->setCurrTool(tool_index); return true; };

  private:
    ToolManager* tools;
    int tool_index;
};

class NextBasicTool : public VFunctor {
  public:
    NextBasicTool() : tools(nullptr) {};
    NextBasicTool(ToolManager* tools) : tools(tools) {};
    virtual ~NextBasicTool() {};

    bool action() override { tools->setNext(); return true; };

  private:
    ToolManager* tools;
};

class IncThickness : public VFunctor {
  public:
    IncThickness() : tools(nullptr) {};
    IncThickness(ToolManager* tools) : tools(tools) {};
    virtual ~IncThickness() {};

    bool action() override { 
        Feather* feather = reinterpret_cast<Feather*>(tools->operator[](TOOL_FEATHER));
        if (feather->getThickness() < MAX_THICKNESS) {
            feather->setThickness(feather->getThickness() + 1);  
        }
        return true;
    };

  private:
    ToolManager* tools;  
};

class DecThickness : public VFunctor {
  public:
    DecThickness() : tools(nullptr) {};
    DecThickness(ToolManager* tools) : tools(tools) {};
    virtual ~DecThickness() {};

    bool action() override {
        Feather* feather = reinterpret_cast<Feather*>(tools->operator[](TOOL_FEATHER));
        if (feather->getThickness() > MIN_THICKNESS) {
            feather->setThickness(feather->getThickness() - 1);  
        }
        return true;
    };

  private:
    ToolManager* tools;  
};

class DrawFunctor : public VFunctor {
  public:
    DrawFunctor() : render(nullptr), canvas(nullptr), mouse(nullptr), abs_old_coord(0, 0) {};
    DrawFunctor(ManagerWindow* window, Renderer* render, WindowMouse* mouse, App* app, ToolManager* tool_manager) :
                render(render), canvas(window), mouse(mouse), app(app), tool_manager(tool_manager) {};
    virtual ~DrawFunctor() {};

    void startDraw() {
        is_drawing = true;
        curr_tool = tool_manager->getCurrTool();
        canvas->getParent()->getParent()->makeFirst(canvas->getParent());
        curr_tool->ProceedPressDown(canvas, render, mouse->getRelCoord().x, mouse->getRelCoord().y);
        abs_old_coord = mouse->getAbsCoord();
        app->setActiveWindow(canvas);
    }
    void endDraw() {
        is_drawing = false;
        curr_tool->ProceedPressUp(canvas, render, mouse->getAbsCoord().x, mouse->getAbsCoord().y);
        app->setActiveWindow(nullptr);
    }

    bool action() override {
        //cout << "in draw!" << "\n";
        if (is_drawing) {
            curr_tool->ProceedMove(canvas, render, mouse->getAbsCoord().x - abs_old_coord.x, mouse->getAbsCoord().y - abs_old_coord.y); //dx, dy coordinates
            abs_old_coord = mouse->getAbsCoord();
        }
        return true;
    };

  private:
    bool is_drawing;
    Renderer* render;
    
    ManagerWindow* canvas;
    
    WindowMouse* mouse;
    
    ToolManager* tool_manager;
    VTool* curr_tool;
    
    Pair<int> abs_old_coord;
    
    App* app;
};

class StartDraw : public VFunctor {
  public:
    StartDraw();
    StartDraw(DrawFunctor* draw_f) : draw_f(draw_f) {};
    virtual ~StartDraw() {};

    bool action() override { draw_f->startDraw(); return true; };

  protected:
    DrawFunctor* draw_f;
};

class EndDraw : public VFunctor {
  public:
    EndDraw();
    EndDraw(DrawFunctor* draw_f) : draw_f(draw_f) {};
    virtual ~EndDraw() {};

    bool action() override { draw_f->endDraw(); return true; };

  private:
    DrawFunctor* draw_f;
};

class CloseCanvasFunctor : public VFunctor {
  public:
    CloseCanvasFunctor();
    CloseCanvasFunctor(InvisibleWindow* window) : window_to_close(window) {};
    virtual ~CloseCanvasFunctor() {};

    bool action() override {
        ManagerWindow* parent = window_to_close->getParent();
        assert(parent);
        parent->delChild(window_to_close);
        return true;
    };

  private:
    InvisibleWindow* window_to_close;
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
    FileFunctor(ManagerWindow* window, Renderer* render, Feather* feather, WindowMouse* mouse, App* app) : window(window), render(render), mouse(mouse), feather(feather), app(app) {};
    virtual ~FileFunctor() {};

    bool action() override {
        InvisibleWindow* canvas_layer = MakeResizeCanvas(window->getSizeX() / 2, window->getSizeY() / 2, window->getSizeX() / 4, window->getSizeY() / 4, "Canvas", window, render, feather, mouse, app);
        window->addChild(canvas_layer);
        return true;
    };

  private:
    ManagerWindow* window;
    Renderer* render;
    Feather* feather;
    WindowMouse* mouse;
    App* app;
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

class MoveFunctor : public VFunctor {
  public:
    MoveFunctor();
    MoveFunctor(ManagerWindow* window, WindowMouse* mouse, App* app, ManagerWindow* activate_window = nullptr) : 
        activate_window(activate_window), move_window(window), mouse(mouse), on_move(false), old_coord(0, 0), app(app) {};
    virtual ~MoveFunctor() {};

    virtual void startMove() { 
        move_window->getParent()->makeFirst(move_window); 
        on_move = true;
        old_coord = mouse->getAbsCoord();
        if (activate_window) {
            app->setActiveWindow(activate_window);  
        } else {
            app->setActiveWindow(move_window);
        }
        //cout << "started move!";
    };

    virtual void endMove() {
        on_move = false;
        app->setActiveWindow(nullptr);
        //cout << "ended move!";
    };

    bool action() override {
        //cout << "try movimg!";
        if (on_move) {
            //cout << "in move!";
            move_window->setCoord(move_window->getCoord() + (mouse->getAbsCoord() - old_coord));
            old_coord = mouse->getAbsCoord();
        }
        return true;
    };

  protected:
    ManagerWindow* activate_window;
    ManagerWindow* move_window;
    WindowMouse* mouse;
    Pair<int> old_coord;
    bool on_move;
    App* app;
};

class StartMove : public VFunctor {
  public:
    StartMove();
    StartMove(MoveFunctor* move_f) : move_f(move_f) {};
    virtual ~StartMove() {};

    bool action() override { move_f->startMove(); return true; };

  protected:
    MoveFunctor* move_f;
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

class RecalcThickness : public VFunctor {
  public:
    RecalcThickness(int min_coord, int max_coord, ManagerWindow* bar, ToolManager* tools) : bar(bar), tools(tools), min_coord(min_coord), max_coord(max_coord) {};
    virtual ~RecalcThickness() {};

    bool action () override {
        Feather* feather = reinterpret_cast<Feather*>(tools->operator[](TOOL_FEATHER));
        feather->setThickness(MIN_THICKNESS + (bar->getCoordX() - min_coord) * (MAX_THICKNESS - MIN_THICKNESS) / (max_coord - min_coord));    
        return true;
    }; 

  private:
    ManagerWindow* bar;
    ToolManager* tools;
    int min_coord;
    int max_coord;
};

class PlaceBar {
  public:
    PlaceBar() {};
    PlaceBar(int min_coord, int max_coord, char type, ManagerWindow* bar, VFunctor* action) : bar(bar), min_coord(min_coord), max_coord(max_coord), action(action), type(type), is_fixed_pos(false) {};
    ~PlaceBar() {};

    void fixPos() { is_fixed_pos = true; };
    void unfixPos() { is_fixed_pos = false; };

    void place(int new_coord) {
        if (!is_fixed_pos) {
            if (new_coord < min_coord) {
                new_coord = min_coord;
            }
            if (new_coord > max_coord) {
                new_coord = max_coord;
            }
            if (type == 'X') {
                bar->setCoordX(new_coord);
            } else if (type == 'Y') {
                bar->setCoordY(new_coord);
            }
            action->action();
        }
    }

    ManagerWindow* getBar() const { return bar; };

  private: 
    VFunctor* action;
    bool is_fixed_pos;
    char type;
    int min_coord;
    int max_coord;
    ManagerWindow* bar;
};

class MoveBarLeft : public VFunctor {
  public:
    MoveBarLeft(PlaceBar* placer) : placer(placer) {};
    virtual ~MoveBarLeft() {};

    bool action() override {
        ManagerWindow* bar = placer->getBar();
        placer->place(bar->getCoordX() - 5);
        return true;
    };

  private:
    PlaceBar* placer;  
};

class MoveBarRight : public VFunctor {
  public:
    MoveBarRight(PlaceBar* placer) : placer(placer) {};
    virtual ~MoveBarRight() {};

    bool action() override {
        ManagerWindow* bar = placer->getBar();
        placer->place(bar->getCoordX() + 5);
        return true;
    };

  private:
    PlaceBar* placer;  
};

class MoveBarUp : public VFunctor {
  public:
    MoveBarUp(PlaceBar* placer) : placer(placer) {};
    virtual ~MoveBarUp() {};

    bool action() override {
        ManagerWindow* bar = placer->getBar();
        placer->place(bar->getCoordY() - 5);
        return true;
    };

  private:
    PlaceBar* placer;  
};

class MoveBarDown : public VFunctor {
  public:
    MoveBarDown(PlaceBar* placer) : placer(placer) {};
    virtual ~MoveBarDown() {};

    bool action() override {
        ManagerWindow* bar = placer->getBar();
        placer->place(bar->getCoordY() + 5);
        return true;
    };

  private:
    PlaceBar* placer;  
};

class PlaceBarOnClickX : public StartMove {
  public:
    PlaceBarOnClickX() {};
    PlaceBarOnClickX(PlaceBar* placer, ManagerWindow* window, WindowMouse* mouse, MoveFunctor* functor) : placer(placer), mouse(mouse), StartMove(functor) {};
    virtual ~PlaceBarOnClickX() {};

    bool action() override {
        ManagerWindow* bar = placer->getBar();
        placer->place(mouse->getRelCoord().x - placer->getBar()->getSizeX() / 2);
        move_f->startMove();
        return true;
    };

  private:
    PlaceBar* placer;
    WindowMouse* mouse;
};

class PlaceBarOnClickY : public StartMove {
  public:
    PlaceBarOnClickY() {};
    PlaceBarOnClickY(PlaceBar* placer, ManagerWindow* window, WindowMouse* mouse, MoveFunctor* functor) : placer(placer), mouse(mouse), StartMove(functor) {};
    virtual ~PlaceBarOnClickY() {};

    bool action() override {
        ManagerWindow* bar = placer->getBar();
        placer->place(mouse->getRelCoord().y - placer->getBar()->getSizeY() / 2);
        move_f->startMove();
        return true;
    };

  private:
    PlaceBar* placer;
    WindowMouse* mouse;
};

class MoveBarRandomX : public MoveFunctor {
  public:
    MoveBarRandomX() {};
    MoveBarRandomX(PlaceBar* placer, ManagerWindow* window, WindowMouse* mouse, App* app) : placer(placer), MoveFunctor(window, mouse, app) {};

    bool action() override {
        if (on_move) {
            placer->place(placer->getBar()->getCoordX() + (mouse->getAbsCoord().x - old_coord.x));
            old_coord = mouse->getAbsCoord();
        }
        if (on_move && !mouse->isLeftClick()) {
            on_move = false;
        }
        return true;
    };

  private:
    PlaceBar* placer;
};

class MoveBarRandomY : public MoveFunctor {
  public:
    MoveBarRandomY() {};
    MoveBarRandomY(PlaceBar* placer, ManagerWindow* window, WindowMouse* mouse, App* app) : placer(placer), MoveFunctor(window, mouse, app) {};

    bool action() override {
        if (on_move && mouse->isLeftClick()) {
            placer->place(placer->getBar()->getCoordY() + (mouse->getAbsCoord().y - old_coord.y));
            old_coord = mouse->getAbsCoord();
        }
        if (on_move && !mouse->isLeftClick()) {
            on_move = false;
        }
        return true;
    };

  private:
    PlaceBar* placer;
};

class GlowPicFunctor : public VFunctor {
  public:
    GlowPicFunctor() {};
    GlowPicFunctor(PicWindow* window, const char* default_name, const char* glowing_name) : glow_window(window),
        default_wnd(window->getSizeX(), window->getSizeY(), default_name, 0, 0),
        glowing_wnd(window->getSizeX(), window->getSizeY(), glowing_name, 0, 0) {
        window->showOn(&default_wnd);
        curr_state = STATE_DEFAULT;
    };

    virtual ~GlowPicFunctor() {};

    bool action() override {
        //cout << "In glow!!" << "\n";
        if (glow_window->isPointed() && curr_state != STATE_GLOWING) {
            //cout << "changed to glow!" << "\n";
            glowing_wnd.showOn(glow_window);
            curr_state = STATE_GLOWING;
        }
        if (!glow_window->isPointed() && curr_state == STATE_GLOWING) {
            //cout << "changed to default!" << "\n";
            default_wnd.showOn(glow_window);
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

class GlowBorderFunctor : public VFunctor {
  public:
    GlowBorderFunctor() {};
    GlowBorderFunctor(BorderWindow* window, COLORREF glowing_main_c, COLORREF glowing_bord_c) : 
        glowing_main_c(glowing_main_c), glowing_bord_c(glowing_bord_c), glow_window(window) {
        default_main_c = window->getColor();
        default_bord_c = window->getBorderColor();
        curr_state = STATE_DEFAULT;
    };

    virtual ~GlowBorderFunctor() {};

    bool action() override {
        //cout << "In glow!!" << "\n";
        if (glow_window->isPointed() && curr_state != STATE_GLOWING) {
            //cout << "changed to glow!" << "\n";
            glow_window->setColor(glowing_main_c);
            glow_window->setBorderColor(glowing_bord_c);
            glow_window->setRedraw(true);
            curr_state = STATE_GLOWING;
        }
        else if (!glow_window->isPointed() && curr_state == STATE_GLOWING) {
            //cout << "changed to default!" << "\n";
            glow_window->setColor(default_main_c);
            glow_window->setBorderColor(default_bord_c);
            glow_window->setRedraw(true);
            curr_state = STATE_DEFAULT;
        }

        return true;
    };

  private:
    BorderWindow* glow_window;
    COLORREF default_main_c;
    COLORREF default_bord_c;
    COLORREF glowing_main_c;
    COLORREF glowing_bord_c;
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

class ResizeCanvas : public MoveFunctor {
  public:
    ResizeCanvas();
    ResizeCanvas(ManagerWindow* window, Renderer* render, Feather* feather, WindowMouse* mouse, App* app) : MoveFunctor(window, mouse, app), render(render), feather(feather), direction(DIRECTION_NONE) {};
    virtual ~ResizeCanvas() {};

    void startMove() override {
        move_window->getParent()->makeFirst(move_window);
        on_move = true;
        old_coord = mouse->getAbsCoord();
        Pair<int> coord = mouse->getRelCoord();
        if (coord.x >= (move_window->getSizeX() - grab_len)) {
            SET_DIR_RIGHT(direction);
        }
        if (coord.x <= grab_len) {
            SET_DIR_LEFT(direction);
        }
        if (coord.y <= grab_len) {
            SET_DIR_UP(direction);
        }
        if (coord.y >= (move_window->getSizeY() - grab_len)) {
            SET_DIR_DOWN(direction);
        }
        //std::cout << "current direction: " << direction;
        app->setActiveWindow(move_window);
    };

    void endMove() override {
        on_move = false;
        CLEAR_DIR(direction);
        app->setActiveWindow(nullptr);
    };

    bool action() override {
        if (on_move && mouse->isLeftClick()) {
            new_size = move_window->getSize();
            new_coord = move_window->getCoord();
            Pair<int> d_mouse_move = mouse->getAbsCoord() - old_coord;
            if (IS_DIR_RIGHT(direction)) {
                new_size.x = move_window->getSizeX() + d_mouse_move.x;
            }
            if (IS_DIR_LEFT(direction)) {
                new_size.x = move_window->getSizeX() - d_mouse_move.x;
                new_coord.x = move_window->getCoordX() + d_mouse_move.x;
            }
            if (IS_DIR_UP(direction)) {
                new_size.y = move_window->getSizeY() - d_mouse_move.y;
                new_coord.y = move_window->getCoordY() + d_mouse_move.y;
            }
            if (IS_DIR_DOWN(direction)) {
                new_size.y = move_window->getSizeY() + d_mouse_move.y;
            }
            if (old_coord != mouse->getAbsCoord()) {
                ResizeCanvasWindow(dynamic_cast<CanvasWindow*>(move_window), render, new_size, new_coord);
            }
            old_coord = mouse->getAbsCoord();
        } else {
          this->endMove();
        }
        return true;
    };
  
  private:
    Pair<int> new_size;
    Pair<int> new_coord;
    int resize_dx = 0;
    int resize_dy = 0;
    Renderer* render;
    Feather* feather;

    int direction;
};

class CalcGraphDotLeft : public VFunctor {
  public:
    CalcGraphDotLeft(int min_coord, int max_coord, ManagerWindow* bar, GraphWindow* window) : window(window), bar(bar), min_coord(min_coord), max_coord(max_coord) {};
    virtual ~CalcGraphDotLeft() {};

    bool action () override {
        window->setLeftDot((bar->getCoordY() - min_coord) * window->getSizeY() / (max_coord - min_coord)); 
        window->setRedraw(true);
        return true;
    }; 

  private:
    GraphWindow* window;
    ManagerWindow* bar;
    int min_coord;
    int max_coord;
};

class CalcGraphDotRight : public VFunctor {
  public:
    CalcGraphDotRight(int min_coord, int max_coord, ManagerWindow* bar, GraphWindow* window) : window(window), bar(bar), min_coord(min_coord), max_coord(max_coord) {};
    virtual ~CalcGraphDotRight() {};

    bool action () override {
        window->setRightDot((bar->getCoordY() - min_coord) * window->getSizeY() / (max_coord - min_coord)); 
        window->setRedraw(true);
        return true;
    }; 

  private:
    GraphWindow* window;
    ManagerWindow* bar;
    int min_coord;
    int max_coord;
};

class ResetBars : public VFunctor {
  public:
    ResetBars();
    ResetBars(PlaceBar* placer_left, PlaceBar* placer_right, int reset_coord_left, int reset_coord_right) : 
        placer_right(placer_right), placer_left(placer_left), reset_coord_left(reset_coord_left), reset_coord_right(reset_coord_right) {};
    virtual ~ResetBars() {};

    bool action() override { 
        placer_left->unfixPos(); placer_right->unfixPos();
        placer_left->place(reset_coord_left); placer_right->place(reset_coord_right); 
        return true; 
    };

  private:
    PlaceBar* placer_left;
    PlaceBar* placer_right;
    int reset_coord_left;
    int reset_coord_right;
};

class MakeFixedBars : public VFunctor {
  public:
    MakeFixedBars();
    MakeFixedBars(PlaceBar* placer_left, PlaceBar* placer_right) : placer_left(placer_left), placer_right(placer_right) {};
    virtual ~MakeFixedBars() {};

    bool action() override { placer_right->fixPos(); placer_left->fixPos(); return true; };

  private:
    PlaceBar* placer_left;
    PlaceBar* placer_right;
};

class MakeUnFixedBars : public VFunctor {
  public:
    MakeUnFixedBars();
    MakeUnFixedBars(PlaceBar* placer_left, PlaceBar* placer_right) : placer_left(placer_left), placer_right(placer_right) {};
    virtual ~MakeUnFixedBars() {};

    bool action() override { placer_right->unfixPos(); placer_left->unfixPos(); return true; };

  private:
    PlaceBar* placer_left;
    PlaceBar* placer_right;
};

class ClearCanvas : public VFunctor {
  public:
    ClearCanvas();
    ClearCanvas(BorderWindow* canvas, Renderer* render) : canvas(canvas), render(render) {};
    virtual ~ClearCanvas() {};

    bool action() override { render->setWindow(canvas); render->drawFilledRectangle(0, 0, max_canvas_x, max_canvas_y, white_c, white_c, 1); return true; };

  private:
    BorderWindow* canvas;
    Renderer* render;
};
