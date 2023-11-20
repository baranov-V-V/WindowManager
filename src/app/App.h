#pragma once
#include <queue>
#include <string>

//#include "src/gui_core/widgets/BasicWindow.h"
#include "../gui_core/widgets/BasicWindow.h"
#include "gui_core/widgets/Window.h"

class App {
  private:
    App();
  public:
    static App* getInstance() {
        if (instance == nullptr) {
            instance = new App();
        }
        return instance;
    };
    //App(int app_x, int app_y, const std::string& skin_name);
    ~App() {};
    
    void run();
    void stop() { on_run = false; };

    void setActiveWindow(ManagerWindow* window) { active_window = window; };
    ManagerWindow* getActiveWindow(ManagerWindow* window) const { return active_window; };

    ToolManager* getToolManager() { return tool_manager; };
    DisplayManager* getCanvasManager() { return canvas_manager; };

    ManagerWindow* getAppWindow() { return &app_window; };

  public:
    static App* instance;

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

enum CURSOR_TYPE {
    CURSOR_ARROW,
    CURSOR_CROSS,
    CURSOR_RESIZE_E,
    CURSOR_RESIZE_N,
    CURSOR_RESIZE_NE,
    CURSOR_RESIZE_SE,
};

class ResizeCursors {
  public:
    ResizeCursors();
    ~ResizeCursors();

    HCURSOR getCurrCursor() { return cursors[curr_type]; };
    void SetCurrCursor(CURSOR_TYPE new_type);

    void setResizeCursor(int direction);

  private:
    
    std::map<CURSOR_TYPE, HCURSOR> cursors;

    CURSOR_TYPE curr_type;
};