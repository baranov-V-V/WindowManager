#pragma once
#include <queue>
#include <string>

#include "BasicInfo.h"
#include "Window.h"
#include "Tools.h"


class App {
  public:
    App(int app_x, int app_y);
    App(int app_x, int app_y, const std::string& skin_name);
    ~App() {};
    
    void run();
    void stop() { on_run = false; };

    void setActiveWindow(ManagerWindow* window) { active_window = window; };
    ManagerWindow* getActiveWindow(ManagerWindow* window) const { return active_window; };

    ToolManager* getToolManager() { return tool_manager; };
    DisplayManager* getCanvasManager() { return canvas_manager; };

  public:
    void initWindows();
    void initBasicTools();
    void makeEvents();
    void sleep(int millisec) const;

    Pair<int> app_size;
    Window users_window;
    PicWindow app_window;
    WindowMouse mouse;
    bool on_run;

    std::queue<VFunctor*> events_queue;
    
    DisplayManager* canvas_manager;
    ToolManager* tool_manager;
    ManagerWindow* active_window = nullptr;
};