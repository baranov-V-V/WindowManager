#include <cassert>
#include "PluginApiClasses.h"

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

    memcpy(buf.pixels, texture->getBuf(), this->GetHieght() * this->GetWidth());

    return buf;
}
void RenderTexture::ReleaseBuffer(Buffer buffer) {

}
void RenderTexture::LoadBuffer(Buffer buffer) {
    for (int i = 0; i < this->GetWidth() * this->GetHieght(); ++i) {
        texture->getBuf()[i] = ToRGBQUAD(buffer.pixels[i]);
    }
}
void RenderTexture::Clear(Color color) {

}
void RenderTexture::Present() {
    //lol nothing;
}
void RenderTexture::DrawLine(int32_t x0, int32_t y0, int32_t x1, int32_t y1, Color color) {
    render->setWindow(texture);
    render->drawLine(x0, y0, x1, y1, color, 1);
}
void RenderTexture::DrawThickLine(int32_t x0, int32_t y0, int32_t x1, int32_t y1, int32_t thickness, Color color) {
    render->setWindow(texture);
    render->drawLine(x0, y0, x1, y1, color, thickness);
}
void RenderTexture::DrawCircle(int32_t x, int32_t y, int32_t radius, Color color) {
    render->setWindow(texture);
    render->drawCircle(x, y, radius, color, color);
}
void RenderTexture::DrawRect(int32_t x, int32_t y, int32_t width, int32_t height, Color color) {
    render->drawFilledRectangle(x, y, x + width, y + height, color, color, 1);
}
void RenderTexture::CopyTexture(ITexture* texture, int32_t x, int32_t y, int32_t width, int32_t height) {
    //??? че писать то
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

    //window = new TextButtonWindow(main_ch_size_x * (text_len + 3), main_ch_size_y * 3 / 2, 0, 0, mgrey_c, dgrey_c, 1, silver_c, text, main_font, main_ch_size_x, main_ch_size_y, ALIGN_LEFT)

};
TextButton::TextButton(int32_t width, int32_t height, const char* text, int32_t char_size) {

};
void TextButton::SetClickCallback(plugin::IClickCallback* callback) {};

PicButton::PicButton(const char* icon_file_name) {};
PicButton::PicButton(int32_t width, int32_t height, const char* icon_file_name) {};
void PicButton::SetClickCallback(plugin::IClickCallback* callback) {}

Slider::Slider() {};
void Slider::SetSliderCallback(plugin::ISliderCallback* callback) {};
float Slider::GetValue() { return 0; };
void Slider::SetValue(float value) {};

Label::Label() {};
void Label::SetText(const char* text) {};

PreferencesPanel::PreferencesPanel() {};
void PreferencesPanel::Attach(IWidget* widget, int32_t x, int32_t y) {};

WidgetFactory::WidgetFactory(Renderer* render) : render(render) {};
plugin::IButton* WidgetFactory::CreateDefaultButtonWithIcon(const char* icon_file_name) {};
plugin::IButton* WidgetFactory::CreateDefaultButtonWithText(const char* text) {};
plugin::IButton* WidgetFactory::CreateButtonWithIcon(int32_t width, int32_t height, const char* icon_file_name) {};
plugin::IButton* WidgetFactory::CreateButtonWithText(int32_t width, int32_t height, const char* text, int32_t char_size) {};
plugin::ISlider* WidgetFactory::CreateDefaultSlider(float range_min, float range_max) {};
plugin::ISlider* WidgetFactory::CreateSlider(int32_t width, int32_t height, float range_min, float range_max) {};
plugin::ISlider* WidgetFactory::CreateSlider(int32_t width, int32_t height, float thumb_width, float thumb_height, float range_min, float range_max) {};
plugin::ILabel*  WidgetFactory::CreateDefaultLabel(const char* text) {};
plugin::ILabel*  WidgetFactory::CreateLabel(int32_t width, int32_t height, const char* text, int32_t char_size) {};
plugin::IPreferencesPanel* WidgetFactory::CreateDefaultPreferencesPanel() {};

API::API(Renderer* render) : widget_factory(render), texture_factory(render) {};
plugin::IWidgetFactory*  API::GetWidgetFactory () { return &widget_factory; };
plugin::ITextureFactory* API::GetTextureFactory() { return &texture_factory; };

