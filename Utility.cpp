#include "WindowManager.h"

PicWindow* MakeBasicMenu(int x_size, int y_size, int coord_x, int coord_y, ManagerWindow* parent, int comp_x) {
    int but_x = x_size / comp_x;
    int but_y = y_size;  

    PicWindow* menu  = new PicWindow(x_size, y_size, coord_x, coord_y, img_menu_bar, parent);
    PicWindow* hide  = new PicWindow(but_x, but_y, x_size - but_x * 2, 0, img_hide, menu);
    PicWindow* close = new PicWindow(but_x, but_y, x_size - but_x * 1, 0, img_close, menu);

    CloseCanvasFunctor* close_f  = new CloseCanvasFunctor(reinterpret_cast<CanvasWindow*>(parent)); 
    HideCanvasFunctor*  hide_f   = new HideCanvasFunctor(reinterpret_cast<CanvasWindow*>(parent)); 
    
    GlowPicFunctor* close_glow = new GlowPicFunctor(close, img_close2);
    GlowPicFunctor* hide_glow = new GlowPicFunctor(hide, img_hide2);

    hide->setPressUp(hide_f);
    close->setPressUp(close_f);

    hide->setPointed(hide_glow);
    close->setPointed(close_glow);

    menu->addChild(hide);
    menu->addChild(close);

    return menu;
}

PicWindow* MakePalette(int x_size, int y_size, int coord_x, int coord_y, ManagerWindow* parent, Renderer* render, Feather* feather) {

    PicWindow* palette = new PicWindow(x_size, y_size, coord_x, coord_y, img_palette, parent);
    
    int c_box_x = 5 * x_size / (6 * x_count_c + 1);
    int c_box_y = c_box_x;
    
    int c_box_dx = x_size / (6 * x_count_c + 1);
    int c_box_dy = x_size / (6 * x_count_c + 1);

    int start_boxes_y = y_size / 8;

    for (int i = 0; i < y_count_c; ++i) {
        for (int j = 0; j < x_count_c; ++j) {
            ChangeColor* change_f   = new ChangeColor(feather, palette_colors[i][j]);
            BorderWindow* color_button = new BorderWindow(c_box_x, c_box_y, c_box_dx + (c_box_dx + c_box_x) * j, start_boxes_y + (c_box_y + c_box_dy) * i, palette_colors[i][j], black_c, 1, render, palette, change_f);
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
    PicWindow* feather_button = new PicWindow(inst_x, inst_y, inst_dx, feather_y                         ,img_feather, palette, change_to_feather);
    PicWindow* eraser_button  = new PicWindow(inst_x, inst_y, inst_dx, feather_y + (inst_dy + inst_y) * 1,img_eraser , palette, change_to_eraser ); 
    ThicknessWindow* thick_ind_w = new ThicknessWindow(2 * inst_x, inst_y, 2 * inst_dx + inst_x, feather_y                         , white_c, mgrey_c, 1, feather, render, palette);
    ThicknessWindow* thick_ind_b = new ThicknessWindow(2 * inst_x, inst_y, 2 * inst_dx + inst_x, feather_y + (inst_dy + inst_y) * 1, black_c, mgrey_c, 1, feather, render, palette);

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
    PicWindow* file = new PicWindow(file_x, file_y, 0, 0, img_file, menu);
    PicWindow* help = new PicWindow(help_x, help_y, file_x, 0, img_help, menu);
    ClockWindow* clock = new ClockWindow(clock_x, clock_y, x_size - menu->getChild(0)->getSizeX() * menu->getCount() - clock_x, 0, mgrey_c, menu);
    
    FileFunctor* file_f  = new FileFunctor(file); 
    HelpFunctor* help_f  = new HelpFunctor(help); 
    
    file->setPressUp(file_f);
    help->setPressUp(help_f);

    menu->addChild(clock);
    menu->addChild(file);
    menu->addChild(help);

    return menu;
};
