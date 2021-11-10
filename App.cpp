#include "WindowManager.h"

App::App(int app_x, int app_y) : users_window(app_x, app_y), feather(black_c, 2), app_size(app_x, app_y),
    app_window(app_x, app_y, 0, 0, img_back_font, nullptr, nullptr, nullptr, nullptr, true),
    mouse(static_cast<ManagerWindow*>(&app_window)), render(&app_window, 0, 0, app_x, app_y), on_run(false) {
    this->initWindows();
};
    
void App::run() {
    on_run = true;

    while (on_run) {
        app_window.draw(&render);
        app_window.showOn(&users_window);
        this->proceedMouseEvent();
        //this->sleep(5);
    }
};

void App::initWindows() {
    const int palette_x = app_size.x / 2;
    const int palette_y = 2 * app_size.y / 3;

    //CanvasWindow* canvas   = new CanvasWindow(app_size.x / 2, app_size.y / 2, app_size.x / 4, app_size.y / 4, "Canvas1", &(this->app_window), &render, &feather, &mouse);
    //CanvasWindow* canvas_2 = new CanvasWindow(400, 200, 850, 200, "Canvas2", &(this->app_window), &render, &feather, &mouse);
    InvisibleWindow* canvas_layer = MakeResizeCanvas(app_size.x / 2, app_size.y / 2, app_size.x / 4, app_size.y / 4, "Canvas1", &(this->app_window), &render, &feather, &mouse, this);
    InvisibleWindow* canvas_layer2 = MakeResizeCanvas(app_size.x / 2, app_size.y / 2, app_size.x / 4, app_size.y / 4, "Canvas1", &(this->app_window), &render, &feather, &mouse, this);
    
    //InvisibleWindow* new_canvas_layer = GetResizedCanvas(canvas_layer, &render, &feather, &mouse, {app_size.x / 2 + 20, app_size.y / 2 + 20}, {app_size.x / 4 + 20, app_size.y / 4 + 20});
    //this->app_window.addChild(new_canvas_layer);
    
    BorderWindow* graph = MakeGraphWindow(500, 400, app_size.x / 7, app_size.y / 7, &(this->app_window), &(this->render), &(this->mouse), this);
    
    
    InvFunctorTrue* debug_f = new InvFunctorTrue();
    DedWindow* round_wnd    = new DedWindow(50, 400, 300, app_size.x / 4, app_size.y / 4, silver_c, black_c, 4, &(this->render), &(this->app_window), debug_f);
    PicWindow* menu         = MakeLayout(app_size.x, app_size.y / 23, 0, 0, &(this->app_window), 26, &render, &feather, &mouse, this); //menu->children[0] == close_button;
    PicWindow* palette      = MakePalette(app_size.x / 8, app_size.y / 2, 0, app_size.y / 23, &(this->app_window), &(this->render), &(this->feather), &(this->mouse), this);
    
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
    MakeMovable(round_wnd, round_wnd, &(this->mouse), this);

    this->app_window.addChild(palette);
    this->app_window.addChild(menu);
    this->app_window.addChild(round_wnd);
    this->app_window.addChild(graph);
    this->app_window.addChild(canvas_layer);
    this->app_window.addChild(canvas_layer2);
    //this->app_window.addChild(canvas_2);
};

void App::proceedMouseEvent() {
    static int state = 0;
    static Pair<int> abs_coord(0, 0); 

    mouse.update();
    
    if ((mouse.getState() & LEFT_CLICK) && !(state & LEFT_CLICK)) {
        
        if (active_window != nullptr) {
            active_window->getPressDown()->action();
        } else {
            app_window.proceedPressDown(&mouse);
        }
        
    } else if (!(mouse.getState() & LEFT_CLICK) && (state & LEFT_CLICK)) {
        
        if (active_window != nullptr) {
            active_window->getPressUp()->action();
        } else {
            app_window.proceedPressUp(&mouse);
        }
        
    } else if (mouse.getAbsCoord() != abs_coord) {
        if (active_window != nullptr) {
            active_window->getPointed()->action();
        } else {
            //cout << "sending mouse move to active\n";
            app_window.proceedPointed(&mouse);
        }
        
    }

    state = mouse.getState();
    abs_coord = mouse.getAbsCoord();
};