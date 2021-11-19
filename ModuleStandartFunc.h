#pragma once

#include "BasicWindow.h"

RGBQUAD* TextureGetBuf(Texture* texture);

void RenderDrawLine(Renderer* render, Texture* target, double x_begin, double y_begin, double x_end, double y_end, COLORREF color = black_c, int thickness = 1);
void RenderSetPixel(Renderer* render, Texture* target, double x, double y, COLORREF color);
void RenderdrawCircle(Renderer* render, Texture* target, double x, double y, double r, COLORREF color = black_c,  COLORREF border_color = black_c, int thickness = 1);
void RenderdrawRectangle(Renderer* render, Texture* target, double x1, double y1, double x2, double y2, COLORREF color = black_c, int thinkness = 1);
void RenderdrawFilledRectangle(Renderer* render, Texture* target, double x1, double y1, double x2, double y2, COLORREF fill_color, COLORREF color, int thickness = 1);
void RenderdrawRoundRect(Renderer* render, Texture* target, double x1, double y1, double x2, double y2, double width, double height, COLORREF color = black_c, int thinkness = 1);

typedef void (*pRenderDrawLine)(Renderer* render, Texture* target, double x_begin, double y_begin, double x_end, double y_end, COLORREF color, int thickness);
typedef void (*pRenderSetPixel)(Renderer* render, Texture* target, double x, double y, COLORREF color);
typedef void (*pRenderdrawCircle)(Renderer* render, Texture* target, double x, double y, double r, COLORREF color,  COLORREF border_color, int thickness);
typedef void (*pRenderdrawRectangle)(Renderer* render, Texture* target, double x1, double y1, double x2, double y2, COLORREF color, int thinkness1);
typedef void (*pRenderdrawFilledRectangle)(Renderer* render, Texture* target, double x1, double y1, double x2, double y2, COLORREF fill_color, COLORREF color, int thickness);
typedef void (*pRenderdrawRoundRect)(Renderer* render, Texture* target, double x1, double y1, double x2, double y2, double width, double height, COLORREF color, int thinkness);