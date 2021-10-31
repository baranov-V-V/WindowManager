#pragma once

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

const int MAX_THICKNESS = 20;
const int MIN_THICKNESS = 2;

#define IS_CLICKABLE(type) (type & TYPE_CLICKABLE)
#define IS_GLOWABLE(type) (type & TYPE_GLOWABLE)

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