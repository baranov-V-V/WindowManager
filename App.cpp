#include "WindowManager.h"

App::App(int app_x, int app_y) : users_window(app_x, app_y), feather(black_c, 2), app_size(app_x, app_y),
    app_window(app_x, app_y, 0, 0, nullptr, img_back_font, nullptr),
    mouse(static_cast<ManagerWindow*>(&app_window)), render(&app_window, 0, 0, app_x, app_y) {};
    
void App::run() {
    this->initWindows();

    //render.setWindow(app_window.getChild(2));
    //render.drawCircle(100, 100, 30, black_c, 5);
    
    while (!GetAsyncKeyState(VK_ESCAPE)) {
        app_window.draw(&render);
        //render.setWindow(app_window.getChild(0));
        //render.drawCircle(20, 20, 30, black_c, 5);
        app_window.showOn(&users_window);
        this->proceedMouseEvent();
        //this->sleep(5);
    }
};

void App::initWindows() {
    const int palette_x = app_size.x / 2;
    const int palette_y = 2 * app_size.y / 3;

    PicWindow* canvas    = MakeCanvas(400, 300, 400, 200, &(this->app_window), &render, &feather, &mouse);
    PicWindow* canvas_2  = MakeCanvas(200, 400, 850, 200, &(this->app_window), &render, &feather, &mouse);
    PicWindow* menu      = MakeLayout(app_size.x, app_size.y / 23, 0, 0, &(this->app_window), 26);
    PicWindow* palette   = MakePalette(app_size.x / 8, app_size.y - app_size.y / 23, 0, app_size.y / 23, &(this->app_window), &(this->render), &(this->feather));

    this->app_window.addChild(palette);
    this->app_window.addChild(menu);
    this->app_window.addChild(canvas);
    this->app_window.addChild(canvas_2);
};

void App::proceedMouseEvent() {
    mouse.update();
    if(!app_window.proceedClicks(&mouse)) {
        //cout << "Click with info wasn't taken by any window\n";
        //mouse.printState(); 
    };
};