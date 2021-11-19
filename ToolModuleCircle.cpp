#define EXPORT_TOOL
#include "ToolModule.h"

extern "C" TOOLAPI void TOOLCALL Move(StandartModuleFuncs* function, Texture* target, Renderer* render, int x, int y, void* info) {

};

extern "C" TOOLAPI void TOOLCALL Press(StandartModuleFuncs* function, Texture* target, Renderer* render, int x, int y, void* info) {
    function->DrawCircle(render, target, x, y, 10, white_c, red_c, 1);
};

extern "C" TOOLAPI void TOOLCALL Release(StandartModuleFuncs* function, Texture* target, Renderer* render, int x, int y, void* info) {
    
};

extern "C" TOOLAPI char* TOOLCALL GetName() {
    static char* tool_name = "Circle drawer";
    return tool_name;
};