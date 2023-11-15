#include <cassert>
#include "PluginApiClasses.h"
#include "Slider.h"
#include "Functors.h"
#include "App.h"
#include "SkinsConfig.h"

using plugin::Buffer;

COLORREF ToCOLORREF(plugin::color_t color) {
    return color & 0x00FFFFFF;
}

plugin::color_t ToColor_t(COLORREF color) {
    return color | 0xFF'00'00'00;
}

namespace plugin {

RenderTexture::RenderTexture(Texture* texture, Renderer* render) : texture(texture), render(render) {};
int32_t RenderTexture::GetSizeX() {
    return texture->getSizeX();
}
int32_t RenderTexture::GetSizeY() {
    return texture->getSizeY();
}
Buffer RenderTexture::ReadBuffer() {
    Buffer buf = {};
    buf.texture = this;
    buf.pixels = new color_t[this->GetSizeY() * this->GetSizeX()];

    for (int ix = 0; ix < texture->getSizeX(); ++ix) {
        for (int iy = 0; iy < texture->getSizeY(); ++iy) {
            buf.pixels[ix + iy * texture->getSizeX()] = ToColor_t(toCOLORREF(texture->getBuf()[ix + (max_canvas_y - iy - 1) * max_canvas_x]));
        }
    }

    return buf;
}
void RenderTexture::ReleaseBuffer(Buffer buffer) {

    delete[] buffer.pixels;
}
void RenderTexture::LoadBuffer(Buffer buffer) {

    for (int ix = 0; ix < texture->getSizeX(); ++ix) {
        for (int iy = 0; iy < texture->getSizeY(); ++iy) {
            texture->getBuf()[ix + (max_canvas_y - iy - 1) * max_canvas_x] = ToRGBQUAD(ToCOLORREF(buffer.pixels[ix + iy * texture->getSizeX()]));
        }
    }

}
void RenderTexture::Clear(color_t color) {
    render->setWindow(texture);
    render->drawFilledRectangle(0, 0, texture->getSizeX(), texture->getSizeY(), ToCOLORREF(color), ToCOLORREF(color), 1);
}
void RenderTexture::Present() {
    //lol nothing;
}
void RenderTexture::DrawLine(const Line& line) {
    render->setWindow(texture);
    render->drawLine(line.x0, line.y0, line.x1, line.y1, ToCOLORREF(line.color), line.thickness);
}
void RenderTexture::DrawCircle(const Circle& circle) {
    render->setWindow(texture);
    render->drawCircle(circle.x, circle.y, circle.radius, ToCOLORREF(circle.fill_color), ToCOLORREF(circle.outline_color), circle.outline_thickness);
}
void RenderTexture::DrawRect(const Rect& rect) {
    render->setWindow(texture);
    render->drawFilledRectangle(rect.x, rect.y, rect.x + rect.size_x, rect.y + rect.size_y, ToCOLORREF(rect.fill_color), ToCOLORREF(rect.fill_color), rect.outline_thickness);
}
void RenderTexture::CopyTexture(ITexture* source, int32_t x, int32_t y, int32_t width, int32_t height) {
    RenderTexture* source_t = dynamic_cast<RenderTexture*>(source);
    source_t->getTexture()->showOn(this->getTexture(), x, y, width, height);
}
void RenderTexture::CopyTexture(ITexture* source, int32_t x, int32_t y) {
    RenderTexture* source_t = dynamic_cast<RenderTexture*>(source);
    source_t->getTexture()->showOn(this->getTexture(), x, y);
};


TextureFactory::TextureFactory(Renderer* render) : render(render) {};
ITexture* TextureFactory::CreateTexture(const char* filename) {
    Texture* new_texure = new Texture(filename, 0, 0);
    return new RenderTexture(new_texure, render);
};
ITexture* TextureFactory::CreateTexture(int32_t width, int32_t height) {
    Texture* new_texure = new Texture(width, height, white_c, 0, 0);
    return new RenderTexture(new_texure, render);
};  

ClickCallback::ClickCallback() {};
void ClickCallback::RespondOnClick() {};

SliderCallback::SliderCallback() {};
void SliderCallback::RespondOnSlide(float old_value, float current_value) {};

Slider::Slider(float range_min, float range_max) {
    slider = new SliderBasicX(100, 15, 0, 0, range_min, range_max);
};
Slider::Slider(int32_t width, int32_t height, float range_min, float range_max) {
    slider = new SliderBasicX(width, height, 0, 0, range_min, range_max);
};
Slider::Slider(int32_t width, int32_t height, float thumb_width, float thumb_height, float range_min, float range_max) {
    slider = new SliderBasicX(width, height, 0, 0, range_min, range_max, thumb_width);
};
void Slider::SetSliderCallback(plugin::ISliderCallback* callback) {
    slider->SetSliderAction(new SliderCallbackAction(callback));
};
float Slider::GetValue() {
    return slider->getValue();
};
void Slider::SetValue(float value) {
    slider->setValue(value);
};
int32_t Slider::GetSizeX() { 
    return slider->getSizeX();
};
int32_t Slider::GetSizeY() { 
    return slider->getSizeY();
};
ManagerWindow* Slider::getWindow() {
    return slider;
};

Label::Label(const char* text) {
    assert(text);
    int text_len = strlen(text);
    
    window = new TextButtonWindow(main_ch_size_x * (text_len + 3), main_ch_size_y * 3 / 2, 0, 0, palette_c, palette_c, 1, silver_c, text, main_font, main_ch_size_x, main_ch_size_y, ALIGN_MID, Renderer::getInstance(), nullptr);
};
Label::Label(int32_t width, int32_t height, const char* text, int32_t char_size) {
    assert(text);
    
    window = new TextButtonWindow(width, height, 0, 0, palette_c, palette_c, 1, silver_c, text, main_font, char_size * 2 / 3, char_size * 2, ALIGN_MID, Renderer::getInstance(), nullptr);
};
void Label::SetText(const char* text) {
    window->setText(const_cast<char*>(text));
};

Button::Button(ManagerWindow* window) : window(window) {};
void Button::SetClickCallback(plugin::IClickCallback* callback) {
    window->setPressUp(new ClickCallbackFunctor(callback));
};

Icon::Icon(int32_t size_x, int32_t size_y) {
    window = new BorderWindow(size_x, size_y, 0, 0, white_c, mdgrey_c, 1, Renderer::getInstance());
};
void Icon::SetIcon(ITexture* icon) {
    Texture* source = dynamic_cast<RenderTexture*>(icon)->getTexture();

    int dx = std::max((window->getSizeX() - source->getSizeX()) / 2, 0);
    int dy = std::max((window->getSizeY() - source->getSizeY()) / 2, 0);

    source->showOn(window, dx, dy, std::min(source->getSizeX(), window->getSizeX()), std::min(source->getSizeY(), window->getSizeY()));
};

Palette::Palette() {
    int x_size = 150;
    int y_size = 300;

    window = new BorderWindow(x_size, y_size, 0, 0, mdgrey_c, mdgrey_c, 1, Renderer::getInstance());
    
    int c_box_x = 5 * x_size / (6 * x_count_c + 1);
    int c_box_y = c_box_x;
    
    int c_box_dx = x_size / (6 * x_count_c + 1);
    int c_box_dy = x_size / (6 * x_count_c + 1);

    int start_boxes_y = 8;

    for (int i = 0; i < y_count_c; ++i) {
        for (int j = 0; j < x_count_c; ++j) {
            ChangeColorCallbackFunctor* change_f  = new ChangeColorCallbackFunctor(this, palette_colors[i][j]);
            BorderWindow* color_button = new BorderWindow(c_box_x, c_box_y, c_box_dx + (c_box_dx + c_box_x) * j, start_boxes_y + (c_box_y + c_box_dy) * i, palette_colors[i][j], black_c, 1, Renderer::getInstance(), window, change_f);
            window->addChild(color_button);
        }
    }
    
};
void Palette::ChangeColor(color_t color) {
    callback->RespondOnChangeColor(color);
};
void Palette::SetPaletteCallback(IPaletteCallback* callback) {
    Palette::callback = callback;
}; 

PreferencesPanel::PreferencesPanel() {
    layout = new BorderWindow(400, 400, 300, 150, palette_c, dgrey_c, 1, Renderer::getInstance());

    PicWindow* close = new PicWindow(40, 25, 400 - 40 * 1, 0, img_close, layout);
    layout->addChild(close);
    
    close->setPressUp(new SetShowFunctor(layout, false));
    close->setPointed(new GlowPicFunctor(close, img_close, img_close2));

    MakeMovable(layout, layout, App::getInstance());
};
void PreferencesPanel::Attach(IButton* button, int32_t x, int32_t y) {
    layout->addChild(dynamic_cast<Button*>(button)->getWindow());
};
void PreferencesPanel::Attach(ILabel* label, int32_t x, int32_t y) {
    layout->addChild(dynamic_cast<Label*>(label)->getWindow());
};
void PreferencesPanel::Attach(ISlider* slider, int32_t x, int32_t y) {
    layout->addChild(dynamic_cast<Slider*>(slider)->getWindow());
};
void PreferencesPanel::Attach(IIcon* icon, int32_t x, int32_t y) {
    layout->addChild(dynamic_cast<Icon*>(icon)->getWindow());
};
void PreferencesPanel::Attach(IPalette* palette, int32_t x, int32_t y) { 
    layout->addChild(dynamic_cast<Palette*>(palette)->getWindow());
};

WidgetFactory::WidgetFactory(Renderer* render) : render(render) {};
plugin::IButton* WidgetFactory::CreateDefaultButtonWithIcon(const char* icon_file_name) {
    return new Button(new PicWindow(0, 0, const_cast<char*>(icon_file_name)));
};
plugin::IButton* WidgetFactory::CreateDefaultButtonWithText(const char* text) {
    assert(text);
    int text_len = strlen(text);
    
    TextButtonWindow* window = new TextButtonWindow(main_ch_size_x * (text_len + 3), main_ch_size_y * 3 / 2, 0, 0, mdgrey_c, mdgrey_c, 2, silver_c, text, main_font, main_ch_size_x, main_ch_size_y, ALIGN_LEFT, Renderer::getInstance(), nullptr);
    window->setPointed(new GlowBorderFunctor(window, mdgrey_c, lgrey_c));

    return new Button(window);
};
plugin::IButton* WidgetFactory::CreateButtonWithIcon(int32_t width, int32_t height, const char* icon_file_name) {
    return new Button(new PicWindow(width, height, 0, 0, icon_file_name));
};
plugin::IButton* WidgetFactory::CreateButtonWithText(int32_t width, int32_t height, const char* text, int32_t char_size) {
    assert(text);
    
    TextButtonWindow* window = new TextButtonWindow(width, height, 0, 0, mdgrey_c, mdgrey_c, 2, silver_c, text, main_font, char_size * 2 / 3, char_size * 2, ALIGN_LEFT, Renderer::getInstance(), nullptr);
    window->setPointed(new GlowBorderFunctor(window, mdgrey_c, lgrey_c));

    return new Button(window);
};
plugin::ISlider* WidgetFactory::CreateDefaultSlider(float range_min, float range_max) {
    return new Slider(range_min, range_max);
};
plugin::ISlider* WidgetFactory::CreateSlider(int32_t width, int32_t height, float range_min, float range_max) {
    return new Slider(width, height, range_min, range_max);
};
plugin::ILabel* WidgetFactory::CreateDefaultLabel(const char* text) {
    return new Label(text);  
};  
plugin::ILabel* WidgetFactory::CreateLabel(int32_t width, int32_t height, const char* text, int32_t char_size) {
    return new Label(width, height, text, char_size);
};
plugin::IIcon* WidgetFactory::CreateIcon(int32_t size_x, int32_t size_y) {
    return new Icon(size_x, size_y);
};
plugin::IPalette* WidgetFactory::CreatePalette() { 
    return new Palette();
};
plugin::IPreferencesPanel* WidgetFactory::CreatePreferencesPanel() {
    return new PreferencesPanel();
};

API::API() : widget_factory(Renderer::getInstance()), texture_factory(Renderer::getInstance()) {};
plugin::IWidgetFactory*  API::GetWidgetFactory () { return &widget_factory; };
plugin::ITextureFactory* API::GetTextureFactory() { return &texture_factory; };

} //namespace plugin