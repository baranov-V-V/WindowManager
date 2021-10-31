#include "WindowManager.h"

PicWindow* MakeBasicMenu(int x_size, int y_size, int coord_x, int coord_y, ManagerWindow* parent, int but_x) {
    int but_y = y_size;  

    PicWindow* menu  = new PicWindow(x_size, y_size, coord_x, coord_y, img_menu_bar, parent);
    PicWindow* hide  = new PicWindow(but_x, but_y, x_size - but_x * 2, 0, img_hide, menu);
    PicWindow* close = new PicWindow(but_x, but_y, x_size - but_x * 1, 0, img_close, menu);

    CloseCanvasFunctor* close_f  = new CloseCanvasFunctor(reinterpret_cast<CanvasWindow*>(parent)); 
    HideCanvasFunctor*  hide_f   = new HideCanvasFunctor(reinterpret_cast<CanvasWindow*>(parent)); 
    
    GlowPicFunctor* close_glow = new GlowPicFunctor(close, img_close, img_close2);
    GlowPicFunctor* hide_glow  = new GlowPicFunctor(hide , img_hide , img_hide2 );

    hide->setPressUp(hide_f);
    close->setPressUp(close_f);

    hide->setPointed(hide_glow);
    close->setPointed(close_glow);

    menu->addChild(close);
    menu->addChild(hide);

    return menu;
}

void MakeMovable(ManagerWindow* activate_wnd, ManagerWindow* move_wnd, WindowMouse* mouse) {
    MoveFunctor* move_f = new MoveFunctor(move_wnd, mouse);
    StartMove* start_move_f = new StartMove(move_f);
    EndMove*   end_move_f   = new EndMove(move_f);

    activate_wnd->setPointed(move_f);
    activate_wnd->setPressDown(start_move_f);
    activate_wnd->setPressUp(end_move_f);
}

PicWindow* MakePalette(int x_size, int y_size, int coord_x, int coord_y, ManagerWindow* parent, Renderer* render, Feather* feather, WindowMouse* mouse) {

    PicWindow* palette = new PicWindow(x_size, y_size, coord_x, coord_y, img_palette, parent);
    MakeMovable(palette, palette, mouse);

    int c_box_x = 5 * x_size / (6 * x_count_c + 1);
    int c_box_y = c_box_x;
    
    int c_box_dx = x_size / (6 * x_count_c + 1);
    int c_box_dy = x_size / (6 * x_count_c + 1);

    int start_boxes_y = y_size / 16;

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
    PicWindow* feather_button = new PicWindow(inst_x, inst_y, inst_dx, feather_y                         , img_feather, palette, change_to_feather);
    PicWindow* eraser_button  = new PicWindow(inst_x, inst_y, inst_dx, feather_y + (inst_dy + inst_y) * 1, img_eraser , palette, change_to_eraser ); 
    ThicknessWindow* thick_ind_w = new ThicknessWindow(2 * inst_x, inst_y, 2 * inst_dx + inst_x, feather_y                         , white_c, mgrey_c, 1, feather, render, palette);
    ThicknessWindow* thick_ind_b = new ThicknessWindow(2 * inst_x, inst_y, 2 * inst_dx + inst_x, feather_y + (inst_dy + inst_y) * 1, black_c, mgrey_c, 1, feather, render, palette);

    GlowPicFunctor* feather_glow = new GlowPicFunctor(feather_button, img_feather, img_feather2);
    GlowPicFunctor* eraser_glow  = new GlowPicFunctor(eraser_button , img_eraser , img_eraser2 );
    feather_button->setPointed(feather_glow);
    eraser_button->setPointed(eraser_glow); 

    int layer_size_x = x_size - 2 * c_box_dx;
    int layer_size_y = 22;
    int arrow_size_x = 22;
    int arrow_size_y = 22;
    InvisibleWindow* layer = new InvisibleWindow(layer_size_x, layer_size_y, inst_dx, feather_y + (inst_dy + inst_y) * 2, palette);
    
    PicWindow* dec_thickness_button = new PicWindow(arrow_size_x, arrow_size_y, 0                          , 0, img_arrow_left , layer);
    PicWindow* inc_thickness_button = new PicWindow(arrow_size_x, arrow_size_y, layer_size_x - arrow_size_x, 0, img_arrow_right, layer);
    BorderWindow* thickness_bar     = new BorderWindow(layer_size_x - 2 * arrow_size_x, arrow_size_y, arrow_size_x, 0, dgrey_c, lgrey_c, 1, render, layer);
    BorderWindow* moving_bar        = new BorderWindow(arrow_size_x - 2, arrow_size_y - 2, arrow_size_x, 1, lgrey_c, lgrey_c, 1, render, layer);

    RecalcThickness* recalc_functor = new RecalcThickness(arrow_size_x, layer_size_x - 2 * arrow_size_x, moving_bar, feather);
    PlaceBar* placer = new PlaceBar(arrow_size_x, layer_size_x - 2 * arrow_size_x, 'X', moving_bar, recalc_functor);
    MoveBarRight* move_right = new MoveBarRight(placer);
    MoveBarLeft*  move_left  = new MoveBarLeft (placer);
    MoveBarRandomX* move_x_axis = new MoveBarRandomX(placer, moving_bar, mouse);
    PlaceBarOnClickX* start_move = new PlaceBarOnClickX(placer, moving_bar, mouse, move_x_axis);
    StartMove* start_move_bar = new StartMove(move_x_axis);
    EndMove* end_move = new EndMove(move_x_axis);

    dec_thickness_button->setPressUp(move_left);
    inc_thickness_button->setPressUp(move_right);
    thickness_bar->setPressDown(start_move);
    moving_bar->setPressDown(start_move_bar);
    moving_bar->setPressUp(end_move);
    moving_bar->setPointed(move_x_axis);

    layer->addChild(dec_thickness_button);
    layer->addChild(inc_thickness_button);
    layer->addChild(thickness_bar);
    layer->addChild(moving_bar);

    palette->addChild(layer);
    palette->addChild(thick_ind_w);
    palette->addChild(thick_ind_b);
    palette->addChild(feather_button);
    palette->addChild(eraser_button);

    return palette;
};

PicWindow* MakeLayout(int x_size, int y_size, int coord_x, int coord_y, ManagerWindow* parent, int comp_x) {
    int but_x = x_size / comp_x;
    int but_y = y_size;  

    PicWindow* menu  = new PicWindow(x_size, y_size, coord_x, coord_y, img_menu_bar, parent);
    PicWindow* hide  = new PicWindow(but_x, but_y, x_size - but_x * 2, 0, img_hide, menu);
    PicWindow* close = new PicWindow(but_x, but_y, x_size - but_x * 1, 0, img_close, menu);

    CloseCanvasFunctor* close_f  = new CloseCanvasFunctor(reinterpret_cast<CanvasWindow*>(parent)); 
    HideCanvasFunctor*  hide_f   = new HideCanvasFunctor(reinterpret_cast<CanvasWindow*>(parent)); 
    
    GlowPicFunctor* close_glow = new GlowPicFunctor(close, img_close, img_close2);
    GlowPicFunctor* hide_glow  = new GlowPicFunctor(hide , img_hide , img_hide2);

    hide->setPressUp(hide_f);
    close->setPressUp(close_f);

    hide->setPointed(hide_glow);
    close->setPointed(close_glow);

    menu->addChild(close);
    menu->addChild(hide);
    
    //PicWindow* menu = MakeBasicMenu(x_size, y_size, coord_x, coord_y, parent, comp_x);
    
    int file_x = x_size / 34;
    int file_y = y_size;

    int help_x = x_size / 28;    
    int help_y = y_size;

    int view_x = help_x;
    int view_y = help_y;
    
    int clock_x = x_size / 15;
    int clock_y = y_size;

    //file->help->(to be continued...)
    PicWindow* file = new PicWindow(file_x, file_y, 0, 0, img_file, menu);
    PicWindow* help = new PicWindow(help_x, help_y, file_x, 0, img_help, menu);
    PicWindow* view = new PicWindow(view_x, view_y, file_x + help_x, 0, img_view, menu);
    ClockWindow* clock = new ClockWindow(clock_x, clock_y, x_size - menu->getChild(0)->getSizeX() * menu->getCount() - clock_x, 0, mgrey_c, menu);
    
    FileFunctor* file_f  = new FileFunctor(file); 
    HelpFunctor* help_f  = new HelpFunctor(help); 
    ViewFunctor* view_f  = new ViewFunctor(view); 

    GlowPicFunctor* file_glow = new GlowPicFunctor(file, img_file, img_file2);
    GlowPicFunctor* help_glow = new GlowPicFunctor(help, img_help, img_help2);
    GlowPicFunctor* view_glow = new GlowPicFunctor(view, img_view, img_view2);
    
    file->setPressUp(file_f);
    help->setPressUp(help_f);
    help->setPressUp(view_f);
    file->setPointed(file_glow);
    help->setPointed(help_glow);
    view->setPointed(view_glow);

    menu->addChild(clock);
    menu->addChild(file);
    menu->addChild(help);
    menu->addChild(view);

    return menu;
};

void MakeResizeable(InvisibleWindow* canvas_layer) {

}

void ReplaceFunctors(ManagerWindow* lhs, ManagerWindow* rhs) {
    lhs->setPointed(rhs->getPointed());
    lhs->setPressDown(rhs->getPressDown());
    lhs->setPressUp(rhs->getPressUp());
}

InvisibleWindow* MakeResizeCanvas(int size_x, int size_y, int coord_x, int coord_y, char* name, ManagerWindow* parent, Renderer* render, Feather* feather, WindowMouse* mouse) {
    InvisibleWindow* canvas_layer = new InvisibleWindow(size_x + 2 * grab_len, size_y + 2 * grab_len, coord_x - grab_len, coord_y - grab_len, parent);
    CanvasWindow* canvas = new CanvasWindow(size_x, size_y, grab_len, grab_len, name, canvas_layer, render, feather, mouse, img_canvas);
    
    InvFunctorTrue* invs_f = new InvFunctorTrue();
    canvas_layer->setPressUp(invs_f);
    // invest resize functors here

    canvas_layer->addChild(canvas);

    return canvas_layer;
}

InvisibleWindow* MakeStaticCanvas(int size_x, int size_y, int coord_x, int coord_y, char* name, ManagerWindow* parent, Renderer* render, Feather* feather, WindowMouse* mouse) {
    InvisibleWindow* canvas_layer = new InvisibleWindow(size_x, size_y, coord_x, coord_y, parent);
    CanvasWindow* canvas = new CanvasWindow(size_x, size_y, 0, 0, name, canvas_layer, render, feather, mouse);

    canvas_layer->addChild(canvas);

    return canvas_layer;
}

InvisibleWindow* GetResizedCanvas(InvisibleWindow* canvas_layer, Renderer* render, Feather* feather, WindowMouse* mouse, const Pair<int>& new_size, const Pair<int>& new_coord) {
    CanvasWindow* canvas = dynamic_cast<CanvasWindow*>(canvas_layer->getChild(0));

    CanvasWindow* new_canvas = new CanvasWindow(new_size.x - 2 * grab_len, new_size.y - 2 * grab_len, grab_len, grab_len, canvas, render, feather, mouse);
    InvisibleWindow* new_canvas_layer = new InvisibleWindow(new_size.x, new_size.y, new_coord.x, new_coord.y, canvas_layer->getParent());
    new_canvas_layer->addChild(new_canvas);

    ReplaceFunctors(new_canvas, canvas);
    ReplaceFunctors(new_canvas_layer, canvas_layer);

    //delete canvas_layer;
    return new_canvas_layer;
}