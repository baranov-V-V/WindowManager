#pragma once

#include<windows.h>

enum WINDOW_TYPES {
    TYPE_WINDOW = 1,
    TYPE_TEXTURE = 2,
};

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

const int MAX_THICKNESS = 20;
const int MIN_THICKNESS = 2;

const int close_button_x = 30;
const int menu_size_y = 25;

#define IS_CLICKABLE(type) (type & TYPE_CLICKABLE)
#define IS_GLOWABLE(type)  (type & TYPE_GLOWABLE)

const COLORREF palette_c   = RGB( 37,  37,  38);
const COLORREF black_c     = RGB(  0,   0,   0);
const COLORREF white_c     = RGB(255, 255, 255); 
const COLORREF canvas_c    = RGB( 85,  86, 179);
const COLORREF blue_c      = RGB( 15,   2, 255);
const COLORREF red_c       = RGB(255,   2,   2);
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

const int grab_len = 5; //in pixels

const int max_canvas_x = 1980;
const int max_canvas_y = 1080;

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
class VTool;
class ToolManager;
class ToolFeather;
class ToolEraser;
class ToolRect;
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


class VFunctor;
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

PicWindow* MakeBasicMenu(int x_size, int y_size, int coord_x, int coord_y, ManagerWindow* parent, int but_x = 30);
PicWindow* MakePalette(int x_size, int y_size, int coord_x, int coord_y, ManagerWindow* parent, Renderer* render, App* app);
PicWindow* MakeLayout(int x_size, int y_size, int coord_x, int coord_y, ManagerWindow* parent, int comp_x, Renderer* render, App* app);
void MakeMovable(ManagerWindow* activate_wnd, ManagerWindow* move_wnd, App* app);
void WriteCanvasName(CanvasWindow* canvas_layer, Renderer* render);

InvisibleWindow* MakeResizeCanvas(int size_x, int size_y, int coord_x, int coord_y, char* name, ManagerWindow* parent, Renderer* render, App* app);
InvisibleWindow* MakeStaticCanvas(int size_x, int size_y, int coord_x, int coord_y, char* name, ManagerWindow* parent, Renderer* render, App* app);
void ReplaceFunctors(ManagerWindow* lhs, ManagerWindow* rhs);
void ResizeCanvasWindow(CanvasWindow* canvas_layer, Renderer* render, Pair<int> new_size, Pair<int> new_coord);
BorderWindow* MakeGraphWindow(int size_x, int size_y, int coord_x, int coord_y, ManagerWindow* parent, Renderer* render, App* app);
