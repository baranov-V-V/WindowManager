#pragma once

#include "PluginApi.h"
#include "Slider.h"
#include "BasicInfo.h"
#include "Window.h"

using plugin::Buffer;
using plugin::Color;
using plugin::ITexture;
using plugin::ITextureFactory;

COLORREF ToCOLORREF(plugin::Color color);

class RenderTexture : public plugin::ITexture {
  public:
    RenderTexture(Texture* texture, Renderer* render);
    virtual ~RenderTexture() {}

    virtual int32_t GetWidth() override;
    virtual int32_t GetHieght() override;

    virtual Buffer ReadBuffer() override;
    virtual void ReleaseBuffer(Buffer buffer) override;
    virtual void LoadBuffer(Buffer buffer) override;

    virtual void Clear(Color color) override;
    virtual void Present() override;

    virtual void DrawLine(int32_t x0, int32_t y0, int32_t x1, int32_t y1, Color color) override;
    virtual void DrawThickLine(int32_t x0, int32_t y0, int32_t x1, int32_t y1, int32_t thickness, Color color) override;
    virtual void DrawCircle(int32_t x, int32_t y, int32_t radius, Color color) override;
    virtual void DrawRect(int32_t x, int32_t y, int32_t width, int32_t height, Color color) override;

    virtual void CopyTexture(ITexture* texture, int32_t x, int32_t y, int32_t width, int32_t height) override;

    Texture* getTexture() const { return texture; };
    Renderer* getRenderer() const { return render; };

  private:
    Texture* texture;
    Renderer* render;
};

class TextureFactory : public plugin::ITextureFactory {
  public:
    TextureFactory(Renderer* render);
    virtual ~TextureFactory() {}

    virtual ITexture* CreateTexture(const char* filename) override;
    virtual ITexture* CreateTexture(int32_t width, int32_t height) override;

  private:
    Renderer* render;
};

class WidgetInfo : public plugin::IWidget {
  public:
    WidgetInfo() : window_info(nullptr) {};
    WidgetInfo(ManagerWindow* window) : window_info(window) {};
    
    virtual ~WidgetInfo() {};

    ManagerWindow* getAttachWindow() { return window_info; };
    void setAttachWindow(ManagerWindow* new_window) { window_info = new_window; };

  private:
    ManagerWindow* window_info;
};

class ClickCallback : public plugin::IClickCallback {
  public:
    ClickCallback();
    virtual ~ClickCallback() {}

    virtual void RespondOnClick() override;
  private:
};

class SliderCallback : public plugin::ISliderCallback {
  public:
    SliderCallback();
    virtual ~SliderCallback() {}

    virtual void RespondOnSlide(float old_value, float current_value) override;
  private:
};

class TextButton : public plugin::IButton, public WidgetInfo {
  public:
    TextButton(const char* text);
    TextButton(int32_t width, int32_t height, const char* text, int32_t char_size);
    virtual ~TextButton() {}

    int32_t GetWidth() override { return window->getSizeX(); };
    int32_t GetHieght() override { return window->getSizeY(); };

    virtual void SetClickCallback(plugin::IClickCallback* callback) override;
  private:
    TextButtonWindow* window;
};

class PicButton : public plugin::IButton, public WidgetInfo {
  public:
    PicButton(const char* icon_file_name);
    PicButton(int32_t width, int32_t height, const char* icon_file_name);
    virtual ~PicButton() {};

    int32_t GetWidth() override { return window->getSizeX(); };
    int32_t GetHieght() override { return window->getSizeY(); };

    virtual void SetClickCallback(plugin::IClickCallback* callback) override;
  private:
    PicWindow* window;
};

class Slider : public plugin::ISlider, public WidgetInfo {
  public:
    Slider(float range_min, float range_max);
    Slider(int32_t width, int32_t height, float range_min, float range_max);
    Slider(int32_t width, int32_t height, float thumb_width, float thumb_height, float range_min, float range_max);
    
    virtual ~Slider() {};

    virtual void SetSliderCallback(plugin::ISliderCallback* callback) override;

    virtual float GetValue() override { return slider->getValue(); };
    virtual void SetValue(float value) override { slider->setValue(value); };
  
    virtual int32_t GetWidth() override { return slider->getSizeX(); };
    virtual int32_t GetHieght() override { return slider->getSizeY(); };

  private:
    BasicSliderX* slider;
};

class Label : public plugin::ILabel, public WidgetInfo {
  public:
    Label(const char* text);
    Label(int32_t width, int32_t height, const char* text, int32_t char_size);
    virtual ~Label() {}

    virtual int32_t GetWidth() override { return window->getSizeX(); };
    virtual int32_t GetHieght() override { return window->getSizeY(); };

    virtual void SetText(const char* text) override;
  private:
    TextButtonWindow* window;
};

class PreferencesPanel : public plugin::IPreferencesPanel, public WidgetInfo {
  public:
    PreferencesPanel();
    virtual ~PreferencesPanel() {}

    int32_t GetWidth()  override { return layout->getSizeX(); };
    int32_t GetHieght() override { return layout->getSizeY(); };

    virtual void Attach(IWidget* widget, int32_t x, int32_t y) override;

    BorderWindow* getLayout() const { return layout; };

  public:
    BorderWindow* layout;
};

class WidgetFactory : public plugin::IWidgetFactory {
  public:
    WidgetFactory(Renderer* render);
    virtual ~WidgetFactory() {}

    virtual plugin::IButton* CreateDefaultButtonWithIcon(const char* icon_file_name) override;
    virtual plugin::IButton* CreateDefaultButtonWithText(const char* text) override;
    virtual plugin::IButton* CreateButtonWithIcon(int32_t width, int32_t height, const char* icon_file_name) override;
    virtual plugin::IButton* CreateButtonWithText(int32_t width, int32_t height, const char* text, int32_t char_size) override;

    virtual plugin::ISlider* CreateDefaultSlider(float range_min, float range_max) override;
    virtual plugin::ISlider* CreateSlider(int32_t width, int32_t height, float range_min, float range_max) override;
    virtual plugin::ISlider* CreateSlider(int32_t width, int32_t height, float thumb_width, float thumb_height, float range_min, float range_max) override;

    virtual plugin::ILabel*  CreateDefaultLabel(const char* text) override;
    virtual plugin::ILabel*  CreateLabel(int32_t width, int32_t height, const char* text, int32_t char_size) override;

    virtual plugin::IPreferencesPanel* CreateDefaultPreferencesPanel() override;
  private:
    Renderer* render;
};

class API : public plugin::IAPI {
  public:
    API();
    virtual ~API() {};

    virtual plugin::IWidgetFactory*  GetWidgetFactory () override;
    virtual plugin::ITextureFactory* GetTextureFactory() override;
  private:
    WidgetFactory widget_factory;
    TextureFactory texture_factory; 
};

/*
class Plugin : public plugin::IPlugin {
  public:
    Plugin();
    virtual ~Plugin() {}

    virtual std::list<plugin::IFilter*> GetFilters() const override;
    virtual std::list<plugin::ITool*>   GetTools () const override;
  private:

};
*/
