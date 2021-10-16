#include "WindowManager.h"

App::App(int app_x, int app_y) : users_window(app_x, app_y), feather(), app_size(app_x, app_y),
    app_window(app_x, app_y, white_c, 0, 0, nullptr, nullptr, black_c, 5) {};
    
void App::run() {
    this->initWindows();
    Renderer render(static_cast<BasicWindow*>(&app_window), 0, 0, app_size.x, app_size.y);

    while (!GetAsyncKeyState(VK_ESCAPE)) {
        app_window.draw(&render);

        app_window.showOn(&users_window);

        this->sleep(30);
    }
};

void App::initWindows() const {
    
};

void App::proceedPress() const {

};