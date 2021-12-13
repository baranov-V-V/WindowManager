#include <cassert>
#include <iostream>
#include <mutex>

#include "Functors.h"
#include "Window.h"
#include "Tools.h"
#include "Slider.h"
#include "App.h"

extern ResizeCursors cursors;

DummyFunctor::DummyFunctor() {};
bool DummyFunctor::action(const EventData& data) {
    return true;
};

FeatherFunctor::FeatherFunctor(ToolFeather* feather) : feather(feather), new_color(feather->getColor()), new_thickness(feather->getThickness()) {};
bool FeatherFunctor::action(const EventData& data) { 
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
bool DebugFunctorTrue::action(const EventData& data) {
    //std::cout << "click in debug_f!\n";
    return true;
};

InvFunctorTrue::InvFunctorTrue() {};
bool InvFunctorTrue::action(const EventData& data) { 
    std::cout << "Right CLick registered\n"; 
    return true; 
};

StopAppFunctor::StopAppFunctor() : app(nullptr) {};
StopAppFunctor::StopAppFunctor(App* app) : app(app) {};
bool StopAppFunctor::action(const EventData& data) { 
    app->stop(); 
    return true; 
};

DebugFunctorFalse::DebugFunctorFalse() {};
DebugFunctorFalse::DebugFunctorFalse(ManagerWindow* window) : window(window) {};
bool DebugFunctorFalse::action(const EventData& data) { 
    /*cout << "Click false on " << window << "\n";*/ 
    return false;
};

ChangeColor::ChangeColor() : tools(nullptr), color(black_c) {};
ChangeColor::ChangeColor(ToolManager* tools, COLORREF color) : tools(tools), color(color) {};
bool ChangeColor::action(const EventData& data) { 
    for (int i = 0; i < tools->getCount(); ++i) {
        if (IS_OPTION_COLOR(tools->operator[](i)->getOption())) {
            tools->operator[](i)->setColor(color);tools->operator[](i)->setColor(color);
        }
    }
    return true;
};

ChangeToolTrue::ChangeToolTrue() : tools(nullptr) { tool_index = 0; };
ChangeToolTrue::ChangeToolTrue(ToolManager* tools, int tool_index) : tools(tools), tool_index(tool_index) {};
bool ChangeToolTrue::action(const EventData& data) {
    tools->setCurrTool(tool_index);
    return true;
};

ChangeToolFalse::ChangeToolFalse() : tools(nullptr) { tool_index = 0; };
ChangeToolFalse::ChangeToolFalse(ToolManager* tools, int tool_index) : tools(tools), tool_index(tool_index) {};
bool ChangeToolFalse::action(const EventData& data) {
    //std::cout << "clicked!\n";
    tools->setCurrTool(tool_index);
    return false;
};

NextBasicTool::NextBasicTool() : tools(nullptr) {};
NextBasicTool::NextBasicTool(ToolManager* tools) : tools(tools) {};
bool NextBasicTool::action(const EventData& data) {
    tools->setNext();
    return true;
};

IncThickness::IncThickness() : tools(nullptr) {};
IncThickness::IncThickness(ToolManager* tools) : tools(tools) {};
bool IncThickness::action(const EventData& data) {         
    ToolFeather* feather = reinterpret_cast<ToolFeather*>(tools->operator[](TOOL_FEATHER));
    if (feather->getThickness() < MAX_THICKNESS) {
        feather->setThickness(feather->getThickness() + 1);  
    }
    return true;
};

DecThickness::DecThickness() : tools(nullptr) {};
DecThickness::DecThickness(ToolManager* tools) : tools(tools) {};
bool DecThickness::action(const EventData& data) {
    ToolFeather* feather = reinterpret_cast<ToolFeather*>(tools->operator[](TOOL_FEATHER));
    if (feather->getThickness() > MIN_THICKNESS) {
        feather->setThickness(feather->getThickness() - 1);  
    }
    return true;
};


DrawFunctor::DrawFunctor() : render(nullptr), canvas(nullptr), abs_old_coord(0, 0) {};
DrawFunctor::DrawFunctor(ManagerWindow* window, Renderer* render, App* app, ToolManager* tool_manager) :
                         render(render), canvas(window), app(app), tool_manager(tool_manager) {};
void DrawFunctor::startDraw(const EventData& data) {
    is_drawing = true;
    
    curr_tool = tool_manager->getCurrTool();
    canvas->getParent()->getParent()->makeFirst(canvas->getParent());

    Pair<int> rel_coord = canvas->toRelCoord(data.abs_coord);
    curr_tool->ProceedPressDown(canvas, render, rel_coord.x, rel_coord.y);
    
    abs_old_coord = data.abs_coord;
    app->setActiveWindow(canvas);
}
void DrawFunctor::endDraw(const EventData& data) {
    is_drawing = false;
    
    Pair<int> rel_coord = canvas->toRelCoord(data.abs_coord);
    curr_tool->ProceedPressUp(canvas, render, rel_coord.x, rel_coord.y);
    
    app->setActiveWindow(nullptr);
}
bool DrawFunctor::action(const EventData& data)  {
    //cout << "in draw!" << "\n";
    cursors.SetCurrCursor(CURSOR_CROSS);

    if (is_drawing) {
        Pair<int> rel_coord = canvas->toRelCoord(data.mouse_data.start);
        curr_tool->ProceedMove(canvas, render, rel_coord.x, rel_coord.y, data.mouse_data.d.x, data.mouse_data.d.y); //dx, dy coordinates
        //abs_old_coord = mouse->getAbsCoord();
    }
    return true;
};

StartDraw::StartDraw() {};
StartDraw::StartDraw(DrawFunctor* draw_f) : draw_f(draw_f) {};
bool StartDraw::action(const EventData& data) {
    draw_f->startDraw(data);
    return true;
};

EndDraw::EndDraw() {};
EndDraw::EndDraw(DrawFunctor* draw_f) : draw_f(draw_f) {};
bool EndDraw::action(const EventData& data) {
    draw_f->endDraw(data);
    return true;
};

CloseCanvasFunctor::CloseCanvasFunctor() {};
CloseCanvasFunctor::CloseCanvasFunctor(CanvasWindow* window, DisplayManager* canvas_manager) : window_to_close(window), canvas_manager(canvas_manager) {};
bool CloseCanvasFunctor::action(const EventData& data) {
    ManagerWindow* parent = window_to_close->getParent();
    assert(parent);
    canvas_manager->delWindow(window_to_close);

    parent->delChild(window_to_close);
    //std::cout << "canvas_count: " << canvas_manager->getCount() << "\n";
    
    canvas_manager->updateCanvasMenu();
    
    return true;
};

SetShowFunctor::SetShowFunctor() {};
SetShowFunctor::SetShowFunctor(ManagerWindow* window, bool is_shown) : window_to_hide(window), is_shown(is_shown) {};
bool SetShowFunctor::action(const EventData& data)  {
    window_to_hide->setShow(is_shown);
    //std::cout << "done!\n";
    return true;
};

HideAllCanvasFunctor::HideAllCanvasFunctor() {};
HideAllCanvasFunctor::HideAllCanvasFunctor(DisplayManager* canvas_manager) : canvas_manager(canvas_manager) {};
bool HideAllCanvasFunctor::action(const EventData& data) {
    canvas_manager->hideAll();
    std::cout << "done!\n";
    return false;
};

ShowAllCanvasFunctor::ShowAllCanvasFunctor() {};
ShowAllCanvasFunctor::ShowAllCanvasFunctor(DisplayManager* canvas_manager) : canvas_manager(canvas_manager) {};
bool ShowAllCanvasFunctor::action(const EventData& data) {
    canvas_manager->showAll();
    return false;
};

FileFunctor::FileFunctor() {};
FileFunctor::FileFunctor(ManagerWindow* window, Renderer* render, App* app) : window(window), render(render), app(app) {};
bool FileFunctor::action(const EventData& data) {
    CanvasWindow* canvas_layer = MakeResizeCanvas(window->getSizeX() / 2, window->getSizeY() / 2, window->getSizeX() / 4, window->getSizeY() / 4, "Canvas", window, render, app);
    window->addChild(canvas_layer);
    app->getCanvasManager()->addWindow(canvas_layer);
    app->getCanvasManager()->updateCanvasMenu();
    return true;
};

HelpFunctor::HelpFunctor() {};
HelpFunctor::HelpFunctor(ManagerWindow* window) : window(window) {};
bool HelpFunctor::action(const EventData& data) {
    /*cout << "Click true on " << window << "\n";*/
    return true;
};

ViewFunctor::ViewFunctor() {};
ViewFunctor::ViewFunctor(ManagerWindow* window) : window(window) {};
bool ViewFunctor::action(const EventData& data) {
    /*cout << "Click true on " << window << "\n";*/
    return true;
};

MoveFunctor::MoveFunctor() {};
MoveFunctor::MoveFunctor(ManagerWindow* window, App* app, ManagerWindow* activate_window) : 
                         activate_window(activate_window), move_window(window), on_move(false), old_coord(0, 0), app(app) {};
void MoveFunctor::startMove(const EventData& data) {
    move_window->getParent()->makeFirst(move_window); 
    
    on_move = true;
    old_coord = data.abs_coord;
    
    if (activate_window) {
        app->setActiveWindow(activate_window);  
    } else {
        app->setActiveWindow(move_window);
    }

    //cout << "started move!";
};
void MoveFunctor::endMove(const EventData& data) {
    on_move = false;
    app->setActiveWindow(nullptr);

    //cout << "ended move!";
};
bool MoveFunctor::action(const EventData& data) {
    //cout << "try movimg!";
    if (on_move) {
        //cout << "in move!";
        move_window->setCoord(move_window->getCoord() + data.mouse_data.d);
        old_coord = old_coord + data.mouse_data.d;
    }
    return true;
};

StartMove::StartMove() {};
StartMove::StartMove(MoveFunctor* move_f) : move_f(move_f) {};
bool StartMove::action(const EventData& data) {
    move_f->startMove(data);
    return true;
};

EndMove::EndMove() {};
EndMove::EndMove(MoveFunctor* move_f) : move_f(move_f) {};
bool EndMove::action(const EventData& data) {
    move_f->endMove(data);
    return true;
};

RecalcThickness::RecalcThickness(int min_coord, int max_coord, ManagerWindow* bar, ToolManager* tools) : bar(bar), tools(tools), min_coord(min_coord), max_coord(max_coord) {};
bool RecalcThickness::action(const EventData& data) {
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
void PlaceBar::place(const EventData& data, int new_coord) {
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
        action->action(data);
    }
}
ManagerWindow* PlaceBar::getBar() const { return bar; };

MoveBarLeft::MoveBarLeft(PlaceBar* placer) : placer(placer) {};
bool MoveBarLeft::action(const EventData& data) {
    ManagerWindow* bar = placer->getBar();
    placer->place(data, bar->getCoordX() - 5);
    return true;
};

MoveBarRight::MoveBarRight(PlaceBar* placer) : placer(placer) {};
bool MoveBarRight::action(const EventData& data) {
    ManagerWindow* bar = placer->getBar();
    placer->place(data, bar->getCoordX() + 5);
    return true;
};

MoveBarUp::MoveBarUp(PlaceBar* placer) : placer(placer) {};
bool MoveBarUp::action(const EventData& data) {
    ManagerWindow* bar = placer->getBar();
    placer->place(data, bar->getCoordY() - 5);
    return true;
};

MoveBarDown::MoveBarDown(PlaceBar* placer) : placer(placer) {};
bool MoveBarDown::action(const EventData& data) {
    ManagerWindow* bar = placer->getBar();
    placer->place(data, bar->getCoordY() + 5);
    return true;
};

PlaceBarOnClickX::PlaceBarOnClickX() {};
PlaceBarOnClickX::PlaceBarOnClickX(PlaceBar* placer, ManagerWindow* window, MoveFunctor* functor) : placer(placer), StartMove(functor), window(window) {};
bool PlaceBarOnClickX::action(const EventData& data) {
    ManagerWindow* bar = placer->getBar();

    Pair<int> rel_coord = window->toRelCoord(data.abs_coord);

    placer->place(data, rel_coord.x - placer->getBar()->getSizeX() / 2);
    move_f->startMove(data);
    return true;
};

PlaceBarOnClickY::PlaceBarOnClickY() {};
PlaceBarOnClickY::PlaceBarOnClickY(PlaceBar* placer, ManagerWindow* window, MoveFunctor* functor) : placer(placer), StartMove(functor), window(window) {};
bool PlaceBarOnClickY::action(const EventData& data) {
    ManagerWindow* bar = placer->getBar();

    Pair<int> rel_coord = window->toRelCoord(data.abs_coord);

    placer->place(data, rel_coord.y - placer->getBar()->getSizeY() / 2);
    
    move_f->startMove(data);
    return true;
};

MoveBarRandomX::MoveBarRandomX() {};
MoveBarRandomX::MoveBarRandomX(PlaceBar* placer, ManagerWindow* window, App* app) : placer(placer), MoveFunctor(window, app) {};
bool MoveBarRandomX::action(const EventData& data) {
    if (on_move) {
        placer->place(data, placer->getBar()->getCoordX() + data.mouse_data.d.x);
    }
    return true;
};

MoveBarRandomY::MoveBarRandomY() {};
MoveBarRandomY::MoveBarRandomY(PlaceBar* placer, ManagerWindow* window, App* app) : placer(placer), MoveFunctor(window, app) {};
bool MoveBarRandomY::action(const EventData& data) {
    if (on_move) {
        placer->place(data, placer->getBar()->getCoordY() + data.mouse_data.d.y);
    }
    return true;
};

GlowPicFunctor::GlowPicFunctor(PicWindow* window, const char* default_name, const char* glowing_name) : glow_window(window),
    default_wnd(window->getSizeX(), window->getSizeY(), default_name, 0, 0),
    glowing_wnd(window->getSizeX(), window->getSizeY(), glowing_name, 0, 0) {
    window->showOn(&default_wnd);
    curr_state = STATE_DEFAULT;
};
bool GlowPicFunctor::action(const EventData& data) {
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
    return false;
};

GlowBorderFunctor::GlowBorderFunctor() {};
GlowBorderFunctor::GlowBorderFunctor(BorderWindow* window, COLORREF glowing_main_c, COLORREF glowing_bord_c) : 
    glowing_main_c(glowing_main_c), glowing_bord_c(glowing_bord_c), glow_window(window) {
    default_main_c = window->getColor();
    default_bord_c = window->getBorderColor();
    curr_state = STATE_DEFAULT;
};
bool GlowBorderFunctor::action(const EventData& data) {
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
    return false;
};

MakeFirst::MakeFirst() {};
MakeFirst::MakeFirst(ManagerWindow* window) : window(window) {};
bool MakeFirst::action(const EventData& data) { window->getParent()->makeFirst(window); return true; };

ResizeCanvas::ResizeCanvas() {};
ResizeCanvas::ResizeCanvas(ManagerWindow* window, Renderer* render, App* app) : MoveFunctor(window, app), render(render), direction(DIRECTION_NONE) {};
void ResizeCanvas::startMove(const EventData& data) {
    
    move_window->getParent()->makeFirst(move_window);
    on_move = true;

    old_coord = data.abs_coord;
    Pair<int> coord = move_window->toRelCoord(data.abs_coord);
    
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
void ResizeCanvas::endMove(const EventData& data) {
    on_move = false;
    CLEAR_DIR(direction);

    app->setActiveWindow(nullptr);
};
void ResizeCanvas::changeCursor(Pair<int> rel_coord) {
    
    CLEAR_DIR(direction);

    if (rel_coord.x >= (move_window->getSizeX() - grab_len)) {
        SET_DIR_RIGHT(direction);
    }
    if (rel_coord.x <= grab_len) {
        SET_DIR_LEFT(direction);
    }
    if (rel_coord.y <= grab_len) {
        SET_DIR_UP(direction);
    }
    if (rel_coord.y >= (move_window->getSizeY() - grab_len)) {
        SET_DIR_DOWN(direction);
    }

    cursors.setResizeCursor(direction);

    //std::cout << "changed dir to " << direction << "\n";
};
bool ResizeCanvas::action(const EventData& data) {
    if (!on_move) {
        this->changeCursor(move_window->toRelCoord(data.mouse_data.start));
    }

    if (on_move) {
        cursors.setResizeCursor(direction);
        new_size = move_window->getSize();
        new_coord = move_window->getCoord();
        Pair<int> d_mouse_move = data.mouse_data.d;
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
        if (d_mouse_move.x != 0 || d_mouse_move.y != 0) {
            ResizeCanvasWindow(dynamic_cast<CanvasWindow*>(move_window), render, new_size, new_coord);
        }
    } 
    //else {
    //  this->endMove(data);
    //}
    return true;
};

CalcGraphDotLeft::CalcGraphDotLeft(int min_coord, int max_coord, ManagerWindow* bar, GraphWindow* window) : window(window), bar(bar), min_coord(min_coord), max_coord(max_coord) {};
bool CalcGraphDotLeft::action(const EventData& data) {
    window->setLeftDot((bar->getCoordY() - min_coord) * window->getSizeY() / (max_coord - min_coord)); 
    window->setRedraw(true);
    return true;
}; 

CalcGraphDotRight::CalcGraphDotRight(int min_coord, int max_coord, ManagerWindow* bar, GraphWindow* window) : window(window), bar(bar), min_coord(min_coord), max_coord(max_coord) {};
bool CalcGraphDotRight::action(const EventData& data) {
    window->setRightDot((bar->getCoordY() - min_coord) * window->getSizeY() / (max_coord - min_coord)); 
    window->setRedraw(true);
    return true;
}; 
    
ResetBars::ResetBars() {};
ResetBars::ResetBars(PlaceBar* placer_left, PlaceBar* placer_right, int reset_coord_left, int reset_coord_right) : 
                     placer_right(placer_right), placer_left(placer_left), reset_coord_left(reset_coord_left), reset_coord_right(reset_coord_right) {};
bool ResetBars::action(const EventData& data) { 
    placer_left->unfixPos();
    placer_right->unfixPos();
    placer_left->place(data, reset_coord_left);
    placer_right->place(data, reset_coord_right); 
    return true; 
};

MakeFixedBars::MakeFixedBars() {};
MakeFixedBars::MakeFixedBars(PlaceBar* placer_left, PlaceBar* placer_right) : placer_left(placer_left), placer_right(placer_right) {};
bool MakeFixedBars::action(const EventData& data) {
    placer_right->fixPos();
    placer_left->fixPos();
    return true;
};

MakeUnFixedBars::MakeUnFixedBars() {};
MakeUnFixedBars::MakeUnFixedBars(PlaceBar* placer_left, PlaceBar* placer_right) : placer_left(placer_left), placer_right(placer_right) {};
bool MakeUnFixedBars::action(const EventData& data) { 
    placer_right->unfixPos(); 
    placer_left->unfixPos(); 
    return true;
};

ClearCanvas::ClearCanvas() {};
ClearCanvas::ClearCanvas(BorderWindow* canvas, Renderer* render) : canvas(canvas), render(render) {};
bool ClearCanvas::action(const EventData& data) {
    render->setWindow(canvas);
    render->drawFilledRectangle(0, 0, max_canvas_x, max_canvas_y, white_c, white_c, 1); 
    return true; 
};

SetHideFunctor::SetHideFunctor() {};
SetHideFunctor::SetHideFunctor(ManagerWindow* window, bool is_shown) : window(window), is_shown(is_shown) {};
bool SetHideFunctor::action(const EventData& data) {
    //std::cout << "hiding menu!\n";
    window->setShow(!window->getShow());
    return true; 
};

ShowCanvasMenuFunctor::ShowCanvasMenuFunctor() {};
ShowCanvasMenuFunctor::ShowCanvasMenuFunctor(DisplayManager* canvas_manager) : canvas_manager(canvas_manager) {};
bool ShowCanvasMenuFunctor::action(const EventData& data) {
    //std::cout << "canvas menu show functor!\n";
    canvas_manager->invertShowMenu();
    return true;
};

ShowToolMenuFunctor::ShowToolMenuFunctor() {};
ShowToolMenuFunctor::ShowToolMenuFunctor(ToolManager* tool_manager) : tool_manager(tool_manager) {};
bool ShowToolMenuFunctor::action(const EventData& data) {
    tool_manager->invertShowMenu();
    return true;
};

ClickCallbackFunctor::ClickCallbackFunctor() {};
ClickCallbackFunctor::ClickCallbackFunctor(plugin::IClickCallback* call_back) : call_back(call_back) {};
bool ClickCallbackFunctor::action(const EventData& data) {
    call_back->RespondOnClick();
    return true;
};

AdjustVToolFunctor::AdjustVToolFunctor() {};
AdjustVToolFunctor::AdjustVToolFunctor(VTool* tool) : tool(tool) {};
bool AdjustVToolFunctor::action(const EventData& data) {
    //std::cout << "in adjust functor\n";
    tool->adjust();
    return true;
};

MoveBarX::MoveBarX() {};
MoveBarX::MoveBarX(VSlider* slider) : slider(slider), MoveFunctor(slider->getBar(), App::getInstance()) {};
bool MoveBarX::action(const EventData& data) {
    if (on_move) {
        slider->place(slider->getCoord() + data.mouse_data.d.x);
    }
    return true;
}

MoveSliderBarLeft::MoveSliderBarLeft(SliderX* slider) : slider(slider) {};
bool MoveSliderBarLeft::action(const EventData& data) {
    slider->moveBarLeft();
    return true;
};

MoveSliderBarRight::MoveSliderBarRight(SliderX* slider) : slider(slider) {};;
bool MoveSliderBarRight::action(const EventData& data) {
    slider->moveBarRight();
    return true;
};

PlaceSliderBarOnClickX::PlaceSliderBarOnClickX(SliderX* slider) : slider(slider) {};
bool PlaceSliderBarOnClickX::action(const EventData& data) {
    Pair<int> rel_coord = slider->getChild(2)->toRelCoord(data.abs_coord);

    slider->place(rel_coord.x - slider->getBar()->getSizeX() / 2);
    slider->getBar()->getPressDown()->action(data);
    return true;
}

ChangeColorCallbackFunctor::ChangeColorCallbackFunctor() {};
ChangeColorCallbackFunctor::ChangeColorCallbackFunctor(plugin::Palette* palette, COLORREF color) : color(color), palette(palette) {};
bool ChangeColorCallbackFunctor::action(const EventData& data) {
    palette->ChangeColor(plugin::ToColor_t(color));
    return true;
};