#include "WindowManager.h"

App::App(int app_x, int app_y) : users_window(app_x, app_y), feather(black_c, 2), app_size(app_x, app_y),
    app_window(app_x, app_y, white_c, 0, 0, nullptr, nullptr, black_c, 5),
    mouse(static_cast<ManagerWindow*>(&app_window)), render(&app_window, 0, 0, app_x, app_y) {};
    
void App::run() {
    this->initWindows();

    render.setWindow(app_window.getChild(2));
    //render.drawCircle(100, 100, 30, black_c, 5);
    
    while (!GetAsyncKeyState(VK_ESCAPE)) {
        app_window.draw(&render);
        render.setWindow(app_window.getChild(0));
        render.drawCircle(20, 20, 30, black_c, 5);
        app_window.showOn(&users_window);
        this->proceedMouseEvent();
        this->sleep(30);
    }
};

void App::initWindows() {
    App* no_const_this = const_cast<App*>(this);

    const int palette_x = app_size.x / 15;
    const int palette_y = app_size.y;

    BorderWindow* canvas  = new BorderWindow(400, 300, white_c, 400, 200, nullptr, &(no_const_this->app_window), black_c, 3);
    BorderWindow* menu    = new BorderWindow(app_size.x - palette_x, app_size.y / 10, white_c, palette_x, 0, nullptr, &(no_const_this->app_window), black_c, 3);
    BorderWindow* palette = new BorderWindow(palette_x, palette_y - app_size.y / 10, white_c, 0, app_size.y / 10, nullptr, &(no_const_this->app_window), black_c, 3);

    DrawFunctor* canvas_functor = new DrawFunctor(canvas, &render, &feather, &mouse);
    DebugFunctorTrue* menu_functor = new DebugFunctorTrue(menu);
    DebugFunctorTrue* palette_functor = new DebugFunctorTrue(palette);
    DebugFunctorTrue* app_window_functor = new DebugFunctorTrue(&app_window);
    

    canvas->setFunctor(canvas_functor);
    menu->setFunctor(menu_functor);
    palette->setFunctor(palette_functor);
    app_window.setFunctor(app_window_functor);

    no_const_this->app_window.addChild(palette);
    no_const_this->app_window.addChild(menu);
    no_const_this->app_window.addChild(canvas);

    const int c_box_x = palette_x / 3;
    const int c_box_y = palette_x / 3;
    
    ChangeColor* change_to_red   = new ChangeColor(&feather, red_c);
    ChangeColor* change_to_blue  = new ChangeColor(&feather, blue_c);
    ChangeColor* change_to_grey  = new ChangeColor(&feather, grey_c);
    ChangeColor* change_to_black = new ChangeColor(&feather, black_c);

    //row 1 of colors
    BorderWindow* palette_red   = new BorderWindow(c_box_x, c_box_y, red_c  , c_box_x / 3                , palette_y / 4 + (c_box_y + c_box_x / 3) * 0, change_to_red , palette, black_c, 1);
    BorderWindow* palette_blue  = new BorderWindow(c_box_x, c_box_y, blue_c , 2 * (c_box_x / 3) + c_box_x, palette_y / 4 + (c_box_y + c_box_x / 3) * 0, change_to_blue, palette, black_c, 1);
    
    //row 2 of colors;
    BorderWindow* palette_grey  = new BorderWindow(c_box_x, c_box_y, grey_c , c_box_x / 3                , palette_y / 4 + (c_box_y + c_box_x / 3) * 1, change_to_grey , palette, black_c, 1);
    BorderWindow* palette_black = new BorderWindow(c_box_x, c_box_y, black_c, 2 * (c_box_x / 3) + c_box_x, palette_y / 4 + (c_box_y + c_box_x / 3) * 1, change_to_black, palette, black_c, 1);

    palette->addChild(palette_red);
    palette->addChild(palette_blue);
    palette->addChild(palette_black);
    palette->addChild(palette_grey);
    
};

void App::proceedMouseEvent() {
    mouse.update();
    if(!app_window.proceedClicks(&mouse)) {
        //cout << "Click with info wasn't taken by any window\n";
        //mouse.printState(); 
    };
};