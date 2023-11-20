#include <mutex>

#include "App.h"

#include "gui_core/tools/Tools.h"
#include "config/SkinsConfig.h"
#include "gui_core/actions/Functors.h"
#include "gui_core/widgets/Window.h"
#include "event/Events.h"
#include "gui_core/widgets/Slider.h"

Renderer* Renderer::instance = nullptr;
std::mutex cursor_mutex;
extern ResizeCursors cursors;

HCURSOR curr_cursor = LoadCursor(NULL, IDC_CROSS);

App::App() : users_window(1196, 690), app_size(1196, 690),
    app_window(1196, 690, 0, 0, img_back_font, nullptr, nullptr, nullptr, nullptr, true),
    mouse(static_cast<ManagerWindow*>(&app_window)), on_run(false) {

    //app_window.setFunctor(EVENT_MOUSE_PRESSED_RC, new InvFunctorTrue());
};
    
void App::run() {
    on_run = true;

    while (on_run) {
        this->makeEvents();
        app_window.draw(Renderer::getInstance());
        app_window.showOn(&users_window);
    }

};

void App::initBasicTools() {
    tool_manager = new ToolManager();

    tool_manager->addTool(new ToolFeather());
    tool_manager->addTool(new ToolEraser());
    tool_manager->addTool(new ToolRect());
    tool_manager->addTool(new Tool1(this));

    //LoadTool(tool_manager, "tools.dll");
    LoadTools(tool_manager);
    //ToolModule* circle_drawer = LoadTool("circle.dll");
    //tool_manager.addTool(circle_drawer);
}

void App::initWindows() {
    canvas_manager = new DisplayManager(Renderer::getInstance());

    //this->app_window.addChild(new SliderX(150, 20, 800, 600, 0, 10, 0.2, new SliderAction(0, 10)));

    const int palette_x = app_size.x / 2;
    const int palette_y = 2 * app_size.y / 3;

    //CanvasWindow* canvas   = new CanvasWindow(app_size.x / 2, app_size.y / 2, app_size.x / 4, app_size.y / 4, "Canvas1", &(this->app_window), &render, &feather, &mouse);
    //CanvasWindow* canvas_2 = new CanvasWindow(400, 200, 850, 200, "Canvas2", &(this->app_window), &render, &feather, &mouse);
    //InvisibleWindow* canvas_layer = MakeResizeCanvas(app_size.x / 2, app_size.y / 2, app_size.x / 4, app_size.y / 4, "Canvas1", &(this->app_window), &render, &feather, &mouse, this);
    //InvisibleWindow* canvas_layer2 = MakeResizeCanvas(app_size.x / 2, app_size.y / 2, app_size.x / 4, app_size.y / 4, "Canvas1", &(this->app_window), &render, &feather, &mouse, this);
    
    //InvisibleWindow* new_canvas_layer = GetResizedCanvas(canvas_layer, &render, &feather, &mouse, {app_size.x / 2 + 20, app_size.y / 2 + 20}, {app_size.x / 4 + 20, app_size.y / 4 + 20});
    //this->app_window.addChild(new_canvas_layer);
    
    BorderWindow* graph = MakeGraphWindow(500, 400, app_size.x / 7, app_size.y / 7, &(this->app_window), Renderer::getInstance(), this);

    
    InvFunctorTrue* debug_f = new InvFunctorTrue();
    DedWindow* round_wnd    = new DedWindow(50, 400, 300, app_size.x / 4, app_size.y / 4, silver_c, black_c, 4, Renderer::getInstance(), &(this->app_window), debug_f);
    PicWindow* menu         = MakeLayout(app_size.x, app_size.y / 23, 0, 0, &(this->app_window), 26, Renderer::getInstance(), this); //menu->children[0] == close_button;
    PicWindow* palette      = MakePalette(app_size.x / 8, app_size.y / 2, 0, app_size.y / 23, &(this->app_window), Renderer::getInstance(), this);
    /*
    InvFunctorTrue* invs_f = new InvFunctorTrue();    
    InvisibleWindow* inv_wnd = new InvisibleWindow(app_size.x / 8, app_size.y / 8, 3 * app_size.x / 4, 3 * app_size.y / 4, &(this->app_window), invs_f);
    this->app_window.addChild(inv_wnd);

    TextButtonWindow* text_button = new TextButtonWindow(120, 30, 0, 0, dgrey_c, yellow_c, 1, white_c, "button!", "Helvetica", 8, 24, ALIGN_RIGHT, &render, &(this->app_window));
    GlowBorderFunctor* glow_txt_but = new GlowBorderFunctor(text_button, blue_c, magenta_c);
    text_button->setPointed(glow_txt_but);
    inv_wnd->addChild(text_button);
    */

    StopAppFunctor* stop_app = new StopAppFunctor(this);
    menu->getChild(0)->setPressUp(stop_app);
    MakeMovable(round_wnd, round_wnd, this);

    this->app_window.addChild(palette);
    this->app_window.addChild(menu);
    this->app_window.addChild(round_wnd);
    //this->app_window.addChild(graph);
    //this->app_window.addChild(canvas_layer);
    //this->app_window.addChild(canvas_layer2);
    //this->app_window.addChild(canvas_2);
};

void App::makeEvents() {
    static int state = 0;
    static Pair<int> abs_coord(0, 0); 

    mouse.update();
    
    Event new_event;

    if ((mouse.getState() & LEFT_CLICK) && !(state & LEFT_CLICK)) {     //mouse pressed
        new_event.setType(EVENT_MOUSE_PRESSED_LC);
        new_event.setData(mouse.getAbsCoord());

        if (active_window != nullptr) {
            if (active_window->getPressDown() != nullptr) {
                active_window->getPressDown()->action(new_event.getData());   
            }
        } else {
            app_window.processEvent(new_event);
        }
    
    }  else if ((mouse.getState() & RIGHT_CLICK) && !(state & RIGHT_CLICK)) {
        new_event.setType(EVENT_MOUSE_PRESSED_RC);
        new_event.setData(mouse.getAbsCoord());

        if (active_window != nullptr) {
            if (active_window->getFunctor(EVENT_MOUSE_PRESSED_RC) != nullptr) {
                active_window->getFunctor(EVENT_MOUSE_PRESSED_RC)->action(new_event.getData());   
            }
        } else {
            //std::cout << "send event rc!\n";
            app_window.processEvent(new_event);
        } 
        
    } else if (!(mouse.getState() & LEFT_CLICK) && (state & LEFT_CLICK)) {      // mouse released
        
        new_event.setType(EVENT_MOUSE_RELEASED_LC);
        new_event.setData(mouse.getAbsCoord());

        if (active_window != nullptr) {
            if (active_window->getPressUp() != nullptr) {
                active_window->getPressUp()->action(new_event.getData());
            }
        } else {
            app_window.processEvent(new_event);
        }
        
    } else if (mouse.getAbsCoord() != abs_coord) {  //mouse moved
        
        new_event.setType(EVENT_MOUSE_MOVE);
        new_event.setData(MouseData(abs_coord, mouse.getAbsCoord() - abs_coord, mouse.getState() & LEFT_CLICK));
        
        cursors.SetCurrCursor(CURSOR_ARROW);

        if (active_window != nullptr) {
            if (active_window->getPointed() != nullptr) {
                active_window->getPointed()->action(new_event.getData());
            }
        } else {
            //cout << "sending mouse move to active\n";
            app_window.processEvent(new_event);
        }
        
    }
 
    state = mouse.getState();
    abs_coord = mouse.getAbsCoord();
};

ResizeCursors::ResizeCursors() {
    cursors[CURSOR_ARROW] = LoadCursor(NULL, IDC_ARROW);
    cursors[CURSOR_CROSS] = LoadCursor(NULL, IDC_CROSS);
    cursors[CURSOR_RESIZE_E] = LoadCursor(NULL, IDC_SIZEWE);
    cursors[CURSOR_RESIZE_N] = LoadCursor(NULL, IDC_SIZENS);
    cursors[CURSOR_RESIZE_NE] = LoadCursor(NULL, IDC_SIZENESW);
    cursors[CURSOR_RESIZE_SE] = LoadCursor(NULL, IDC_SIZENWSE);
    curr_type = CURSOR_ARROW;
};

ResizeCursors::~ResizeCursors() {

};

void ResizeCursors::SetCurrCursor(CURSOR_TYPE new_type) {
    curr_type = new_type;
    cursor_mutex.lock();
    curr_cursor = cursors[curr_type];
    cursor_mutex.unlock();
};

void ResizeCursors::setResizeCursor(int resize_direction) {
    if (resize_direction == DIRECTION_RIGHT || resize_direction == DIRECTION_LEFT) {
        curr_type = CURSOR_RESIZE_E;
    } else if (resize_direction == DIRECTION_UP || resize_direction == DIRECTION_DOWN) {
        curr_type = CURSOR_RESIZE_N;
    } else if ((IS_DIR_DOWN(resize_direction) && IS_DIR_RIGHT(resize_direction)) || (IS_DIR_UP(resize_direction) && IS_DIR_LEFT(resize_direction))) {
        curr_type = CURSOR_RESIZE_SE;
    } else {
        curr_type = CURSOR_RESIZE_NE;
    }
    cursor_mutex.lock();
    curr_cursor = cursors[curr_type];
    cursor_mutex.unlock();
};