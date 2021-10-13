#include "TXLib.h"
#include <cmath>
#include <ctime>
#include "MathVector2D.h"
#include "MathVector3D.h"

using std::rand;
using std::max;

const double dt = 0.03;

const double vi_x = 5;
const double vi_y = 5;
const double vi_z = 10;

const double li_x = 5;
const double li_y = 5;
const double li_z = 0;

typedef MathVector3D<double> vec3d;
typedef MathVector3D<double> vec_c;

enum WALLS {
    LEFT    = 1,
    RIGHT   = 2,
    UP      = 3,
    DOWN    = 4,
    NO_WALL = -1
};

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

class BasicPhys;
class Window;
class Renderer;
class MainWindow;
class Frame;
class VirtualWindow;
class Graph;

template <typename T>
class Pair {
  public:
    Pair() = default;
    Pair(T x, T y) : x(x), y(y) {};
    T x;
    T y;
};

class Coordinates {
  public:
    Coordinates(double min_x, double max_x, double min_y, double max_y) : max(max_x, max_y), min(min_x, min_y) {};

    int checkX(double x) const;
    int checkY(double y) const;
    
    double getMaxX() const {return max.x;};    
    double getMinX() const {return min.x;};
    double getMaxY() const {return max.y;};    
    double getMinY() const {return min.y;};

    double setMaxX(double x) { max.x = x; };    
    double setMinX(double x) { min.x = x; };
    double setMaxY(double y) { max.y = y; };    
    double setMinY(double y) { min.y = y; };

  private:
    Pair<double> max;
    Pair<double> min;  
};

class Renderer {
  public:
    Renderer(VirtualWindow* window, Coordinates* coord);

    void setWindow(VirtualWindow* new_window);
    VirtualWindow* getWindow() const { return window; };
    
    void setCoordinates(Coordinates* new_coordinates) { coordinates = new_coordinates; };
    Coordinates* getCoordinates() const { return coordinates; };
    
    void clear() const;
    void drawLine(double x_begin, double y_begin, double x_end, double y_end, COLORREF color = TX_BLACK, int thickness = 1) const;
    void setPixel(double x, double y, COLORREF color) const;
    void drawCircle(double x, double y, double r, COLORREF color = TX_BLACK, int thickness = 1) const;
    void drawRectangle(double x1, double y1, double x2, double y2, COLORREF color = TX_BLACK, int thinkness = 1) const;
    void drawRoundRect(double x1, double y1, double x2, double y2, double width, double height, COLORREF color = TX_BLACK, int thinkness = 1) const;

    int toPixelX(double coord) const;
    int toPixelY(double coord) const;
    
    double toCoordX(int x) const;
    double toCoordY(int y) const;

    double getScaleX() const { return scale.x; };
    double getScaleY() const { return scale.y; };  

    void calcScale();

  private:
    VirtualWindow* window;
    Coordinates* coordinates;
    Pair<double> scale;
};


class BasicPhys {
  public:
    BasicPhys(double v_x, double v_y, double mass) : speed(v_x, v_y), mass(mass) {};

    virtual void move(double time) = 0;

    MathVector2D<double> get_speed();
    void setSpeed(double v_x, double v_y);
    void setSpeed(MathVector2D<double> new_speed);
    double getMass() const { return mass; };
    double setMass(double new_mass) { mass = new_mass; }; 

  protected:
    MathVector2D<double> speed;
    double mass;
};

class Shape {
  public:
    Shape(double x, double y) : coord(x, y), shape_type(NOT_SPECIFIED) {};
    virtual void draw(const Renderer& renderer) const = 0;

    double getX() const {return coord.x;};
    double getY() const {return coord.y;};
    int getType() const {return shape_type;};

  protected:
    Pair<double> coord;
    int shape_type;
};

class PhysShape : public Shape, public BasicPhys {
  public:
    PhysShape(double x, double y, double v_x, double v_y, double mass) :
        Shape(x, y), BasicPhys(v_x, v_y, mass) {};
};

class PhysCircle : public PhysShape {
  public:
    PhysCircle(double x, double y, double radius, double v_x, double v_y, double mass, COLORREF color);
    PhysCircle();
    void move(double time) override;
    virtual void draw(const Renderer& renderer) const override;
    
    double getRadius() const { return radius; };  
    bool checkBound(double x, double y) const;
  
  protected:
    double radius;
    COLORREF color;
};

//z-axis is points towards us.
class PhysSphere : public PhysCircle {
  public:
    PhysSphere(double x, double y, double radius, double v_x, double v_y, double mass,
               COLORREF color, COLORREF ambient, COLORREF light, vec3d light_pos, vec3d view_pos);
    PhysSphere();

    virtual void draw(const Renderer& renderer) const override;
  
    void setView(vec3d new_view) { view_pos = new_view; };
    void setLight(vec3d new_light) { light_pos = new_light; };

    vec3d getView() const { return view_pos; };
    vec3d getLight() const { return light_pos; };

  private:
    vec_c ambient_intense;
    vec_c light_intense;
    vec3d light_pos;
    vec3d view_pos;
};

class PhysRect: public PhysShape {
  public:
    virtual void draw(const Renderer& renderer) const override;
    PhysRect(double x1, double y1, double x2, double y2, double v_x, double v_y, double mass, COLORREF color);
    PhysRect();
    void move(double time) override;
    
    double getX1() const { return coord1.x; };
    double getY1() const { return coord1.y; }; 

    double getX2() const { return coord2.x; };
    double getY2() const { return coord2.y; };

    double getSizeX() const { return size.x; };
    double getSizeY() const { return size.y; };

    double set_new_size(double x, double y);

  protected:
    Pair<double> size;
    Pair<double> coord1;
    Pair<double> coord2;

  private:
    COLORREF color;
};

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


class Frame {
  public:
    Frame(int x_size, int y_size) : size(x_size, y_size) {};

    int getSizeX() const;
    int getSizeY() const;

  protected:
    Pair<int> size;
};

class VirtualWindow : public Frame {
  public:
    VirtualWindow(int x_size, int y_size, COLORREF color);
    
    HDC getHdc() const;
    RGBQUAD* getBuf() const;
    COLORREF getColor() const;
    int getType() const { return type; };

  protected:
    int type;
    HDC screen;
    RGBQUAD* buf_screen;
    COLORREF color;
};

class Window : public VirtualWindow {
  public:
    Window(int x_size, int y_size, COLORREF color);
};

class Texture : public VirtualWindow {
  public:
    Texture(int x_size, int y_size, COLORREF color, int coord_x, int coord_y);
    ~Texture();
    
    int getCoordX() const;
    int getCoordY() const;
    
    void show_on(const Window* window) const;

  private:
    Pair<int> coord;
};

class VFunctor {
  public:
    virtual void action() = 0;
};

class IncreaseTemp : public VFunctor {
  public:
    IncreaseTemp(Manager* manager) : manager(manager) {};
    void action() override;
  private:
    Manager* manager;
};

class DecreaseTemp : public VFunctor {
  public:
    DecreaseTemp(Manager* manager) : manager(manager) {};
    void action() override;
  private:
    Manager* manager;
};

class AddCircle : public VFunctor {
  public:
    AddCircle(Manager* manager) : manager(manager) {};
    void action() override;
  private:
    Manager* manager;
};

class AddRect : public VFunctor {
  public:
    AddRect(Manager* manager) : manager(manager) {};
    void action() override;
  private:
    Manager* manager;
};

class AdjustScale : public VFunctor {
  public:
    AdjustScale(Manager* manager, Renderer* render) : manager(manager), render(render) {};
    void action() override;
  private:
    Renderer* render;
    Manager* manager;
};

class Button : public Frame {
  public:
    Button(int size_x, int size_y, int x, int y);

    int getCoordX() const { return coord.x; };
    int getCoordY() const { return coord.y; };
    
    bool checkPress(const Renderer& render) const;
    bool checkBoundary(const Renderer& render) const;
    virtual void action() const = 0;
    virtual void draw(const Renderer& render) const = 0;

  protected:
    Pair<int> coord;
};

class TextButton : public Button {
  public:
    TextButton(int x_size, int y_size, int x, int y, COLORREF color, char* text, VFunctor* functor);
    TextButton();
    TextButton(const TextButton&);
    ~TextButton();
    
    COLORREF    getColor() const { return color; };
    const char* getText()  const { return text;  };
    int         getLen()   const { return len;   };
    
    void draw(const Renderer& render) const override;
    void action() const override;

  private:
    COLORREF color;
    char* text;
    int len;
    VFunctor* functor;
};

class ButtonManager {
  public:
    ButtonManager(int max_count);
    ~ButtonManager();

    void addButton(Button* button);
    void delButton(const Button* button);
    void delLast();

    Button* getButton(int pos) const { return buttons[pos]; };
    int getCount() const { return count; };

    void drawAll(const Renderer& renderer) const;
    void checkAll(const Renderer& render, Manager& manager) const;

  private:
    int count;
    int size;
    Button** buttons;
};


class Graph {
public:
    Graph(double max_x, double min_x);
    ~Graph();

    void draw(const Renderer& render) const;
    void drawAxis(const Renderer& render);
    void addDot(double dot);
private:
    double* dots;
    int dots_count;
};


void ProceedMoving(Manager& manager, Renderer& render);

RGBQUAD ToRGBQUAD(COLORREF color);

void CheckCollision_CC(PhysShape* lhs, PhysShape* rhs, Manager& manager);
void CheckCollision_CR(PhysShape* lhs, PhysShape* rhs, Manager& manager);
void CheckCollision_RC(PhysShape* lhs, PhysShape* rhs, Manager& manager);
void CheckCollision_RR(PhysShape* lhs, PhysShape* rhs, Manager& manager);

void ProceedCollision(PhysCircle* lhs, PhysCircle* rhs);
int CheckWall_C(PhysShape* object, const Coordinates* coord);
int CheckWall_R(PhysShape* object, const Coordinates* coord);
void ProceedWall(PhysShape* object, const Coordinates* coord, int wall_type);

void CheckAllCollisions(Manager& manager, Coordinates* coord);

void ActionAddCircle(Manager& manager);
void ActionAddRect(Manager& manager);
void ActionIncSpeed(Manager& manager);
void ActionDecSpeed(Manager& manager);

void PrintMousePos();
int NumOfDigits(int num);
void GetMouse(double& x, double& y, const Renderer& render);


RGBQUAD ToRGBQUAD(COLORREF color);
RGBQUAD ToRGBQUAD(BYTE red, BYTE green, BYTE blue);
vec_c ToVec(COLORREF color);
COLORREF ToCOLORREF(vec_c color_vec);
double CorrectColor(double color);
double Random(double min, double max);