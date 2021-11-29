#define EXPORT_TOOL
#include <iostream>
#include "PluginApi.h"

struct Point {
    int x;
    int y;

    Point(int x, int y) : x(x), y(y) {};
};

namespace plugin {

class ShapesDrawer;
class Filler;

enum ShapeType {
    TYPE_SQUARE,
    TYPE_CIRCLE
};

enum ColorComponent {
    COMPONENT_RED,
    COMPONENT_GREEN,
    COMPONENT_BLUE
};

IAPI* api = nullptr;

void MakeSliderWithTitle(int size_x, int size_y, int coord_x, int coord_y, int max_val, int min_val, char* title, 
                         IPreferencesPanel* panel, ISliderCallback* callback,
                         int title_size_x = 80, int title_size_y = 30, int ch_size = 10);

void MakeSliderWithTitleMeasure(int size_x, int size_y, int coord_x, int coord_y, int max_val, int min_val, char* title, 
                         IPreferencesPanel* panel, ILabel* num_label, ISliderCallback* callback,
                         int title_size_x = 80, int title_size_y = 30, int ch_size = 10);


class ChangeShapeeCallback : public IClickCallback {
  public:
    ChangeShapeeCallback(ShapesDrawer* drawer, ShapeType type);
    virtual ~ChangeShapeeCallback() {};

    virtual void RespondOnClick() override;

  private:
    ShapesDrawer* drawer;
    ShapeType type;
};

class ChangeColorSlideCallback : public ISliderCallback {
  public:
    ChangeColorSlideCallback(ShapesDrawer* drawer, ILabel* num_label, ColorComponent component);
    virtual ~ChangeColorSlideCallback() {};

    virtual void RespondOnSlide(float old_value, float current_value) override;

  private:
    ShapesDrawer* drawer;
    ILabel* num_label;
    ColorComponent component;
};

class ChangeSizeSlideCallback : public ISliderCallback {
  public:
    ChangeSizeSlideCallback(ShapesDrawer* drawer, ILabel* num_label);
    virtual ~ChangeSizeSlideCallback() {};

    virtual void RespondOnSlide(float old_value, float current_value) override;

  private:
    ShapesDrawer* drawer;
    ILabel* num_label;
};

class TestCallback : public IClickCallback {
  public:
    TestCallback();
    virtual ~TestCallback() {}

    virtual void RespondOnClick() override;
  private:
};

class TestSliderCallback : public ISliderCallback {
  public:
    TestSliderCallback();
    virtual ~TestSliderCallback() {};

    virtual void RespondOnSlide(float old_value, float current_value) override;
};

class Filler : public IFilter {
  public:
    Filler();
    virtual ~Filler() {};

    virtual void Apply(ITexture* canvas) override;
    virtual const char* GetName() const override;

    virtual IPreferencesPanel* GetPreferencesPanel() const override;

    void ConstructPreferencePanel();

  private:
    IPreferencesPanel* panel;
};

class ShapesDrawer : public ITool {
  public:
    ShapesDrawer();
    virtual ~ShapesDrawer() {};

    virtual void ActionBegin(ITexture* canvas, int x, int y) override;

    void SetSize(int new_size);
    Color GetColor() const { return color; };
    void SetColor(Color new_color) { color = new_color; };
    void SetShape(ShapeType new_type) { type = new_type; };


    virtual void Action     (ITexture* canvas, int x, int y, int dx, int dy) override {};
    virtual void ActionEnd  (ITexture* canvas, int x, int y) override {};

    virtual const char* GetIconFileName() const override;

    virtual IPreferencesPanel* GetPreferencesPanel() const override;

    void ConstructPreferencePanel();

  private:
    IPreferencesPanel* panel;
    ShapeType type;
    int size;
    Color color;
};

class Plugin : public IPlugin {
  public:
    Plugin(Filler* filler, ShapesDrawer* drawer);
    ~Plugin() {};

    virtual std::list<IFilter*> GetFilters() const override;

    virtual std::list<ITool*> GetTools () const override;

  private:
    Filler* filler;
    ShapesDrawer* drawer;
};

}

extern "C" TOOLAPI plugin::IPlugin* TOOLCALL Create(plugin::IAPI* gui_api) {
    plugin::api = gui_api;
    return new plugin::Plugin(new plugin::Filler(), new plugin::ShapesDrawer());
};

extern "C" TOOLAPI void TOOLCALL Destroy(plugin::IPlugin* plugin) {
    delete plugin;
};

extern "C" TOOLAPI uint32_t TOOLCALL Version() {
    return plugin::kVersion;
};

#undef EXPORT_TOOL