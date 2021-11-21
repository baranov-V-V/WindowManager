#include "Tools.h"
#include "BasicWindow.h"
#include "App.h"
#include <algorithm>
#include <cassert>
#include <iostream>
    
void ToolManager::addTool(VTool* tool) {
    tools.push_back(tool);
};
void ToolManager::delTool(VTool* tool) {

    auto it = std::find(tools.begin(), tools.end(), tool);
    if (it != tools.end()) {
        tools.erase(it);
    }

};
void ToolManager::delLast() {
    tools.pop_back();
};
void ToolManager::setNext() {
    if (curr_tool != tools.size() - 1) {
        curr_tool++;
    } else {
        curr_tool = 0;
    }
};
void ToolManager::setPrev() {
    if (curr_tool != 0) {
        curr_tool--;
    } else {
        curr_tool = tools.size() - 1;
    }
};

void ToolFeather::ProceedPressDown(Texture* target, Renderer* render, int x, int y) {
    old_coord.x = x;
    old_coord.y = y;
};
void ToolFeather::ProceedMove(Texture* target, Renderer* render, int dx, int dy) {
    render->setWindow(reinterpret_cast<BasicWindow*>(target));
    if (old_coord.x + dx > target->getSizeX() || old_coord.y + dy > target->getSizeY()) {
        old_coord.x += dx;
        old_coord.y += dy;
        return;
    }
    render->drawLine(old_coord.x, old_coord.y, old_coord.x + dx, old_coord.y + dy, color, thickness);
    
    old_coord.x += dx;
    old_coord.y += dy;
};

void ToolEraser::ProceedPressDown(Texture* target, Renderer* render, int x, int y) {
    old_coord.x = x;
    old_coord.y = y;
};
void ToolEraser::ProceedMove(Texture* target, Renderer* render, int dx, int dy) {
    render->setWindow(reinterpret_cast<BasicWindow*>(target));
    if (old_coord.x + dx > target->getSizeX() || old_coord.y + dy > target->getSizeY()) {
        old_coord.x += dx;
        old_coord.y += dy;
        return;
    }
    render->drawLine(old_coord.x, old_coord.y, old_coord.x + dx, old_coord.y + dy, white_c, thickness);
    
    old_coord.x += dx;
    old_coord.y += dy;
};

void ToolRect::ProceedPressDown(Texture* target, Renderer* render, int x, int y) {
        old_coord.x = x;
        old_coord.y = y;
        new_coord = old_coord;
};  
void ToolRect::ProceedMove(Texture* target, Renderer* render, int dx, int dy) { 
    new_coord.x += dx;
    new_coord.y += dy;
};
void ToolRect::ProceedPressUp(Texture* target, Renderer* render, int x, int y) {
    if (new_coord.x > 0 && new_coord.x < target->getSizeX() && new_coord.y > 0 && new_coord.y < target->getSizeY()) {
        render->setWindow(reinterpret_cast<BasicWindow*>(target));
        render->drawFilledRectangle(old_coord.x, old_coord.y, new_coord.x, new_coord.y, color, color, 1);
    }
};

Tool1::Tool1(App* app) : VTool(black_c, 1, tool1_name), app(app) {
    SET_OPTION_ADJUST(options);
    adjust_window = MakeGraphWindow(500, 400, app->app_window.getSizeX() / 7, app->app_window.getSizeY() / 7, &(app->app_window), &(app->render), app);
    adjust_window->setShow(false);
};
void Tool1::draw(Texture* target, Renderer* render, int x, int y) {
   for (int ix = x - 30; ix < x + 30; ++ix) {
        for (int iy = y - 30; iy < y + 30; ++iy) {
            if ((ix > 0 && ix < target->getSizeX()) && (iy > 0 && iy < target->getSizeY())) {
                if (((x - ix) * (x - ix) + (y - iy) * (y - iy) < 30 * 30) && !mark[ix][iy]) {
                    mark[ix][iy] = true;
                    COLORREF color = render->getPixel(ix, iy);
                    BYTE new_red = std::min(255, GetRValue(color) + d_red);
                    BYTE new_green = std::min(255, GetGValue(color) + d_green);
                    BYTE new_blue = std::min(255, GetBValue(color) + d_blue);
                    render->setPixel(ix, iy, RGB(new_red, new_green, new_blue));
                }
            }
        }
    } 
}
void Tool1::ProceedPressDown(Texture* target, Renderer* render, int x, int y) {
    mark.resize(target->getSizeX(), vector<bool>(target->getSizeY(), false));
    render->setWindow(target);

    old_coord.x = x;
    old_coord.y = y;

    this->draw(target, render, x, y);
};
void Tool1::ProceedMove(Texture* target, Renderer* render, int dx, int dy) {
    render->setWindow(target);
    if (old_coord.x + dx > target->getSizeX() || old_coord.y + dy > target->getSizeY()) {
        old_coord.x += dx;
        old_coord.y += dy;
        return;
    }
    this->draw(target, render, old_coord.x + dx, old_coord.y + dy);

    old_coord.x += dx;
    old_coord.y += dy;
};
void Tool1::ProceedPressUp(Texture* target, Renderer* render, int x, int y) {
    mark.clear();
};
void Tool1::adjust() {
    adjust_window->setShow(true);
    
};

void ToolModule::ProceedPressDown(Texture* target, Renderer* render, int x, int y) {
    if (press != NULL) {
        press(&funcs, target, render, x, y, info); 
        std::cout << "pressing!";
    }
    std::cout << "kinda press!";
};
void ToolModule::ProceedMove(Texture* target, Renderer* render, int dx, int dy) {
    if (move != NULL) {
        move(&funcs, target, render, dx, dy, info);
    }
};
void ToolModule::ProceedPressUp(Texture* target, Renderer* render, int x, int y) {
    if (release != NULL) {
        release(&funcs, target, render, x, y, info);
    }
};
ToolModule::ToolModule(char* name, ToolFunc press, ToolFunc move, ToolFunc release, void* info) : 
    VTool(black_c, 1, name), press(press), move(move), release(release), info(info) {
    //printf("\n%p", press); 
    funcs = {&RenderDrawLine,
             &RenderSetPixel, 
             &RenderdrawCircle,
             &RenderdrawRectangle,
             &RenderdrawFilledRectangle,
             &RenderdrawRoundRect};
};


ToolModule* LoadTool(char* file_name) {
    HMODULE module = LoadLibraryA(file_name);   
    if (module == NULL) {
        std::cout << "couldn't open file" << file_name << "\n";
        assert(0);
    }

    ToolFunc tool_funcs[3];
    NameFunc func_name = (NameFunc) GetProcAddress(module, func_names[0]);

    //printf("loaded %s!", func_name());

    for (int i = 1; i < func_count; ++i) {
        tool_funcs[i - 1] = (ToolFunc) GetProcAddress(module, func_names[i]);
        if (tool_funcs[i - 1] == NULL) {
            printf("couldn't find function with name %s\n", func_names[i]);
            assert(0);
        }
    }

    //tool_funcs[0](nullptr, nullptr, 0, 0, nullptr);
    ToolModule* tool = new ToolModule(func_name(), tool_funcs[0], tool_funcs[1], tool_funcs[2], nullptr);
    
    return tool;
}