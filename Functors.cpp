#include "Functors.h"
#include "Window.h"
#include "Tools.h"
#include "App.h"

#include <cassert>
#include <iostream>

DummyFunctor::DummyFunctor() {};
bool DummyFunctor::action() {
    return true;
};

FeatherFunctor::FeatherFunctor(ToolFeather* feather) : feather(feather), new_color(feather->getColor()), new_thickness(feather->getThickness()) {};
bool FeatherFunctor::action() { 
    feather->setColor(new_color);
    feather->setThickness(new_thickness);
    return true;
};
void FeatherFunctor::setColor(COLORREF color) {
    new_color = color;
};
void FeatherFunctor::setThickness(int thinckness) {
    new_thickness = thinckness;
};

DebugFunctorTrue::DebugFunctorTrue() {};
DebugFunctorTrue::DebugFunctorTrue(ManagerWindow* window) : window(window) {};
bool DebugFunctorTrue::action() {
    return true;
};

InvFunctorTrue::InvFunctorTrue() {};
bool InvFunctorTrue::action() { 
    std::cout << "Click true on invs_wnd\n"; 
    return true; 
};

StopAppFunctor::StopAppFunctor() : app(nullptr) {};
StopAppFunctor::StopAppFunctor(App* app) : app(app) {};
bool StopAppFunctor::action() { 
    app->stop(); 
    exit(0); 
    return true; 
};

DebugFunctorFalse::DebugFunctorFalse() {};
DebugFunctorFalse::DebugFunctorFalse(ManagerWindow* window) : window(window) {};
bool DebugFunctorFalse::action() { 
    /*cout << "Click false on " << window << "\n";*/ 
    return false;
};

ChangeColor::ChangeColor() : tools(nullptr), color(black_c) {};
ChangeColor::ChangeColor(ToolManager* tools, COLORREF color) : tools(tools), color(color) {};
bool ChangeColor::action() { 
    for (int i = 0; i < tools->getCount(); ++i) {
        tools->operator[](i)->setColor(color);
    }
    return true;
};

ChangeBasicTool::ChangeBasicTool() : tools(nullptr) { tool_index = 0; };
ChangeBasicTool::ChangeBasicTool(ToolManager* tools, int tool_index) : tools(tools), tool_index(tool_index) {};
bool ChangeBasicTool::action() {
    tools->setCurrTool(tool_index);
    return true;
};

NextBasicTool::NextBasicTool() : tools(nullptr) {};
NextBasicTool::NextBasicTool(ToolManager* tools) : tools(tools) {};
bool NextBasicTool::action() {
    tools->setNext();
    return true;
};

IncThickness::IncThickness() : tools(nullptr) {};
IncThickness::IncThickness(ToolManager* tools) : tools(tools) {};
bool IncThickness::action() {         
    ToolFeather* feather = reinterpret_cast<ToolFeather*>(tools->operator[](TOOL_FEATHER));
    if (feather->getThickness() < MAX_THICKNESS) {
        feather->setThickness(feather->getThickness() + 1);  
    }
    return true;
};

DecThickness::DecThickness() : tools(nullptr) {};
DecThickness::DecThickness(ToolManager* tools) : tools(tools) {};
bool DecThickness::action() {
    ToolFeather* feather = reinterpret_cast<ToolFeather*>(tools->operator[](TOOL_FEATHER));
    if (feather->getThickness() > MIN_THICKNESS) {
        feather->setThickness(feather->getThickness() - 1);  
    }
    return true;
};


DrawFunctor::DrawFunctor() : render(nullptr), canvas(nullptr), mouse(nullptr), abs_old_coord(0, 0) {};
DrawFunctor::DrawFunctor(ManagerWindow* window, Renderer* render, WindowMouse* mouse, App* app, ToolManager* tool_manager) :
                         render(render), canvas(window), mouse(mouse), app(app), tool_manager(tool_manager) {};
void DrawFunctor::startDraw() {
    is_drawing = true;
    curr_tool = tool_manager->getCurrTool();
    canvas->getParent()->getParent()->makeFirst(canvas->getParent());
    curr_tool->ProceedPressDown(canvas, render, mouse->getRelCoord().x, mouse->getRelCoord().y);
    abs_old_coord = mouse->getAbsCoord();
    app->setActiveWindow(canvas);
}
void DrawFunctor::endDraw() {
    is_drawing = false;
    curr_tool->ProceedPressUp(canvas, render, mouse->getAbsCoord().x, mouse->getAbsCoord().y);
    app->setActiveWindow(nullptr);
}
bool DrawFunctor::action()  {
    //cout << "in draw!" << "\n";
    if (is_drawing) {
        curr_tool->ProceedMove(canvas, render, mouse->getAbsCoord().x - abs_old_coord.x, mouse->getAbsCoord().y - abs_old_coord.y); //dx, dy coordinates
        abs_old_coord = mouse->getAbsCoord();
    }
    return true;
};

StartDraw::StartDraw() {};
StartDraw::StartDraw(DrawFunctor* draw_f) : draw_f(draw_f) {};
bool StartDraw::action() {
    draw_f->startDraw();
    return true;
};

EndDraw::EndDraw() {};
EndDraw::EndDraw(DrawFunctor* draw_f) : draw_f(draw_f) {};
bool EndDraw::action() {
    draw_f->endDraw();
    return true;
};

CloseCanvasFunctor::CloseCanvasFunctor() {};
CloseCanvasFunctor::CloseCanvasFunctor(InvisibleWindow* window) : window_to_close(window) {};
bool CloseCanvasFunctor::action() {
    ManagerWindow* parent = window_to_close->getParent();
    assert(parent);
    parent->delChild(window_to_close);
    return true;
};

HideCanvasFunctor::HideCanvasFunctor() {};
HideCanvasFunctor::HideCanvasFunctor(CanvasWindow* window) : window_to_hide(window) {};
bool HideCanvasFunctor::action()  {
    window_to_hide->hide();
    return true;
};

FileFunctor::FileFunctor() {};
FileFunctor::FileFunctor(ManagerWindow* window, Renderer* render, WindowMouse* mouse, App* app) : window(window), render(render), mouse(mouse), app(app) {};
bool FileFunctor::action() {
    InvisibleWindow* canvas_layer = MakeResizeCanvas(window->getSizeX() / 2, window->getSizeY() / 2, window->getSizeX() / 4, window->getSizeY() / 4, "Canvas", window, render, mouse, app);
    window->addChild(canvas_layer);
    return true;
};

HelpFunctor::HelpFunctor() {};
HelpFunctor::HelpFunctor(ManagerWindow* window) : window(window) {};
bool HelpFunctor::action() {
    /*cout << "Click true on " << window << "\n";*/
    return true;
};

ViewFunctor::ViewFunctor() {};
ViewFunctor::ViewFunctor(ManagerWindow* window) : window(window) {};
bool ViewFunctor::action() {
    /*cout << "Click true on " << window << "\n";*/
    return true;
};

MoveFunctor::MoveFunctor() {};
MoveFunctor::MoveFunctor(ManagerWindow* window, WindowMouse* mouse, App* app, ManagerWindow* activate_window) : 
                         activate_window(activate_window), move_window(window), mouse(mouse), on_move(false), old_coord(0, 0), app(app) {};
void MoveFunctor::startMove() { 
    move_window->getParent()->makeFirst(move_window); 
    on_move = true;
    old_coord = mouse->getAbsCoord();
    if (activate_window) {
        app->setActiveWindow(activate_window);  
    } else {
        app->setActiveWindow(move_window);
    }
    //cout << "started move!";
};
void MoveFunctor::endMove() {
    on_move = false;
    app->setActiveWindow(nullptr);
    //cout << "ended move!";
};
bool MoveFunctor::action() {
    //cout << "try movimg!";
    if (on_move) {
        //cout << "in move!";
        move_window->setCoord(move_window->getCoord() + (mouse->getAbsCoord() - old_coord));
        old_coord = mouse->getAbsCoord();
    }
    return true;
};

StartMove::StartMove() {};
StartMove::StartMove(MoveFunctor* move_f) : move_f(move_f) {};
bool StartMove::action() {
    move_f->startMove();
    return true;
};

EndMove::EndMove() {};
EndMove::EndMove(MoveFunctor* move_f) : move_f(move_f) {};
bool EndMove::action() {
    move_f->endMove();
    return true;
};

RecalcThickness::RecalcThickness(int min_coord, int max_coord, ManagerWindow* bar, ToolManager* tools) : bar(bar), tools(tools), min_coord(min_coord), max_coord(max_coord) {};
bool RecalcThickness::action () {
    ToolFeather* feather = reinterpret_cast<ToolFeather*>(tools->operator[](TOOL_FEATHER));
    feather->setThickness(MIN_THICKNESS + (bar->getCoordX() - min_coord) * (MAX_THICKNESS - MIN_THICKNESS) / (max_coord - min_coord));    
    return true;
};

PlaceBar::PlaceBar() {};
PlaceBar::PlaceBar(int min_coord, int max_coord, char type, ManagerWindow* bar, VFunctor* action) : bar(bar), min_coord(min_coord), max_coord(max_coord), action(action), type(type), is_fixed_pos(false) {};
void PlaceBar::fixPos() {
    is_fixed_pos = true;
};
void PlaceBar::unfixPos() {
    is_fixed_pos = false;
};
void PlaceBar::place(int new_coord) {
    if (!is_fixed_pos) {
        if (new_coord < min_coord) {
            new_coord = min_coord;
        }
        if (new_coord > max_coord) {
            new_coord = max_coord;
        }
        if (type == 'X') {
            bar->setCoordX(new_coord);
        } else if (type == 'Y') {
            bar->setCoordY(new_coord);
        }
        action->action();
    }
}
ManagerWindow* PlaceBar::getBar() const { return bar; };

MoveBarLeft::MoveBarLeft(PlaceBar* placer) : placer(placer) {};
bool MoveBarLeft::action() {
    ManagerWindow* bar = placer->getBar();
    placer->place(bar->getCoordX() - 5);
    return true;
};

MoveBarRight::MoveBarRight(PlaceBar* placer) : placer(placer) {};
bool MoveBarRight::action() {
    ManagerWindow* bar = placer->getBar();
    placer->place(bar->getCoordX() + 5);
    return true;
};

MoveBarUp::MoveBarUp(PlaceBar* placer) : placer(placer) {};
bool MoveBarUp::action() {
    ManagerWindow* bar = placer->getBar();
    placer->place(bar->getCoordY() - 5);
    return true;
};

MoveBarDown::MoveBarDown(PlaceBar* placer) : placer(placer) {};
bool MoveBarDown::action() {
    ManagerWindow* bar = placer->getBar();
    placer->place(bar->getCoordY() + 5);
    return true;
};

PlaceBarOnClickX::PlaceBarOnClickX() {};
PlaceBarOnClickX::PlaceBarOnClickX(PlaceBar* placer, ManagerWindow* window, WindowMouse* mouse, MoveFunctor* functor) : placer(placer), mouse(mouse), StartMove(functor) {};
bool PlaceBarOnClickX::action() {
    ManagerWindow* bar = placer->getBar();
    placer->place(mouse->getRelCoord().x - placer->getBar()->getSizeX() / 2);
    move_f->startMove();
    return true;
};

PlaceBarOnClickY::PlaceBarOnClickY() {};
PlaceBarOnClickY::PlaceBarOnClickY(PlaceBar* placer, ManagerWindow* window, WindowMouse* mouse, MoveFunctor* functor) : placer(placer), mouse(mouse), StartMove(functor) {};
bool PlaceBarOnClickY::action() {
    ManagerWindow* bar = placer->getBar();
    placer->place(mouse->getRelCoord().y - placer->getBar()->getSizeY() / 2);
    move_f->startMove();
    return true;
};

MoveBarRandomX::MoveBarRandomX() {};
MoveBarRandomX::MoveBarRandomX(PlaceBar* placer, ManagerWindow* window, WindowMouse* mouse, App* app) : placer(placer), MoveFunctor(window, mouse, app) {};
bool MoveBarRandomX::action() {
    if (on_move) {
        placer->place(placer->getBar()->getCoordX() + (mouse->getAbsCoord().x - old_coord.x));
        old_coord = mouse->getAbsCoord();
    }
    if (on_move && !mouse->isLeftClick()) {
        on_move = false;
    }
    return true;
};

MoveBarRandomY::MoveBarRandomY() {};
MoveBarRandomY::MoveBarRandomY(PlaceBar* placer, ManagerWindow* window, WindowMouse* mouse, App* app) : placer(placer), MoveFunctor(window, mouse, app) {};
bool MoveBarRandomY::action() {
    if (on_move && mouse->isLeftClick()) {
        placer->place(placer->getBar()->getCoordY() + (mouse->getAbsCoord().y - old_coord.y));
        old_coord = mouse->getAbsCoord();
    }
    if (on_move && !mouse->isLeftClick()) {
        on_move = false;
    }
    return true;
};

GlowPicFunctor::GlowPicFunctor(PicWindow* window, const char* default_name, const char* glowing_name) : glow_window(window),
    default_wnd(window->getSizeX(), window->getSizeY(), default_name, 0, 0),
    glowing_wnd(window->getSizeX(), window->getSizeY(), glowing_name, 0, 0) {
    window->showOn(&default_wnd);
    curr_state = STATE_DEFAULT;
};
bool GlowPicFunctor::action() {
    //cout << "In glow!!" << "\n";
    if (glow_window->isPointed() && curr_state != STATE_GLOWING) {
        //cout << "changed to glow!" << "\n";
        glowing_wnd.showOn(glow_window);
        curr_state = STATE_GLOWING;
    }
    if (!glow_window->isPointed() && curr_state == STATE_GLOWING) {
        //cout << "changed to default!" << "\n";
        default_wnd.showOn(glow_window);
        curr_state = STATE_DEFAULT;
    }
    return true;
};

GlowBorderFunctor::GlowBorderFunctor() {};
GlowBorderFunctor::GlowBorderFunctor(BorderWindow* window, COLORREF glowing_main_c, COLORREF glowing_bord_c) : 
    glowing_main_c(glowing_main_c), glowing_bord_c(glowing_bord_c), glow_window(window) {
    default_main_c = window->getColor();
    default_bord_c = window->getBorderColor();
    curr_state = STATE_DEFAULT;
};
bool GlowBorderFunctor::action() {
    //cout << "In glow!!" << "\n";
    if (glow_window->isPointed() && curr_state != STATE_GLOWING) {
        //cout << "changed to glow!" << "\n";
        glow_window->setColor(glowing_main_c);
        glow_window->setBorderColor(glowing_bord_c);
        glow_window->setRedraw(true);
        curr_state = STATE_GLOWING;
    }
    else if (!glow_window->isPointed() && curr_state == STATE_GLOWING) {
        //cout << "changed to default!" << "\n";
        glow_window->setColor(default_main_c);
        glow_window->setBorderColor(default_bord_c);
        glow_window->setRedraw(true);
        curr_state = STATE_DEFAULT;
    }
    return true;
};

MakeFirst::MakeFirst() {};
MakeFirst::MakeFirst(ManagerWindow* window) : window(window) {};
bool MakeFirst::action() { window->getParent()->makeFirst(window); return true; };

ResizeCanvas::ResizeCanvas() {};
ResizeCanvas::ResizeCanvas(ManagerWindow* window, Renderer* render, WindowMouse* mouse, App* app) : MoveFunctor(window, mouse, app), render(render), direction(DIRECTION_NONE) {};
void ResizeCanvas::startMove() {
    move_window->getParent()->makeFirst(move_window);
    on_move = true;
    old_coord = mouse->getAbsCoord();
    Pair<int> coord = mouse->getRelCoord();
    if (coord.x >= (move_window->getSizeX() - grab_len)) {
        SET_DIR_RIGHT(direction);
    }
    if (coord.x <= grab_len) {
        SET_DIR_LEFT(direction);
    }
    if (coord.y <= grab_len) {
        SET_DIR_UP(direction);
    }
    if (coord.y >= (move_window->getSizeY() - grab_len)) {
        SET_DIR_DOWN(direction);
    }
    //std::cout << "current direction: " << direction;
    app->setActiveWindow(move_window);
};
void ResizeCanvas::endMove() {
    on_move = false;
    CLEAR_DIR(direction);
    app->setActiveWindow(nullptr);
};
bool ResizeCanvas::action() {
    if (on_move && mouse->isLeftClick()) {
        new_size = move_window->getSize();
        new_coord = move_window->getCoord();
        Pair<int> d_mouse_move = mouse->getAbsCoord() - old_coord;
        if (IS_DIR_RIGHT(direction)) {
            new_size.x = move_window->getSizeX() + d_mouse_move.x;
        }
        if (IS_DIR_LEFT(direction)) {
            new_size.x = move_window->getSizeX() - d_mouse_move.x;
            new_coord.x = move_window->getCoordX() + d_mouse_move.x;
        }
        if (IS_DIR_UP(direction)) {
            new_size.y = move_window->getSizeY() - d_mouse_move.y;
            new_coord.y = move_window->getCoordY() + d_mouse_move.y;
        }
        if (IS_DIR_DOWN(direction)) {
            new_size.y = move_window->getSizeY() + d_mouse_move.y;
        }
        if (old_coord != mouse->getAbsCoord()) {
            ResizeCanvasWindow(dynamic_cast<CanvasWindow*>(move_window), render, new_size, new_coord);
        }
        old_coord = mouse->getAbsCoord();
    } else {
      this->endMove();
    }
    return true;
};

CalcGraphDotLeft::CalcGraphDotLeft(int min_coord, int max_coord, ManagerWindow* bar, GraphWindow* window) : window(window), bar(bar), min_coord(min_coord), max_coord(max_coord) {};
bool CalcGraphDotLeft::action () {
    window->setLeftDot((bar->getCoordY() - min_coord) * window->getSizeY() / (max_coord - min_coord)); 
    window->setRedraw(true);
    return true;
}; 

CalcGraphDotRight::CalcGraphDotRight(int min_coord, int max_coord, ManagerWindow* bar, GraphWindow* window) : window(window), bar(bar), min_coord(min_coord), max_coord(max_coord) {};
bool CalcGraphDotRight::action () {
    window->setRightDot((bar->getCoordY() - min_coord) * window->getSizeY() / (max_coord - min_coord)); 
    window->setRedraw(true);
    return true;
}; 
    
ResetBars::ResetBars() {};
ResetBars::ResetBars(PlaceBar* placer_left, PlaceBar* placer_right, int reset_coord_left, int reset_coord_right) : 
                     placer_right(placer_right), placer_left(placer_left), reset_coord_left(reset_coord_left), reset_coord_right(reset_coord_right) {};
bool ResetBars::action() { 
    placer_left->unfixPos(); placer_right->unfixPos();
    placer_left->place(reset_coord_left); placer_right->place(reset_coord_right); 
    return true; 
};

MakeFixedBars::MakeFixedBars() {};
MakeFixedBars::MakeFixedBars(PlaceBar* placer_left, PlaceBar* placer_right) : placer_left(placer_left), placer_right(placer_right) {};
bool MakeFixedBars::action() { placer_right->fixPos(); placer_left->fixPos(); return true; };

MakeUnFixedBars::MakeUnFixedBars() {};
MakeUnFixedBars::MakeUnFixedBars(PlaceBar* placer_left, PlaceBar* placer_right) : placer_left(placer_left), placer_right(placer_right) {};
bool MakeUnFixedBars::action() { placer_right->unfixPos(); placer_left->unfixPos(); return true; };

ClearCanvas::ClearCanvas() {};
ClearCanvas::ClearCanvas(BorderWindow* canvas, Renderer* render) : canvas(canvas), render(render) {};
bool ClearCanvas::action() { render->setWindow(canvas); render->drawFilledRectangle(0, 0, max_canvas_x, max_canvas_y, white_c, white_c, 1); return true; };
