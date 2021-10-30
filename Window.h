#include <windows.h>
#include <windef.h>

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
