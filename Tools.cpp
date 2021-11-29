#include "Tools.h"
#include "Window.h"
#include "App.h"
#include "PluginApiClasses.h"
#include <algorithm>
#include <cassert>
#include <iostream>
    

void VTool::adjust() {
    if (adjust_window != nullptr) {
        adjust_window->getParent()->makeFirst(adjust_window);
        adjust_window->setShow(!adjust_window->getShow());
    }
};

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
    if (!tools_menu->getShow()) {
        tools_menu->getParent()->makeFirst(tools_menu);
    }
    tools_menu->setShow(!tools_menu->getShow());
}

void ToolFeather::ProceedPressDown(Texture* target, Renderer* render, int x, int y) {
    //old_coord.x = x;
    //old_coord.y = y;
    render->drawLine(x, y, x, y, color, thickness);
};
void ToolFeather::ProceedMove(Texture* target, Renderer* render, int x, int y, int dx, int dy) {
    render->setWindow(reinterpret_cast<BasicWindow*>(target));
    
    if (x + dx > target->getSizeX() || y + dy > target->getSizeY()) {
        return;
    }
    render->drawLine(x, y, x + dx, y + dy, color, thickness);
    
    /*
    if (old_coord.x + dx > target->getSizeX() || old_coord.y + dy > target->getSizeY()) {
        old_coord.x += dx;
        old_coord.y += dy;
        return;
    }
    render->drawLine(old_coord.x, old_coord.y, old_coord.x + dx, old_coord.y + dy, color, thickness);
    
    old_coord.x += dx;
    old_coord.y += dy;
    */
};

void ToolEraser::ProceedPressDown(Texture* target, Renderer* render, int x, int y) {
    /*
    old_coord.x = x;
    old_coord.y = y;
    */
    render->drawLine(x, y, x + 1, y + 1, white_c, thickness);
};
void ToolEraser::ProceedMove(Texture* target, Renderer* render, int x, int y, int dx, int dy) {
    render->setWindow(reinterpret_cast<BasicWindow*>(target));
    if (x + dx > target->getSizeX() || y + dy > target->getSizeY()) {
        return;
    }
    render->drawLine(x, y, x + dx, y + dy, white_c, thickness);
    /*
    if (old_coord.x + dx > target->getSizeX() || old_coord.y + dy > target->getSizeY()) {
        old_coord.x += dx;
        old_coord.y += dy;
        return;
    }
    render->drawLine(old_coord.x, old_coord.y, old_coord.x + dx, old_coord.y + dy, white_c, thickness);
    
    old_coord.x += dx;
    old_coord.y += dy;
    */
};

void ToolRect::ProceedPressDown(Texture* target, Renderer* render, int x, int y) {
        old_coord.x = x;
        old_coord.y = y;
        new_coord = old_coord;
};  
void ToolRect::ProceedMove(Texture* target, Renderer* render, int x, int y, int dx, int dy) { 
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
    adjust_window = MakeGraphWindow(500, 400, app->app_window.getSizeX() / 7, app->app_window.getSizeY() / 7, &(app->app_window), Renderer::getInstance(), app);
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
void Tool1::ProceedMove(Texture* target, Renderer* render, int x, int y, int dx, int dy) {
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

ToolPlugin::ToolPlugin(plugin::ITool* plugin_tool) : tool(plugin_tool), VTool(white_c, 1, plugin_tool->GetIconFileName()) {
    PreferencesPanel* panel = dynamic_cast<PreferencesPanel*>(tool->GetPreferencesPanel());
    if (panel != nullptr) {
        std::cout << "added tool adjust window!\n";
        this->setAdjustWindow(panel->getLayout());
    }   
};
void ToolPlugin::ProceedPressDown(Texture* target, Renderer* render, int x, int y) {
    RenderTexture texture(target, render);
    tool->ActionBegin(&texture, x, y);
};
void ToolPlugin::ProceedMove(Texture* target, Renderer* render, int x, int y, int dx, int dy) {
    //RenderTexture texture(target, render);
    //tool->Action(texture, )
};
void ToolPlugin::ProceedPressUp(Texture* target, Renderer* render, int x, int y) {
    RenderTexture texture(target, render);
    tool->ActionEnd(&texture, x, y);
};

FilterPlugin::FilterPlugin(plugin::IFilter* plugin_filter) : filter(plugin_filter), VTool(white_c, 1, plugin_filter->GetName()) {
    PreferencesPanel* panel = dynamic_cast<PreferencesPanel*>(filter->GetPreferencesPanel());
    if (panel != nullptr) {
        std::cout << "added filter adjust window!\n";
        this->setAdjustWindow(panel->getLayout());
    }
};
void FilterPlugin::ProceedPressDown(Texture* target, Renderer* render, int x, int y) {
    render->setWindow(target);
    RenderTexture texture(target, render);
    filter->Apply(&texture);
};

void LoadTools(ToolManager* tool_manager, Renderer* render, char* file_name) {
    HMODULE module = LoadLibraryA(file_name);   
    if (module == NULL) {
        std::cout << "couldn't open file" << file_name << "\n";
        assert(0);
    }

    API api;

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
