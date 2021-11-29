#include <cassert>
#include "PluginApiClasses.h"
#include "Functors.h"
#include "App.h"
#include "SkinsConfig.h"

using plugin::Buffer;

COLORREF ToCOLORREF(plugin::Color color) {
    return color & 0x00FFFFFF;
}

plugin::Color ToColor(COLORREF color) {
    return color | 0xFF'00'00'00;
}

RenderTexture::RenderTexture(Texture* texture, Renderer* render) : texture(texture), render(render) {};
int32_t RenderTexture::GetWidth() {
    return texture->getSizeX();
}
int32_t RenderTexture::GetHieght() {
    return texture->getSizeY();
}
Buffer RenderTexture::ReadBuffer() {
    Buffer buf = {};
    buf.texture = this;
    buf.pixels = new Color[this->GetHieght() * this->GetWidth()];

    for (int ix = 0; ix < texture->getSizeX(); ++ix) {
        for (int iy = 0; iy < texture->getSizeY(); ++iy) {
            buf.pixels[ix + iy * texture->getSizeX()] = ToColor(toCOLORREF(texture->getBuf()[ix + (max_canvas_y - iy - 1) * max_canvas_x]));
        }
    }
    /*
    for (int i = 0; i < this->GetHieght() * this->GetWidth(); ++i) {
        buf.pixels[i] = ToColor(toCOLORREF(texture->getBuf()[i]));
        //std::cout << buf.pixels[i] << "\n";
    }
    */

    return buf;
}
void RenderTexture::ReleaseBuffer(Buffer buffer) {

    /*
    for (int ix = 0; ix < texture->getSizeX(); ++ix) {
        for (int iy = 0; iy <= texture->getSizeY(); ++iy) {
            texture->getBuf()[ix + (max_canvas_y - iy) * max_canvas_x] = {128, 128, 128, 0};
        }
    }
    */

    delete[] buffer.pixels;
}
void RenderTexture::LoadBuffer(Buffer buffer) {

    for (int ix = 0; ix < texture->getSizeX(); ++ix) {
        for (int iy = 0; iy < texture->getSizeY(); ++iy) {
            texture->getBuf()[ix + (max_canvas_y - iy - 1) * max_canvas_x] = ToRGBQUAD(ToCOLORREF(buffer.pixels[ix + iy * texture->getSizeX()]));
        }
    }

}
void RenderTexture::Clear(Color color) {
    render->setWindow(texture);
    render->drawFilledRectangle(0, 0, texture->getSizeX(), texture->getSizeY(), ToCOLORREF(color), ToCOLORREF(color), 1);
}
void RenderTexture::Present() {
    //lol nothing;
}
void RenderTexture::DrawLine(int32_t x0, int32_t y0, int32_t x1, int32_t y1, Color color) {
    render->setWindow(texture);
    render->drawLine(x0, y0, x1, y1, ToCOLORREF(color), 1);
}
void RenderTexture::DrawThickLine(int32_t x0, int32_t y0, int32_t x1, int32_t y1, int32_t thickness, Color color) {
    render->setWindow(texture);
    render->drawLine(x0, y0, x1, y1, ToCOLORREF(color), thickness);
}
void RenderTexture::DrawCircle(int32_t x, int32_t y, int32_t radius, Color color) {
    render->setWindow(texture);
    render->drawCircle(x, y, radius, ToCOLORREF(color), ToCOLORREF(color));
}
void RenderTexture::DrawRect(int32_t x, int32_t y, int32_t width, int32_t height, Color color) {
    //std::cout << "in draw rect!\n";
    render->setWindow(texture);
    render->drawFilledRectangle(x, y, x + width, y + height, ToCOLORREF(color), ToCOLORREF(color), 1);
}
void RenderTexture::CopyTexture(ITexture* texture, int32_t x, int32_t y, int32_t width, int32_t height) {
    RenderTexture* source = dynamic_cast<RenderTexture*>(texture);
    source->getTexture()->showOn(this->getTexture(), x, y, width, height);
}

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

TextButton::TextButton(const char* text) {
    assert(text);
    int text_len = strlen(text);
    
    window = new TextButtonWindow(main_ch_size_x * (text_len + 3), main_ch_size_y * 3 / 2, 0, 0, mdgrey_c, mdgrey_c, 2, silver_c, text, main_font, main_ch_size_x, main_ch_size_y, ALIGN_LEFT, Renderer::getInstance(), nullptr);
    this->setAttachWindow(window);

    window->setPointed(new GlowBorderFunctor(window, mdgrey_c, lgrey_c));
};
TextButton::TextButton(int32_t width, int32_t height, const char* text, int32_t char_size) {
    assert(text);
    
    window = new TextButtonWindow(width, height, 0, 0, mdgrey_c, mdgrey_c, 2, silver_c, text, main_font, char_size * 2 / 3, char_size * 2, ALIGN_LEFT, Renderer::getInstance(), nullptr);
    this->setAttachWindow(window);

    window->setPointed(new GlowBorderFunctor(window, mdgrey_c, lgrey_c));
};
void TextButton::SetClickCallback(plugin::IClickCallback* callback) {
    assert(callback);

    window->setPressUp(new ClickCallbackFunctor(callback));
};

PicButton::PicButton(const char* icon_file_name) {
    assert(icon_file_name);
    window = new PicWindow(0, 0, const_cast<char*>(icon_file_name));
    this->setAttachWindow(window);
};
PicButton::PicButton(int32_t width, int32_t height, const char* icon_file_name) {
    assert(icon_file_name);
    window = new PicWindow(width, height, 0, 0, icon_file_name);
    this->setAttachWindow(window);
};
void PicButton::SetClickCallback(plugin::IClickCallback* callback) {
    assert(callback);

    window->setPressUp(new ClickCallbackFunctor(callback));
}

Slider::Slider(float range_min, float range_max) {
    slider = new SliderBasicX(100, 15, 0, 0, range_min, range_max);
    this->setAttachWindow(slider);
};
Slider::Slider(int32_t width, int32_t height, float range_min, float range_max) {
    slider = new SliderBasicX(width, height, 0, 0, range_min, range_max);
    this->setAttachWindow(slider);
};
Slider::Slider(int32_t width, int32_t height, float thumb_width, float thumb_height, float range_min, float range_max) {
    slider = new SliderBasicX(width, height, 0, 0, range_min, range_max, thumb_width);
    this->setAttachWindow(slider);
};
void Slider::SetSliderCallback(plugin::ISliderCallback* callback) {
    slider->SetSliderAction(new SliderCallbackAction(callback));
};

Label::Label(const char* text) {
    assert(text);
    int text_len = strlen(text);
    
    window = new TextButtonWindow(main_ch_size_x * (text_len + 3), main_ch_size_y * 3 / 2, 0, 0, palette_c, palette_c, 1, silver_c, text, main_font, main_ch_size_x, main_ch_size_y, ALIGN_MID, Renderer::getInstance(), nullptr);
    this->setAttachWindow(window);
};
Label::Label(int32_t width, int32_t height, const char* text, int32_t char_size) {
    assert(text);
    
    window = new TextButtonWindow(width, height, 0, 0, palette_c, palette_c, 1, silver_c, text, main_font, char_size * 2 / 3, char_size * 2, ALIGN_MID, Renderer::getInstance(), nullptr);
    this->setAttachWindow(window);
};
void Label::SetText(const char* text) {
    window->setText(const_cast<char*>(text));
};

PreferencesPanel::PreferencesPanel() {
    layout = new BorderWindow(400, 400, 300, 150, palette_c, dgrey_c, 1, Renderer::getInstance());

    PicWindow* close = new PicWindow(40, 25, 400 - 40 * 1, 0, img_close, layout);
    layout->addChild(close);
    
    close->setPressUp(new SetShowFunctor(layout, false));
    close->setPointed(new GlowPicFunctor(close, img_close, img_close2));

    this->setAttachWindow(layout);
    MakeMovable(layout, layout, App::getInstance());
};
void PreferencesPanel::Attach(IWidget* widget, int32_t x, int32_t y) {
    WidgetInfo* info = dynamic_cast<WidgetInfo*>(widget);
    info->getAttachWindow()->setCoord({x, y});
    layout->addChild(info->getAttachWindow());
    info->getAttachWindow()->setParent(layout);
};

WidgetFactory::WidgetFactory(Renderer* render) : render(render) {};
plugin::IButton* WidgetFactory::CreateDefaultButtonWithIcon(const char* icon_file_name) {
    return new PicButton(icon_file_name);
};
plugin::IButton* WidgetFactory::CreateDefaultButtonWithText(const char* text) {
    return new TextButton(text);
};
plugin::IButton* WidgetFactory::CreateButtonWithIcon(int32_t width, int32_t height, const char* icon_file_name) {
    return new PicButton(width, height, icon_file_name);
};
plugin::IButton* WidgetFactory::CreateButtonWithText(int32_t width, int32_t height, const char* text, int32_t char_size) {
    return new TextButton(width, height, text, char_size);
};
plugin::ISlider* WidgetFactory::CreateDefaultSlider(float range_min, float range_max) {
    return new Slider(range_min, range_max);
};
plugin::ISlider* WidgetFactory::CreateSlider(int32_t width, int32_t height, float range_min, float range_max) {
    return new Slider(width, height, range_min, range_max);
};
plugin::ISlider* WidgetFactory::CreateSlider(int32_t width, int32_t height, float thumb_width, float thumb_height, float range_min, float range_max) {
    return new Slider(width, height, thumb_width, thumb_height, range_min, range_max);
};
plugin::ILabel*  WidgetFactory::CreateDefaultLabel(const char* text) {
    return new Label(text);  
};  
plugin::ILabel*  WidgetFactory::CreateLabel(int32_t width, int32_t height, const char* text, int32_t char_size) {
    return new Label(width, height, text, char_size);
};
plugin::IPreferencesPanel* WidgetFactory::CreateDefaultPreferencesPanel() {
    return new PreferencesPanel();
};

API::API() : widget_factory(Renderer::getInstance()), texture_factory(Renderer::getInstance()) {};
plugin::IWidgetFactory*  API::GetWidgetFactory () { return &widget_factory; };
plugin::ITextureFactory* API::GetTextureFactory() { return &texture_factory; };

