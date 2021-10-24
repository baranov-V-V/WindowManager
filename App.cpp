#include "WindowManager.h"

App::App(int app_x, int app_y) : users_window(app_x, app_y), feather(black_c, 2), app_size(app_x, app_y),
    app_window(app_x, app_y, 0, 0, img_back_font),
    mouse(static_cast<ManagerWindow*>(&app_window)), render(&app_window, 0, 0, app_x, app_y) {};
    
void App::run() {
    this->initWindows();

    while (!GetAsyncKeyState(VK_ESCAPE)) {
        app_window.draw(&render);
        app_window.showOn(&users_window);
        this->proceedMouseEvent();
        //this->sleep(5);
    }
};

void App::initWindows() {
    const int palette_x = app_size.x / 2;
    const int palette_y = 2 * app_size.y / 3;

    CanvasWindow* canvas   = new CanvasWindow(400, 300, 400, 200, "Canvas1", &(this->app_window), &render, &feather, &mouse);
    CanvasWindow* canvas_2 = new CanvasWindow(200, 400, 850, 200, "Canvas2", &(this->app_window), &render, &feather, &mouse);
    PicWindow* menu      = MakeLayout(app_size.x, app_size.y / 23, 0, 0, &(this->app_window), 26);
    PicWindow* palette   = MakePalette(app_size.x / 8, app_size.y - app_size.y / 23, 0, app_size.y / 23, &(this->app_window), &(this->render), &(this->feather));

    this->app_window.addChild(palette);
    this->app_window.addChild(menu);
    this->app_window.addChild(canvas);
    this->app_window.addChild(canvas_2);
};

void App::proceedMouseEvent() {
    static int state = 0;
    static Pair<int> abs_coord(0, 0); 

    mouse.update();
    
    //cout << "state_old: " << state << "state_new: " << mouse.getState() <<  "\n";

    if ((mouse.getState() & LEFT_CLICK) && !(state & LEFT_CLICK)) {
        cout << "pressed_down!" << "\n";
        app_window.proceedPressDown(&mouse);
    }

    if (!(mouse.getState() & LEFT_CLICK) && (state & LEFT_CLICK)) {
        cout << "pressed_up!" << "\n";
        app_window.proceedPressUp(&mouse);
    }

    if (mouse.getAbsCoord() != abs_coord) {
        app_window.proceedPointed(&mouse);
    }

    state = mouse.getState();
    abs_coord = mouse.getAbsCoord();
};