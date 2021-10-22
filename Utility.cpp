#include "WindowManager.h"

PicWindow* MakeCanvas(int x_size, int y_size, int coord_x, int coord_y, ManagerWindow* parent,
                      Renderer* render, Feather* feather, WindowMouse* mouse) {
    
    PicWindow*   canvas  = new PicWindow(x_size, y_size, coord_x, coord_y, nullptr, img_canvas, parent);
    DrawFunctor* canvas_functor = new DrawFunctor(canvas, render, feather, mouse);
    canvas->setFunctor(canvas_functor);

    PicWindow* menu = MakeBasicMenu(x_size, 5 + y_size / 15, 0, 0, canvas, 10);
    canvas->addChild(menu);

    return canvas;
}

PicWindow* MakeBasicMenu(int x_size, int y_size, int coord_x, int coord_y, ManagerWindow* parent, int comp_x) {
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

PicWindow* MakePalette(int x_size, int y_size, int coord_x, int coord_y, ManagerWindow* parent, Renderer* render, Feather* feather) {

    PicWindow* palette = new PicWindow(x_size, y_size, coord_x, coord_y, nullptr, img_palette, parent);
    
    int c_box_x = 5 * x_size / (6 * x_count_c + 1);
    int c_box_y = c_box_x;
    
    int c_box_dx = x_size / (6 * x_count_c + 1);
    int c_box_dy = x_size / (6 * x_count_c + 1);

    int start_boxes_y = y_size / 8;

    for (int i = 0; i < y_count_c; ++i) {
        for (int j = 0; j < x_count_c; ++j) {
            ChangeColor* change_f   = new ChangeColor(feather, palette_colors[i][j]);
            BorderWindow* color_button = new BorderWindow(c_box_x, c_box_y, palette_colors[i][j], c_box_dx + (c_box_dx + c_box_x) * j, start_boxes_y + (c_box_y + c_box_dy) * i, change_f, render, palette, black_c, 1);
            palette->addChild(color_button);
        }
    }

    int feather_y = start_boxes_y + (c_box_y + c_box_dy) * y_count_c;
    int feather_x = x_size / 9;

    int inst_x = x_size / 4;
    int inst_y = 3 * inst_x / 4;
    int inst_dx = c_box_dx;
    int inst_dy = c_box_dy;

    ChangeMode* change_to_feather = new ChangeMode(feather, MODE_DRAW);
    ChangeMode* change_to_eraser  = new ChangeMode(feather, MODE_ERASE);
    PicWindow* feather_button = new PicWindow(inst_x, inst_y, inst_dx, feather_y                         , change_to_feather, img_feather, palette);
    PicWindow* eraser_button  = new PicWindow(inst_x, inst_y, inst_dx, feather_y + (inst_dy + inst_y) * 1, change_to_eraser , img_eraser , palette); 
    ThicknessWindow* thick_ind_w = new ThicknessWindow(2 * inst_x, inst_y, white_c, 2 * inst_dx + inst_x, feather_y                         , nullptr, render, feather, palette, mgrey_c, 1);
    ThicknessWindow* thick_ind_b = new ThicknessWindow(2 * inst_x, inst_y, black_c, 2 * inst_dx + inst_x, feather_y + (inst_dy + inst_y) * 1, nullptr, render, feather, palette, mgrey_c, 1);

    palette->addChild(thick_ind_w);
    palette->addChild(thick_ind_b);
    palette->addChild(feather_button);
    palette->addChild(eraser_button);

    return palette;
};

PicWindow* MakeLayout(int x_size, int y_size, int coord_x, int coord_y, ManagerWindow* parent, int comp_x) {
    PicWindow* menu = MakeBasicMenu(x_size, y_size, coord_x, coord_y, parent, comp_x);
    
    int file_x = x_size / 34;
    int file_y = y_size;

    int help_x = x_size / 28;    
    int help_y = y_size;

    int clock_x = x_size / 15;
    int clock_y = y_size;

    //file->help->(to be continued...)
    PicWindow* file = new PicWindow(file_x, file_y, 0, 0, nullptr, img_file, menu);
    PicWindow* help = new PicWindow(help_x, help_y, file_x, 0, nullptr, img_help, menu);
    ClockWindow* clock = new ClockWindow(clock_x, clock_y, mgrey_c, x_size - menu->getChild(0)->getSizeX() * menu->getCount() - clock_x, 0, nullptr, menu, mgrey_c, 1);
    
    FileFunctor* file_f  = new FileFunctor(file); 
    HelpFunctor* help_f  = new HelpFunctor(help); 
    
    file->setFunctor(file_f);
    help->setFunctor(help_f);

    menu->addChild(clock);
    menu->addChild(file);
    menu->addChild(help);

    return menu;
};
