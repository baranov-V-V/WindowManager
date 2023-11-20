#pragma once

#include <ctime>
#include <map>
#include <set>

#include "gui_core/widgets/BasicInfo.h"
#include "event/Events.h"
#include "gui_core/widgets/BasicWindow.h"

using std::map;
using std::set;



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

class DisplayManager {
  public:
    DisplayManager(Renderer* render);
    ~DisplayManager();

    void setMenu(InvisibleWindow* menu) { canvas_menu = menu; };
    void updateCanvasMenu();

    void addWindow(CanvasWindow* window);
    void showWindow(CanvasWindow* window);
    void delWindow(CanvasWindow* window);
    
    void hideWindow(CanvasWindow* window);
    
    void hideAll();
    void showAll();

    void invertShowMenu();  

    int getCount() const { return windows.size(); };

    const std::set<CanvasWindow*>& getData() { return windows; };

  private:
    std::set<CanvasWindow*> windows;
    InvisibleWindow* canvas_menu;
    Renderer* render;
};

class ManagerWindow : public Texture {
  public:
    ManagerWindow();
    ManagerWindow(int x_size, int y_size, int coord_x, int coord_y, COLORREF color, ManagerWindow* parent = nullptr,
                  VFunctor* press_up_f = nullptr, VFunctor* pointed_f = nullptr, VFunctor* press_down_f = nullptr);
    ManagerWindow(int coord_x, int coord_y, const char* file_name, ManagerWindow* parent = nullptr,
                  VFunctor* press_up_f = nullptr, VFunctor* pointed_f = nullptr, VFunctor* press_down_f = nullptr);
    virtual ~ManagerWindow();

    //general part
    virtual void draw(Renderer* render) const = 0;
    void showOnTexture(const Texture* target) const;
    void setRedraw(bool state) { need_redraw = state; };
    bool isRedraw() const { return need_redraw; };

    void setShow(bool new_show) { is_shown = new_show; };
    bool getShow() const { return is_shown; };

    //!mouse press part
    bool isPointed() const { return is_pointed; };
    bool isClicked() const { return is_clicked; };

    virtual bool hitTest(double x, double y) const;

    Pair<int> toRelCoord(const Pair<int>& abs_coord); //new!!

    bool checkPointed(WindowMouse* mouse);
    bool checkPointed(const Pair<int>& abs_coord);

    bool checkLeftClick(WindowMouse* mouse);   //nullptr if it doesn't have click on it, coords of mouse on window if has click on it.
    
    bool proceedPointed(WindowMouse* mouse);
    bool proceedPressDown(WindowMouse* mouse);
    bool proceedPressUp(WindowMouse* mouse);

    EventState processEvent(const Event& event); //new!!
    
    void setPointed(VFunctor* functor);
    void setPressUp(VFunctor* functor);
    void setPressDown(VFunctor* functor);
    void setFunctor(EventType type, VFunctor* functor);

    VFunctor* getPointed() const;
    VFunctor* getPressUp() const;
    VFunctor* getPressDown() const;
    VFunctor* getFunctor(EventType type) const;

    //manager part
    ManagerWindow* getParent() const { return parent; };
    void setParent(ManagerWindow* new_parent) { parent = new_parent; };
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

    map<EventType, VFunctor*> functors;

  protected:
    
    bool need_redraw = false;
    int window_type;
    bool is_clicked;
    bool is_pointed;
    bool is_shown = true;
};

class InvisibleWindow : public ManagerWindow {
  public:
    InvisibleWindow();
    InvisibleWindow(int x_size, int y_size, int coord_x, int coord_y, ManagerWindow* parent = nullptr,
                    VFunctor* press_up_f = nullptr, VFunctor* pointed_f = nullptr, VFunctor* press_down_f = nullptr, bool need_redraw = false);
    virtual ~InvisibleWindow() {};
    
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
    PicWindow(int coord_x, int coord_y, const char* pic_name, ManagerWindow* parent = nullptr,
              VFunctor* press_up_f = nullptr, VFunctor* pointed_f = nullptr, VFunctor* press_down_f = nullptr, bool need_redraw = false);
    virtual ~PicWindow() {};
    
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
    virtual ~BorderWindow() {};

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
    virtual ~ThicknessWindow() {};       

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
    virtual ~TextButtonWindow() {};

    void draw(Renderer* render) const override;

    void setText(char* new_text) {
        text = new_text;
        this->setRedraw(true);
        this->draw(Renderer::getInstance());
        this->setRedraw(false);
    }
  
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
                 Renderer* render, App* app, const char* pic_name = nullptr);
    virtual ~CanvasWindow() {};
    //CanvasWindow(int x_size, int y_size, int coord_x, int coord_y, CanvasWindow* window, Renderer* render, Feather* feather, WindowMouse* mouse);

    const Texture& getBaseImg() { return base_img; };

    Texture* getCanvas() { return this->getChild(0); }
    char* getName() const { return name; };

  private:
    char* name;
    Texture base_img;
};

class GraphWindow : public BorderWindow {
  public:
    GraphWindow();
    GraphWindow(int x_size, int y_size, int coord_x, int coord_y, ManagerWindow* parent, Renderer* render,
                COLORREF backgroud_c = white_c, COLORREF border_c = dgrey_c, COLORREF line_c = black_c, COLORREF net_c = lgrey_c);
    virtual ~GraphWindow() {};

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
    virtual ~ClockWindow() {};

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
    virtual ~RoundWindow() {};

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
    virtual ~DedWindow() {};

    void draw(Renderer* render) const override;
    bool hitTest(double x, double y) const override;

  private:
    int radius;
    int thickness;
    COLORREF border_color;
};
