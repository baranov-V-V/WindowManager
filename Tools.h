#pragma once

#include <vector>
#include <iostream>
#include <string>
#include "NewPluginApi.h"
#include "BasicInfo.h"
#include "SkinsConfig.h"
#include "App.h"

//#include "ToolModule.h"


using std::string;
using std::vector;

const string plugins_folder = "plugins";

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
#define SET_OPTION_ADJUST(option)    (option = option | OPTION_ADJUST)

#define IS_OPTION_COLOR(option)    (option & OPTION_COLOR)
#define IS_OPTION_THICKNES(option) (option & OPTION_THICKNESS)
#define IS_OPTION_ADJUST(option) (option & OPTION_ADJUST)

static const char* feather_name = "Feather";
static const char* eraser_name = "Eraser";
static const char* rect_fill_name = "Rects drawer";
static const char* tool1_name = "Tool1";
static const char* no_name = "Unnamed instrument";

class VTool {
  public:
    VTool() : color(black_c) {};
    VTool(COLORREF color, int thickness, const char* name) : color(color), thickness(thickness), name(name), options(OPTION_COLOR | OPTION_THICKNESS), adjust_window(nullptr), icon(nullptr) {};
    VTool(COLORREF color, int thickness, const char* name, const char* icon_name) : color(color), thickness(thickness), name(name), options(OPTION_COLOR | OPTION_THICKNESS), adjust_window(nullptr), icon(nullptr) {
        if (icon_name) {
            icon = new PicWindow(0, 0, icon_name);
        }
    };
    VTool(COLORREF color, int thickness, const char* name, ManagerWindow* icon) : color(color), thickness(thickness), name(name), options(OPTION_COLOR | OPTION_THICKNESS), adjust_window(nullptr), icon(icon) {};
    virtual ~VTool() {};
    
    virtual void ProceedPressUp(Texture* target, Renderer* render, int x, int y) {};
    virtual void ProceedPressDown(Texture* target, Renderer* render, int x, int y) = 0;
    virtual void ProceedMove(Texture* target, Renderer* render, int x, int y, int dx, int dy) {};
    
    void adjust();

    void setAdjustWindow(ManagerWindow* window) { 
        if (adjust_window == nullptr) {
            adjust_window = window;
            adjust_window->setShow(false);
            App::getInstance()->getAppWindow()->addChild(adjust_window);
        } else {
            std::cout << "trying set new adjust window while old is attached\n";
        }
    };
    ManagerWindow* getAdjustWindow() const { return adjust_window; };

    void setColor(COLORREF color) { VTool::color = color; };
    COLORREF getColor() const { return color; }

    void setThickness(int thickness) { VTool::thickness = thickness; };
    int getThickness() const { return thickness; };

    int getOption() const { return options; };
    const char* getName() const { return name; };

    ManagerWindow* getIcon() const { return icon; };

  private:
    const char* name;
    ManagerWindow* adjust_window; //must be already be child of app_window in App class;
    ManagerWindow* icon;
  protected:
    COLORREF color;
    int thickness;
    int options;
};

class ToolManager {
  public:
    ToolManager() : curr_tool(0) {};
    ~ToolManager() {};

    void setMenu(InvisibleWindow* menu) { tools_menu = menu; };
    void updateToolsMenu();

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

    void invertShowMenu();  

  private:
    std::vector<VTool*> tools;

    InvisibleWindow* tools_menu;

    size_t curr_tool;
};

class ToolFeather : public VTool {
  public:
    ToolFeather() : VTool(black_c, 2, feather_name, img_feather) {};
    virtual ~ToolFeather() {};

    virtual void ProceedPressDown(Texture* target, Renderer* render, int x, int y) override;
    virtual void ProceedMove(Texture* target, Renderer* render, int x, int y, int dx, int dy) override;

  private:
    Pair<int> old_coord;
};

class ToolEraser : public VTool {
  public:
    ToolEraser() : VTool(white_c, 5, eraser_name, img_eraser) {};
    virtual ~ToolEraser() {};

    virtual void ProceedPressDown(Texture* target, Renderer* render, int x, int y) override;
    virtual void ProceedMove(Texture* target, Renderer* render, int x, int y, int dx, int dy) override;

  private:
    Pair<int> old_coord;
};

class ToolRect : public VTool {
  public:
    ToolRect() : VTool(black_c, 1, rect_fill_name) {};
    virtual ~ToolRect() {};

    virtual void ProceedPressDown(Texture* target, Renderer* render, int x, int y) override;
    virtual void ProceedMove(Texture* target, Renderer* render, int x, int y, int dx, int dy) override;
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
    virtual void ProceedMove(Texture* target, Renderer* render, int x, int y, int dx, int dy) override;
    virtual void ProceedPressUp(Texture* target, Renderer* render, int x, int y) override;

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

class ToolPlugin : public VTool {
  public:
    ToolPlugin(plugin::ITool* plugin_tool);
    
    virtual ~ToolPlugin() { delete tool; };

    virtual void ProceedPressDown(Texture* target, Renderer* render, int x, int y) override;
    virtual void ProceedMove(Texture* target, Renderer* render, int x, int y, int dx, int dy) override;
    virtual void ProceedPressUp(Texture* target, Renderer* render, int x, int y) override;

  private:
    plugin::ITool* tool;
};

class FilterPlugin : public VTool {
  public:
    FilterPlugin(plugin::IFilter* plugin_filter);
    
    virtual ~FilterPlugin() { delete filter; };

    virtual void ProceedPressDown(Texture* target, Renderer* render, int x, int y) override;

  private:
    plugin::IFilter* filter;
};

void LoadTool(ToolManager* tool_manager, char* file_name);
void LoadTools(ToolManager* tool_manager);