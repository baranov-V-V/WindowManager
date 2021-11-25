#include "Tools.h"
#include "BasicWindow.h"
#include "App.h"
#include "PluginApiClasses.h"
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

void ToolManager::invertShowMenu() {
    tools_menu->setShow(!tools_menu->getShow());
}

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

ToolPlugin::ToolPlugin(plugin::ITool* plugin_tool) : tool(plugin_tool), VTool(white_c, 1, no_name) {};
void ToolPlugin::ProceedPressDown(Texture* target, Renderer* render, int x, int y) {
    RenderTexture texture(target, render);
    tool->ActionBegin(&texture, x, y);
};
void ToolPlugin::ProceedMove(Texture* target, Renderer* render, int dx, int dy) {
    //RenderTexture texture(target, render);
    //tool->Action(texture, )
};
void ToolPlugin::ProceedPressUp(Texture* target, Renderer* render, int x, int y) {
    RenderTexture texture(target, render);
    tool->ActionEnd(&texture, x, y);
};
void ToolPlugin::adjust() {
    tool->GetPreferencesPanel();

    // later will attach it to real canvas;
};

FilterPlugin::FilterPlugin(plugin::IFilter* plugin_filter) : filter(plugin_filter), VTool(white_c, 1, plugin_filter->GetName()) {};
void FilterPlugin::ProceedPressDown(Texture* target, Renderer* render, int x, int y) {
    render->setWindow(target);
    RenderTexture texture(target, render);
    filter->Apply(&texture);
};
void FilterPlugin::adjust() {
    filter->GetPreferencesPanel();
};


void LoadTools(ToolManager* tool_manager, Renderer* render, char* file_name) {
    HMODULE module = LoadLibraryA(file_name);   
    if (module == NULL) {
        std::cout << "couldn't open file" << file_name << "\n";
        assert(0);
    }

    API api(render);

    plugin::CreateFunction create_f = (plugin::CreateFunction) GetProcAddress(module, "Create");
    assert(create_f);

    plugin::DestroyFunction destroy_f = (plugin::DestroyFunction) GetProcAddress(module, "Destroy");
    assert(create_f);

    plugin::VersionFunction version_f = (plugin::VersionFunction) GetProcAddress(module, "Version");
    assert(create_f);

    plugin::IPlugin* plugin = create_f(&api);
    
    std::list<plugin::IFilter*> filter_list = plugin->GetFilters();
    std::list<plugin::ITool*> tool_list = plugin->GetTools();

    for (auto it = tool_list.begin(); it != tool_list.end(); ++it) {
        tool_manager->addTool(new ToolPlugin(*it));
    }

    for (auto it = filter_list.begin(); it != filter_list.end(); ++it) {
        tool_manager->addTool(new FilterPlugin(*it));
    }

    destroy_f(plugin);
}