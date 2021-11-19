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
};

#define SET_OPTION_COLOR(option)     (option = option | OPTION_COLOR)
#define SET_OPTION_THICKNESS(option) (option = option | OPTION_THICKNESS)

#define IS_OPTION_COLOR(option)    (option & OPTION_COLOR)
#define IS_OPTION_THICKNES(option) (option & OPTION_THICKNESS)

class VTool {
  public:
    VTool() : color(black_c) {};
    VTool(COLORREF color, char* name = "noname", int options = OPTION_NONE) : color(color), options(options) {};
    virtual ~VTool() {};
    
    virtual void ProceedPressUp(Texture* target, Renderer* render, int x, int y) {};
    virtual void ProceedPressDown(Texture* target, Renderer* render, int x, int y) = 0;
    virtual void ProceedMove(Texture* target, Renderer* render, int dx, int dy) {};

    void setColor(COLORREF color) { VTool::color = color; };
    COLORREF getColor() const { return color;}

    int getOption() const { return options; };

  private:
    char* name;
  protected:
    COLORREF color;
    int options = OPTION_NONE;
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
    ToolFeather(COLORREF color = black_c, char* name = "Feather", int thickness = 2) : VTool(color, name, OPTION_COLOR | OPTION_THICKNESS), thickness(thickness) {};
    virtual ~ToolFeather() {};

    virtual void ProceedPressDown(Texture* target, Renderer* render, int x, int y) override;
    virtual void ProceedMove(Texture* target, Renderer* render, int dx, int dy) override;
    
    void setThickness(int thickness) { ToolFeather::thickness = thickness; };
    int getThickness() const { return thickness; }

  private:
    Pair<int> old_coord;
    int thickness;
};

class ToolEraser : public VTool {
  public:
    ToolEraser() : VTool(white_c, "Eraser", OPTION_THICKNESS), thickness(5) {};
    virtual ~ToolEraser() {};

    virtual void ProceedPressDown(Texture* target, Renderer* render, int x, int y) override;
    virtual void ProceedMove(Texture* target, Renderer* render, int dx, int dy) override;
    
    void setThickness(int thickness) { ToolEraser::thickness = thickness; };
    int getThickness() const { return thickness; };

  private:
    Pair<int> old_coord;
    int thickness;
};

class ToolRect : public VTool {
  public:
    ToolRect() : VTool(black_c, "Rect filled", OPTION_COLOR) {};
    virtual ~ToolRect() {};

    virtual void ProceedPressDown(Texture* target, Renderer* render, int x, int y) override;
    virtual void ProceedMove(Texture* target, Renderer* render, int dx, int dy) override;
    virtual void ProceedPressUp(Texture* target, Renderer* render, int x, int y) override;

  private:
    Pair<int> old_coord;
    Pair<int> new_coord;
};

class ToolModule : public VTool {
  public:
    ToolModule(int options, char* name, ToolFunc press, ToolFunc move, ToolFunc release, void* info = nullptr);
    
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