#pragma once

#include "plugin_api/NewPluginApi.h"
#include "gui_core/widgets/Slider.h"
#include "gui_core/widgets/BasicInfo.h"
#include "gui_core/widgets/Window.h"

COLORREF ToCOLORREF(plugin::color_t color);

namespace plugin {

color_t ToColor_t(COLORREF color);

class RenderTexture : public plugin::ITexture {
  public:
    RenderTexture(Texture* texture, Renderer* render);
    virtual ~RenderTexture() {}

    virtual int32_t GetSizeX() override;
    virtual int32_t GetSizeY() override;

    virtual Buffer ReadBuffer() override;
    virtual void ReleaseBuffer(Buffer buffer) override;
    virtual void LoadBuffer(Buffer buffer) override;

    virtual void Clear(color_t color) override;
    virtual void Present() override;

    virtual void DrawLine(const Line& line) override;
    virtual void DrawCircle(const Circle& circle) override;
    virtual void DrawRect(const Rect& rect) override;

    virtual void CopyTexture(ITexture* texture, int32_t x, int32_t y, int32_t width, int32_t height) override;
    virtual void CopyTexture(ITexture* source, int32_t x, int32_t y) override;

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

class Button : public plugin::IButton {
   public:
    Button(ManagerWindow* window);
    virtual ~Button() {};

    int32_t GetSizeX() override { return window->getSizeX(); };
    int32_t GetSizeY() override { return window->getSizeY(); };

    virtual void SetClickCallback(plugin::IClickCallback* callback) override;
    ManagerWindow* getWindow() { return window; };
  
  private:
    ManagerWindow* window;   
};

class Slider : public plugin::ISlider {
  public:
    Slider(float range_min, float range_max);
    Slider(int32_t width, int32_t height, float range_min, float range_max);
    Slider(int32_t width, int32_t height, float thumb_width, float thumb_height, float range_min, float range_max);
    
    virtual ~Slider() {};

    virtual void SetSliderCallback(plugin::ISliderCallback* callback) override;

    virtual float GetValue() override;
    virtual void SetValue(float value) override;
  
    virtual int32_t GetSizeX() override;
    virtual int32_t GetSizeY() override;

    ManagerWindow* getWindow();;

  private:
    SliderBasicX* slider;
};

class Label : public plugin::ILabel {
  public:
    Label(const char* text);
    Label(int32_t width, int32_t height, const char* text, int32_t char_size);
    virtual ~Label() {}

    virtual int32_t GetSizeX() override { return window->getSizeX(); };
    virtual int32_t GetSizeY() override { return window->getSizeY(); };

    virtual void SetText(const char* text) override;

    ManagerWindow* getWindow() { return window; };

  private:
    TextButtonWindow* window;
};

class Icon : public plugin::IIcon {
  public:
    Icon(int32_t size_x, int32_t size_y);
    virtual ~Icon() {};
  
    virtual int32_t GetSizeX() override { return window->getSizeX(); };
    virtual int32_t GetSizeY() override { return window->getSizeY(); };

    virtual void SetIcon(ITexture* icon) override;

    ManagerWindow* getWindow() { return window; };

  private:
    BorderWindow* window;
};

class Palette : public plugin::IPalette {
  public:
    Palette();
    virtual ~Palette() {}; 

    virtual int32_t GetSizeX() override { return window->getSizeX(); };
    virtual int32_t GetSizeY() override { return window->getSizeY(); };

    virtual void SetPaletteCallback(plugin::IPaletteCallback* callback) override; 

    ManagerWindow* getWindow() { return window; };

    void ChangeColor(plugin::color_t color);

  private:
    BorderWindow* window;
    plugin::IPaletteCallback* callback;
};

class PreferencesPanel : public plugin::IPreferencesPanel {
  public:
    PreferencesPanel();
    virtual ~PreferencesPanel() {}

    int32_t GetSizeX() override { return layout->getSizeX(); };
    int32_t GetSizeY() override { return layout->getSizeY(); };

    virtual void Attach(IButton*  button,  int32_t x, int32_t y) override;
    virtual void Attach(ILabel*   label,   int32_t x, int32_t y) override;
    virtual void Attach(ISlider*  slider,  int32_t x, int32_t y) override;
    virtual void Attach(IIcon*    icon,    int32_t x, int32_t y) override;
    virtual void Attach(IPalette* palette, int32_t x, int32_t y) override;

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

    virtual plugin::ILabel*  CreateDefaultLabel(const char* text) override;
    virtual plugin::ILabel*  CreateLabel(int32_t width, int32_t height, const char* text, int32_t char_size) override;

    virtual IIcon*    CreateIcon(int32_t size_x, int32_t size_y) override;

    virtual IPalette* CreatePalette() override;

    virtual plugin::IPreferencesPanel* CreatePreferencesPanel() override;
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


} //namespace plugin

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
