#define EXPORT_TOOL
#include "PluginApi.h"

namespace plugin {

IAPI* api = nullptr;

class Filler : public IFilter {
  public:
    Filler() {
        this->ConstructPreferencePanel();
    };
    virtual ~Filler() {};

    virtual void Apply(ITexture* canvas) override {
        canvas->DrawRect(0, 0, canvas->GetWidth(), canvas->GetHieght(), 0x00'FF'00'FF);    
    };

    virtual const char* GetName() const override {
        static const char* name = "Filler";
        return name;
    };

    virtual IPreferencesPanel* GetPreferencesPanel() const override {
        return panel;
    };

    void ConstructPreferencePanel() {
        panel = api->GetWidgetFactory()->CreateDefaultPreferencesPanel();
    };

  private:
    IPreferencesPanel* panel;
};

class Plugin : public IPlugin {
  public:
    Plugin(Filler* filler) : filler(filler) {}
    ~Plugin() {};

    virtual std::list<IFilter*> GetFilters() const override {
        std::list<IFilter*> filter_list;
        filter_list.push_back(filler);
        return filter_list;
    };

    virtual std::list<ITool*> GetTools () const override {
        return std::list<ITool*>();
    };

  private:
    Filler* filler;
};

}

extern "C" TOOLAPI plugin::IPlugin* TOOLCALL Create(plugin::IAPI* gui_api) {
    plugin::api = gui_api;
    return new plugin::Plugin(new plugin::Filler());
};

extern "C" TOOLAPI void TOOLCALL Destroy(plugin::IPlugin* plugin) {
    delete plugin;
};

extern "C" TOOLAPI uint32_t TOOLCALL Version() {
    return plugin::kVersion;
};