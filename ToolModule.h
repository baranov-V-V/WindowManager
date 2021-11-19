#include "BasicWindow.h"
#include "ModuleStandartFunc.h"

#define TOOLCALL __cdecl

#ifdef EXPORT_TOOL
  #define TOOLAPI __declspec(dllexport)
#else
  #define TOOLAPI __declspec(dllimport)
#endif

//functions in this struct can be used in your dll!
struct StandartModuleFuncs {
  pRenderDrawLine DrawLine;
  pRenderSetPixel SetPixel; 
  pRenderdrawCircle DrawCircle;
  pRenderdrawRectangle DrawRect;
  pRenderdrawFilledRectangle DrawFilledRect;
  pRenderdrawRoundRect DrawRoundRect;
};

/*
FIRST 8 BYTES OF void* info IS RESERVED IN FOLLOWING WAY:
BYTES[0..3] - CURRENCT FEATHER COLOR IN APP (cast to COLORREF)
BYTES[4..7] - CURRENCT FEATHER THICKNESS IN APP (cast to INT)
*/


typedef void (*ToolFunc)(StandartModuleFuncs* function, Texture* target, Renderer* render, int x, int y, void* info);
typedef char* (*NameFunc)();

extern "C" TOOLAPI void TOOLCALL Move(StandartModuleFuncs* function, Texture* target, Renderer* render, int x, int y, void* info);
extern "C" TOOLAPI void TOOLCALL Press(StandartModuleFuncs* function, Texture* target, Renderer* render, int x, int y, void* info);
extern "C" TOOLAPI void TOOLCALL Release(StandartModuleFuncs* function, Texture* target, Renderer* render, int x, int y, void* info);
extern "C" TOOLAPI char* TOOLCALL GetName();

const int func_count = 4;
const int func_press_pos = 1;

static char* func_names[] = {
    "GetName",
    "Press",
    "Move",
    "Release"
};