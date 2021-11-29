#pragma once

#include "BasicInfo.h"
#include "Window.h"
#include "Events.h"
#include "PluginApi.h"

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

class VFunctor {
  public:
    virtual ~VFunctor() {};
    virtual bool action(const EventData& data) = 0;
};

class DummyFunctor : public VFunctor {
  public:
    DummyFunctor();
    virtual ~DummyFunctor() {};
    bool action(const EventData& data) override;
};

class FeatherFunctor : public VFunctor {
    FeatherFunctor(ToolFeather* feather);
    virtual ~FeatherFunctor() {};

    bool action(const EventData& data) override;
    void setColor(COLORREF color);
    void setThickness(int thinckness);

  private:
    ToolFeather* feather;
    COLORREF new_color;
    int new_thickness;  
};

class DebugFunctorTrue : public VFunctor {
  public:
    DebugFunctorTrue();
    DebugFunctorTrue(ManagerWindow* window);
    virtual ~DebugFunctorTrue() {};

    bool action(const EventData& data) override;

  private:
    ManagerWindow* window;
};

class InvFunctorTrue : public VFunctor {
  public:
    InvFunctorTrue();
    virtual ~InvFunctorTrue() {};

    bool action(const EventData& data) override;

  private:
};

class StopAppFunctor : public VFunctor {
  public:
    StopAppFunctor();
    StopAppFunctor(App* app);
    virtual ~StopAppFunctor() {};

    bool action(const EventData& data) override;

  private:
    App* app;
};

class DebugFunctorFalse : public VFunctor {
  public:
    DebugFunctorFalse();
    DebugFunctorFalse(ManagerWindow* window);
    virtual ~DebugFunctorFalse() {};
    bool action(const EventData& data) override;

  private:
    ManagerWindow* window;
};

class ChangeColor : public VFunctor {
  public:
    ChangeColor();
    ChangeColor(ToolManager* tools, COLORREF color);
    virtual ~ChangeColor() {};

    bool action(const EventData& data) override;

  private:
    ToolManager* tools;
    COLORREF color;  
};

class ChangeToolTrue : public VFunctor {
  public:
    ChangeToolTrue();
    ChangeToolTrue(ToolManager* tools, int tool_index);
    virtual ~ChangeToolTrue() {};

    bool action(const EventData& data) override;

  private:
    ToolManager* tools;
    int tool_index;
};

class ChangeToolFalse : public VFunctor {
  public:
    ChangeToolFalse();
    ChangeToolFalse(ToolManager* tools, int tool_index);
    virtual ~ChangeToolFalse() {};

    bool action(const EventData& data) override;

  private:
    ToolManager* tools;
    int tool_index;
};

class NextBasicTool : public VFunctor {
  public:
    NextBasicTool();
    NextBasicTool(ToolManager* tools);
    virtual ~NextBasicTool() {};

    bool action(const EventData& data) override;

  private:
    ToolManager* tools;
};

class IncThickness : public VFunctor {
  public:
    IncThickness();
    IncThickness(ToolManager* tools);
    virtual ~IncThickness() {};

    bool action(const EventData& data) override;

  private:
    ToolManager* tools;  
};

class DecThickness : public VFunctor {
  public:
    DecThickness();
    DecThickness(ToolManager* tools);
    virtual ~DecThickness() {};

    bool action(const EventData& data) override;

  private:
    ToolManager* tools;  
};

class DrawFunctor : public VFunctor {
  public:
    DrawFunctor();
    DrawFunctor(ManagerWindow* window, Renderer* render, App* app, ToolManager* tool_manager);
    virtual ~DrawFunctor() {};

    void startDraw(const EventData& data);
    void endDraw(const EventData& data);

    bool action(const EventData& data) override;

  private:
    bool is_drawing;
    Renderer* render;
    ManagerWindow* canvas;
    
    ToolManager* tool_manager;
    VTool* curr_tool;
    Pair<int> abs_old_coord;
    
    App* app;
};

class StartDraw : public VFunctor {
  public:
    StartDraw();
    StartDraw(DrawFunctor* draw_f);
    virtual ~StartDraw() {};

    bool action(const EventData& data) override;

  protected:
    DrawFunctor* draw_f;
};

class EndDraw : public VFunctor {
  public:
    EndDraw();
    EndDraw(DrawFunctor* draw_f);
    virtual ~EndDraw() {};

    bool action(const EventData& data) override;

  private:
    DrawFunctor* draw_f;
};

class CloseCanvasFunctor : public VFunctor {
  public:
    CloseCanvasFunctor();
    CloseCanvasFunctor(CanvasWindow* window, DisplayManager* canvas_manager);
    virtual ~CloseCanvasFunctor() {};

    bool action(const EventData& data) override;

  private:
    CanvasWindow* window_to_close;
    DisplayManager* canvas_manager;
};

class SetShowFunctor : public VFunctor {
  public:
    SetShowFunctor();
    SetShowFunctor(ManagerWindow* window, bool is_shown);
    virtual ~SetShowFunctor() {};

    bool action(const EventData& data) override;

  private:
    ManagerWindow* window_to_hide;
    bool is_shown;
};

class HideAllCanvasFunctor : public VFunctor {
  public:
    HideAllCanvasFunctor();
    HideAllCanvasFunctor(DisplayManager* canvas_manager);
    virtual ~HideAllCanvasFunctor() {};

    bool action(const EventData& data) override;

  private:
    DisplayManager* canvas_manager;
};

class ShowAllCanvasFunctor : public VFunctor {
  public:
    ShowAllCanvasFunctor();
    ShowAllCanvasFunctor(DisplayManager* canvas_manager);
    virtual ~ShowAllCanvasFunctor() {};

    bool action(const EventData& data) override;

  private:
    DisplayManager* canvas_manager;
};

class FileFunctor : public VFunctor {
  public:
    FileFunctor();
    FileFunctor(ManagerWindow* window, Renderer* render, App* app);
    virtual ~FileFunctor() {};

    bool action(const EventData& data) override;

  private:
    ManagerWindow* window;
    Renderer* render;
    App* app;
};

class HelpFunctor : public VFunctor {
  public:
    HelpFunctor();
    HelpFunctor(ManagerWindow* window);
    virtual ~HelpFunctor() {};

    bool action(const EventData& data) override;;

  private:
    ManagerWindow* window;
};

class ViewFunctor : public VFunctor {
  public:
    ViewFunctor();
    ViewFunctor(ManagerWindow* window);
    virtual ~ViewFunctor() {};

    bool action(const EventData& data) override;;

  private:
    ManagerWindow* window;
};

class MoveFunctor : public VFunctor {
  public:
    MoveFunctor();
    MoveFunctor(ManagerWindow* window, App* app, ManagerWindow* activate_window = nullptr);
    virtual ~MoveFunctor() {};

    virtual void startMove(const EventData& data);
    virtual void endMove(const EventData& data);
    bool action(const EventData& data) override;

  protected:
    ManagerWindow* activate_window;
    ManagerWindow* move_window;
    Pair<int> old_coord;
    bool on_move;
    App* app;
};

class StartMove : public VFunctor {
  public:
    StartMove();
    StartMove(MoveFunctor* move_f);
    virtual ~StartMove() {};

    bool action(const EventData& data) override;;

  protected:
    MoveFunctor* move_f;
};

class EndMove : public VFunctor {
  public:
    EndMove();
    EndMove(MoveFunctor* move_f);
    virtual ~EndMove() {};

    bool action(const EventData& data) override;

  private:
    MoveFunctor* move_f;
};

class RecalcThickness : public VFunctor {
  public:
    RecalcThickness(int min_coord, int max_coord, ManagerWindow* bar, ToolManager* tools);
    virtual ~RecalcThickness() {};

    bool action(const EventData& data) override;

  private:
    ManagerWindow* bar;
    ToolManager* tools;
    int min_coord;
    int max_coord;
};

class PlaceBar {
  public:
    PlaceBar();
    PlaceBar(int min_coord, int max_coord, char type, ManagerWindow* bar, VFunctor* action);
    ~PlaceBar() {};

    void fixPos();
    void unfixPos();
    void place(const EventData& data, int new_coord);
    ManagerWindow* getBar() const;

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
    MoveBarLeft(PlaceBar* placer);
    virtual ~MoveBarLeft() {};

    bool action(const EventData& data) override;

  private:
    PlaceBar* placer;  
};

class MoveBarRight : public VFunctor {
  public:
    MoveBarRight(PlaceBar* placer);
    virtual ~MoveBarRight() {};

    bool action(const EventData& data) override;

  private:
    PlaceBar* placer;  
};

class MoveBarUp : public VFunctor {
  public:
    MoveBarUp(PlaceBar* placer);
    virtual ~MoveBarUp() {};

    bool action(const EventData& data) override;

  private:
    PlaceBar* placer;  
};

class MoveBarDown : public VFunctor {
  public:
    MoveBarDown(PlaceBar* placer);
    virtual ~MoveBarDown() {};

    bool action(const EventData& data) override;

  private:
    PlaceBar* placer;  
};

class PlaceBarOnClickX : public StartMove {
  public:
    PlaceBarOnClickX();
    PlaceBarOnClickX(PlaceBar* placer, ManagerWindow* window, MoveFunctor* functor);
    virtual ~PlaceBarOnClickX() {};

    bool action(const EventData& data) override;

  private:
    PlaceBar* placer;
    ManagerWindow* window;
};

class PlaceBarOnClickY : public StartMove {
  public:
    PlaceBarOnClickY();
    PlaceBarOnClickY(PlaceBar* placer, ManagerWindow* window, MoveFunctor* functor);
    virtual ~PlaceBarOnClickY() {};

    bool action(const EventData& data) override;

  private:
    PlaceBar* placer;
    ManagerWindow* window;
};

class MoveBarRandomX : public MoveFunctor {
  public:
    MoveBarRandomX();
    MoveBarRandomX(PlaceBar* placer, ManagerWindow* window, App* app);

    bool action(const EventData& data) override;

  private:
    PlaceBar* placer;
};

class MoveBarRandomY : public MoveFunctor {
  public:
    MoveBarRandomY();
    MoveBarRandomY(PlaceBar* placer, ManagerWindow* window, App* app);

    bool action(const EventData& data) override;

  private:
    PlaceBar* placer;
};

class GlowPicFunctor : public VFunctor {
  public:
    GlowPicFunctor(PicWindow* window, const char* default_name, const char* glowing_name);
    virtual ~GlowPicFunctor() {};

    bool action(const EventData& data) override;

  private:
    PicWindow* glow_window;
    Texture default_wnd;
    Texture glowing_wnd;
    int curr_state;
};

class GlowBorderFunctor : public VFunctor {
  public:
    GlowBorderFunctor();
    GlowBorderFunctor(BorderWindow* window, COLORREF glowing_main_c, COLORREF glowing_bord_c);

    virtual ~GlowBorderFunctor() {};

    bool action(const EventData& data) override;

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
    MakeFirst(ManagerWindow* window);
    virtual ~MakeFirst() {};

    bool action(const EventData& data) override;

  private:
    ManagerWindow* window;
};

class ResizeCanvas : public MoveFunctor {
  public:
    ResizeCanvas();
    ResizeCanvas(ManagerWindow* window, Renderer* render, App* app);
    virtual ~ResizeCanvas() {};

    void startMove(const EventData& data) override;
    void endMove(const EventData& data) override;

    bool action(const EventData& data) override;
  
  private:
    Pair<int> new_size;
    Pair<int> new_coord;
    int resize_dx = 0;
    int resize_dy = 0;
    Renderer* render;

    int direction;
};

class CalcGraphDotLeft : public VFunctor {
  public:
    CalcGraphDotLeft(int min_coord, int max_coord, ManagerWindow* bar, GraphWindow* window);
    virtual ~CalcGraphDotLeft() {};

    bool action(const EventData& data) override; 

  private:
    GraphWindow* window;
    ManagerWindow* bar;
    int min_coord;
    int max_coord;
};

class CalcGraphDotRight : public VFunctor {
  public:
    CalcGraphDotRight(int min_coord, int max_coord, ManagerWindow* bar, GraphWindow* window);
    virtual ~CalcGraphDotRight() {};

    bool action(const EventData& data) override; 

  private:
    GraphWindow* window;
    ManagerWindow* bar;
    int min_coord;
    int max_coord;
};

class ResetBars : public VFunctor {
  public:
    ResetBars();
    ResetBars(PlaceBar* placer_left, PlaceBar* placer_right, int reset_coord_left, int reset_coord_right);
    virtual ~ResetBars() {};

    bool action(const EventData& data) override;

  private:
    PlaceBar* placer_left;
    PlaceBar* placer_right;
    int reset_coord_left;
    int reset_coord_right;
};

class MakeFixedBars : public VFunctor {
  public:
    MakeFixedBars();
    MakeFixedBars(PlaceBar* placer_left, PlaceBar* placer_right);
    virtual ~MakeFixedBars() {};

    bool action(const EventData& data) override;

  private:
    PlaceBar* placer_left;
    PlaceBar* placer_right;
};

class MakeUnFixedBars : public VFunctor {
  public:
    MakeUnFixedBars();
    MakeUnFixedBars(PlaceBar* placer_left, PlaceBar* placer_right);
    virtual ~MakeUnFixedBars() {};

    bool action(const EventData& data) override;

  private:
    PlaceBar* placer_left;
    PlaceBar* placer_right;
};

class ClearCanvas : public VFunctor {
  public:
    ClearCanvas();
    ClearCanvas(BorderWindow* canvas, Renderer* render);
    virtual ~ClearCanvas() {};

    bool action(const EventData& data) override;

  private:
    BorderWindow* canvas;
    Renderer* render;
};

class SetHideFunctor : public VFunctor {
  public:
    SetHideFunctor();
    SetHideFunctor(ManagerWindow* window, bool is_shown);
    virtual ~SetHideFunctor() {};

    bool action(const EventData& data) override;

  private:
    ManagerWindow* window;
    bool is_shown;
};

class ShowCanvasMenuFunctor : public VFunctor {
  public:
    ShowCanvasMenuFunctor();
    ShowCanvasMenuFunctor(DisplayManager* canvas_manager);
    virtual ~ShowCanvasMenuFunctor() {};

    bool action(const EventData& data) override;

  private:
    DisplayManager* canvas_manager;
};

class ShowToolMenuFunctor : public VFunctor {
  public:
    ShowToolMenuFunctor();
    ShowToolMenuFunctor(ToolManager* tool_manager);
    virtual ~ShowToolMenuFunctor() {};

    bool action(const EventData& data) override;

  private:
    ToolManager* tool_manager;
};

class ClickCallbackFunctor : public VFunctor {
  public:
    ClickCallbackFunctor();
    ClickCallbackFunctor(plugin::IClickCallback* call_back);
    virtual ~ClickCallbackFunctor() { delete call_back; };

    bool action(const EventData& data) override;

  private:
    plugin::IClickCallback* call_back;
};

class AdjustVToolFunctor : public VFunctor {
  public:
    AdjustVToolFunctor();
    AdjustVToolFunctor(VTool* tool);
    virtual ~AdjustVToolFunctor() {};

    bool action(const EventData& data) override;

  private:
    VTool* tool;
};

class MoveBarX : public MoveFunctor {
  public:
    MoveBarX();
    MoveBarX(VSlider* slider);

    bool action(const EventData& data) override;

  private:
    VSlider* slider;
};

class MoveSliderBarLeft : public VFunctor {
  public:
    MoveSliderBarLeft(SliderX* slider);
    virtual ~MoveSliderBarLeft() {};

    bool action(const EventData& data) override;

  private:
    SliderX* slider;  
};

class MoveSliderBarRight : public VFunctor {
  public:
    MoveSliderBarRight(SliderX* slider);
    virtual ~MoveSliderBarRight() {};

    bool action(const EventData& data) override;

  private:
    SliderX* slider;  
};

class PlaceSliderBarOnClickX : public StartMove {
  public:
    PlaceSliderBarOnClickX(SliderX* slider);
    virtual ~PlaceSliderBarOnClickX() {};

    bool action(const EventData& data) override;

  private:
    SliderX* slider;
};
