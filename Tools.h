#pragma once

#include <vector>
#include <iostream>
#include "BasicInfo.h"
#include "ToolModule.h"

using std::vector;

enum BASIC_TOOLS {
    TOOL_FEATHER = 0,
    TOOL_ERASER = 1
};

enum TOOL_OPTIONS {
  OPTION_NONE = 0,
  OPTION_COLOR = 1,
  OPTION_THICKNESS = 2,
  OPTION_ADJUST = 4
};

#define SET_OPTION_COLOR(option)     (option = option | OPTION_COLOR)
#define SET_OPTION_THICKNESS(option) (option = option | OPTION_THICKNESS)
#define SET_OPTION_ADJUST(option) (option = option | OPTION_ADJUST)

#define IS_OPTION_COLOR(option)    (option & OPTION_COLOR)
#define IS_OPTION_THICKNES(option) (option & OPTION_THICKNESS)
#define IS_OPTION_ADJUST(option) (option & OPTION_ADJUST)

static char* feather_name = "Feather";
static char* eraser_name = "Eraser";
static char* rect_fill_name = "Rects drawer";
static char* tool1_name = "Tool1";
static char* no_name = "Unnamed instrument";

class VTool {
  public:
    VTool() : color(black_c) {};
    VTool(COLORREF color, int thickness, char* name = no_name) : color(color), thickness(thickness), name(name), options(OPTION_COLOR | OPTION_THICKNESS) {};
    virtual ~VTool() {};
    
    virtual void ProceedPressUp(Texture* target, Renderer* render, int x, int y) {};
    virtual void ProceedPressDown(Texture* target, Renderer* render, int x, int y) = 0;
    virtual void ProceedMove(Texture* target, Renderer* render, int dx, int dy) {};
    virtual void adjust() {};

    void setColor(COLORREF color) { VTool::color = color; };
    COLORREF getColor() const { return color;}

    void setThickness(int thickness) { VTool::thickness = thickness; };
    int getThickness() const { return thickness; };

    int getOption() const { return options; };
    char* getName() const { return name; };

  private:
    char* name;
  protected:
    COLORREF color;
    int thickness;
    int options;
};

class ToolManager {
  public:
    ToolManager() : curr_tool(0) {};
    ~ToolManager() {};

    size_t getCount() const { return tools.size(); };
    VTool* getCurrTool() const { return tools[curr_tool]; };
    void setCurrTool(int tool_index) { curr_tool = tool_index; };

    void addTool(VTool* tool);
    void delTool(VTool* tool);
    void delLast();

    VTool* operator[] (int index) { return tools[index]; };
    const VTool* operator[] (int index) const { return tools[index]; };
  
    void setNext();
    void setPrev();

  private:
    std::vector<VTool*> tools;

    size_t curr_tool;
};

class ToolFeather : public VTool {
  public:
    ToolFeather(COLORREF color = black_c, char* name = feather_name, int thickness = 2) : VTool(color, thickness, name) {};
    virtual ~ToolFeather() {};

    virtual void ProceedPressDown(Texture* target, Renderer* render, int x, int y) override;
    virtual void ProceedMove(Texture* target, Renderer* render, int dx, int dy) override;

  private:
    Pair<int> old_coord;
};

class ToolEraser : public VTool {
  public:
    ToolEraser() : VTool(white_c, 5, eraser_name) {};
    virtual ~ToolEraser() {};

    virtual void ProceedPressDown(Texture* target, Renderer* render, int x, int y) override;
    virtual void ProceedMove(Texture* target, Renderer* render, int dx, int dy) override;

  private:
    Pair<int> old_coord;
};

class ToolRect : public VTool {
  public:
    ToolRect() : VTool(black_c, 1, rect_fill_name) {};
    virtual ~ToolRect() {};

    virtual void ProceedPressDown(Texture* target, Renderer* render, int x, int y) override;
    virtual void ProceedMove(Texture* target, Renderer* render, int dx, int dy) override;
    virtual void ProceedPressUp(Texture* target, Renderer* render, int x, int y) override;

  private:
    Pair<int> old_coord;
    Pair<int> new_coord;
};

class Tool1 : public VTool {
  public:
    Tool1(App* app);
    virtual ~Tool1() {};

    virtual void ProceedPressDown(Texture* target, Renderer* render, int x, int y) override;
    virtual void ProceedMove(Texture* target, Renderer* render, int dx, int dy) override;
    virtual void ProceedPressUp(Texture* target, Renderer* render, int x, int y) override;
    virtual void adjust() override;

  private:
    void draw(Texture* target, Renderer* render, int x, int y);

    Pair<int> old_coord;

    vector<vector<bool>> mark;
    BorderWindow* adjust_window;
    App* app;

    int d_red = 10;
    int d_green = 10;
    int d_blue = 10;
};



class ToolModule : public VTool {
  public:
    ToolModule(char* name, ToolFunc press, ToolFunc move, ToolFunc release, void* info = nullptr);
    
    virtual ~ToolModule() {};

    virtual void ProceedPressDown(Texture* target, Renderer* render, int x, int y) override;
    virtual void ProceedMove(Texture* target, Renderer* render, int dx, int dy) override;
    virtual void ProceedPressUp(Texture* target, Renderer* render, int x, int y) override;

  private:
    void* info;
    ToolFunc press;
    ToolFunc move;
    ToolFunc release;
    StandartModuleFuncs funcs;
};

ToolModule* LoadTool(char* file_name);