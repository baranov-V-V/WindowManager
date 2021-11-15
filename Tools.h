#pragma once

#include <vector>
#include "BasicInfo.h"

using std::vector;

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
    ToolFeather(COLORREF color = black_c, int thickness = 2) : VTool(color), thickness(thickness) {};
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
    ToolEraser() : VTool(white_c), thickness(5) {};
    virtual ~ToolEraser() {};

    virtual void ProceedPressDown(Texture* target, Renderer* render, int x, int y) override;
    virtual void ProceedMove(Texture* target, Renderer* render, int dx, int dy) override;
    
    int getThickness()  const { return thickness; };

  private:
    Pair<int> old_coord;
    int thickness;
};

class ToolRect : public VTool {
  public:
    ToolRect() : VTool(black_c) {};
    virtual ~ToolRect() {};

    virtual void ProceedPressDown(Texture* target, Renderer* render, int x, int y) override;
    virtual void ProceedMove(Texture* target, Renderer* render, int dx, int dy) override;
    virtual void ProceedPressUp(Texture* target, Renderer* render, int x, int y) override;

  private:
    Pair<int> old_coord;
    Pair<int> new_coord;
};