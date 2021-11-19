#include "ModuleStandartFunc.h"

RGBQUAD* TextureGetBuf(Texture* texture) {
    return texture->getBuf();
};

void RenderDrawLine(Renderer* render, Texture* target, double x_begin, double y_begin, double x_end, double y_end, COLORREF color, int thickness) {
    render->setWindow(target);
    render->drawLine(x_begin, y_begin, x_end, y_end, color, thickness);
};

void RenderSetPixel(Renderer* render, Texture* target, double x, double y, COLORREF color) {
    render->setWindow(target);
    render->setPixel(x, y, color);
};

void RenderdrawCircle(Renderer* render, Texture* target, double x, double y, double r, COLORREF color, COLORREF border_color, int thickness) {
    render->setWindow(target);
    render->drawCircle(x, y, r, color, border_color, thickness);
};

void RenderdrawRectangle(Renderer* render, Texture* target, double x1, double y1, double x2, double y2, COLORREF color, int thinkness) {
    render->setWindow(target);
    render->drawRectangle(x1, y1, x2, y2, color, thinkness);
};

void RenderdrawFilledRectangle(Renderer* render, Texture* target, double x1, double y1, double x2, double y2, COLORREF fill_color, COLORREF color, int thickness) {
    render->setWindow(target);
    render->drawFilledRectangle(x1, y1, x2, y2, fill_color, color, 1);
};

void RenderdrawRoundRect(Renderer* render, Texture* target, double x1, double y1, double x2, double y2, double width, double height, COLORREF color, int thinkness) {
    render->setWindow(target);
    render->drawRoundRect(x1, y1, x2, y2, width, height, color, thinkness);
};