#include "WindowManager.h"

PicWindow* MakeCanvas(int x_size, int y_size, int coord_x, int coord_y, ManagerWindow* parent,
                      Renderer* render, Feather* feather, WindowMouse* mouse) {
    
    PicWindow*   canvas  = new PicWindow(x_size, y_size, coord_x, coord_y, nullptr, img_canvas, parent);
    DrawFunctor* canvas_functor = new DrawFunctor(canvas, render, feather, mouse);
    canvas->setFunctor(canvas_functor);

    PicWindow* menu = MakeBasicMenu(x_size, y_size / comp_y, 0, 0, canvas);
    canvas->addChild(menu);

    return canvas;
}

PicWindow* MakeBasicMenu(int x_size, int y_size, int coord_x, int coord_y, ManagerWindow* parent) {
    int but_x = x_size / comp_x;
    int but_y = y_size;  

    PicWindow* menu  = new PicWindow(x_size, y_size, coord_x, coord_y, nullptr, img_menu_bar, parent);
    PicWindow* hide  = new PicWindow(but_x, but_y, x_size - but_x * 2, 0, nullptr, img_hide, menu);
    PicWindow* close = new PicWindow(but_x, but_y, x_size - but_x * 1, 0, nullptr, img_close, menu);

    CloseFunctor* close_f  = new CloseFunctor(close);    
    HideFunctor*  hide_f   = new HideFunctor(hide); 

    hide->setFunctor(hide_f);
    close->setFunctor(close_f);

    menu->addChild(hide);
    menu->addChild(close);

    return menu;
}

PicWindow* MakePalette(int x_size, int y_size, int coord_x, int coord_y, ManagerWindow* parent) {

};

PicWindow* MakeLayout(int x_size, int y_size, ManagerWindow* parent) {

};
