#include <cstdlib>
#include <cstring>
#include "CustomTools.h"

namespace plugin {

void MakeSliderWithTitle(int size_x, int size_y, int coord_x, int coord_y, int max_val, int min_val, char* title, 
                         IPreferencesPanel* panel, ISliderCallback* callback,
                         int title_size_x, int title_size_y, int ch_size) {

    ILabel* label = api->GetWidgetFactory()->CreateLabel(title_size_x, title_size_y, title, ch_size);
    panel->Attach(label, coord_x + size_x / 2 - title_size_x / 2, coord_y - title_size_y);
    
    ISlider* slider_size = api->GetWidgetFactory()->CreateSlider(size_x, size_y, max_val, min_val);
    slider_size->SetSliderCallback(callback);
    panel->Attach(slider_size, coord_x, coord_y);
};

void MakeSliderWithTitleMeasure(int size_x, int size_y, int coord_x, int coord_y, int max_val, int min_val, char* title, 
                         IPreferencesPanel* panel, ILabel* num_label, ISliderCallback* callback,
                         int title_size_x, int title_size_y, int ch_size) {
                             
    ILabel* label = api->GetWidgetFactory()->CreateLabel(title_size_x, title_size_y, title, ch_size);
    panel->Attach(label, coord_x + size_x / 2 - title_size_x / 2, coord_y - title_size_y);
    panel->Attach(num_label, coord_x + size_x / 2 + title_size_x / 4, coord_y - title_size_y);

    ISlider* slider_size = api->GetWidgetFactory()->CreateSlider(size_x, size_y, max_val, min_val);
    slider_size->SetSliderCallback(callback);
    panel->Attach(slider_size, coord_x, coord_y);
};

ChangeShapeeCallback::ChangeShapeeCallback(ShapesDrawer* drawer, ShapeType type) : drawer(drawer), type(type) {};
void ChangeShapeeCallback::RespondOnClick() {
    drawer->SetShape(type);
};

ChangeColorSlideCallback::ChangeColorSlideCallback(ShapesDrawer* drawer, ILabel* num_label, ColorComponent component) : drawer(drawer), num_label(num_label), component(component) {};
void ChangeColorSlideCallback::RespondOnSlide(float old_value, float current_value) {
    color_t color = drawer->GetColor();

    static char buf[4] = {};

    switch(component) {
        case COMPONENT_RED:
            color &= 0xFF'FF'FF'00;
            color |= static_cast<uint32_t>(current_value) << 0;
        break;

        case COMPONENT_GREEN:
            color &= 0xFF'FF'00'FF;
            color |= static_cast<uint32_t>(current_value) << 8;
        break;

        case COMPONENT_BLUE:
            color &= 0xFF'00'FF'FF;
            color |= static_cast<uint32_t>(current_value) << 16;
        break;

        default:
        break;
    }

    memset(buf, 0, 4);
    itoa(static_cast<uint32_t>(current_value), buf, 10);
    num_label->SetText(buf);

    drawer->SetColor(color);
};

ChangeSizeSlideCallback::ChangeSizeSlideCallback(ShapesDrawer* drawer, ILabel* num_label) : drawer(drawer), num_label(num_label) {};
void ChangeSizeSlideCallback::RespondOnSlide(float old_value, float current_value) {
    drawer->SetSize(static_cast<int>(current_value));
    static char buf[4] = {};
    memset(buf, 0, 4);
    itoa(static_cast<uint32_t>(current_value), buf, 10);
    num_label->SetText(buf);
};

TestCallback::TestCallback() {};
void TestCallback::RespondOnClick() { 
    std::cout << "click callback!\n";
};

TestSliderCallback::TestSliderCallback() {};
void TestSliderCallback::RespondOnSlide(float old_value, float current_value) {
    std::cout << "new value: " << current_value <<"\n";
};

Filler::Filler() {
    this->ConstructPreferencePanel();
};
void Filler::Apply(ITexture* canvas) {
    Buffer buf = canvas->ReadBuffer();
    
    //std::cout << "ApplyFilter\n";
    
    for (int i = 0; i < canvas->GetSizeY() * canvas->GetSizeX(); ++i) {
        color_t pixel = buf.pixels[i];
        pixel = 0xFF'FF'FF'FF - pixel;
        pixel |= 0xFF'00'00'00;
        buf.pixels[i] = pixel;
    }

    canvas->LoadBuffer(buf);
    canvas->ReleaseBuffer(buf);
};
const char* Filler::GetName() const {
    static const char* name = "Filler";
    return name;
};
IPreferencesPanel* Filler::GetPreferencesPanel() const {
    return nullptr;
};
void Filler::ConstructPreferencePanel() {
    panel = api->GetWidgetFactory()->CreatePreferencesPanel();

    IButton* text_button = api->GetWidgetFactory()->CreateButtonWithText(120, 30, "Press me", 10);
    text_button->SetClickCallback(new TestCallback());

    panel->Attach(text_button, panel->GetSizeX() / 2, panel->GetSizeY() / 2);
};

ShapesDrawer::ShapesDrawer() : type(TYPE_CIRCLE), size(1), color(0xFF'00'00'00) {
    this->ConstructPreferencePanel();
};
void ShapesDrawer::ActionBegin(ITexture* canvas, int x, int y) {
    //std::cout << "started drawing shapes!\n";
    switch (type) {
        case TYPE_CIRCLE:
            canvas->DrawCircle({x, y, size, 1, color, color});
        break;
    
        case TYPE_SQUARE:
            canvas->DrawRect({x - size / 2, y - size / 2, x + size / 2, y + size / 2, 1, color, color});
        break;

        default:
            std::cout << "unkonw type: " << type << "\n";
        break;
    }    
};
void ShapesDrawer::SetSize(int new_size) { 
    size = new_size;
};
const char* ShapesDrawer::GetIconFileName() const {
    static const char* name = "shapes_drawer.bmp";
    return name;
};
const char* ShapesDrawer::GetName() const {
    static const char* name = "Shapes Drawer";
    return name;
};
IPreferencesPanel* ShapesDrawer::GetPreferencesPanel() const {
    return panel;
};
void ShapesDrawer::ConstructPreferencePanel() {
    IWidgetFactory* factory = api->GetWidgetFactory();
    
    panel = api->GetWidgetFactory()->CreatePreferencesPanel();

    ILabel* tool_name = api->GetWidgetFactory()->CreateLabel(170, 30, "Shapes drawer", 12);
    panel->Attach(tool_name, panel->GetSizeX() / 2 - 170 / 2, 0);

    Point coord();
    int button_x = 5 * panel->GetSizeX() / 8;
    int button_y = panel->GetSizeY() / 8;
    int dy = 10;
    int button_size_x = 80;
    int button_size_y = 30;
    
    IButton* circle_button = api->GetWidgetFactory()->CreateButtonWithText(button_size_x, button_size_y, "Circle", 10);
    circle_button->SetClickCallback(new ChangeShapeeCallback(this, TYPE_CIRCLE));
    panel->Attach(circle_button, button_x, button_y);

    IButton* square_button = api->GetWidgetFactory()->CreateButtonWithText(button_size_x, button_size_y, "Square", 10);
    square_button->SetClickCallback(new ChangeShapeeCallback(this, TYPE_SQUARE));
    panel->Attach(square_button, button_x, button_y + button_size_y + dy);

    ILabel* red_count   = factory->CreateLabel(50, 30, "0", 10);
    ILabel* green_count = factory->CreateLabel(50, 30, "0", 10);
    ILabel* blue_count  = factory->CreateLabel(50, 30, "0", 10);
    ILabel* size_count  = factory->CreateLabel(50, 30, "1", 10);

    MakeSliderWithTitleMeasure(150, 15, panel->GetSizeX() / 8, panel->GetSizeY() / 5 + 0  , 1, 100, "size:",  panel, size_count,  new ChangeSizeSlideCallback(this, size_count));
    MakeSliderWithTitleMeasure(150, 15, panel->GetSizeX() / 8, panel->GetSizeY() / 5 + 60 , 0, 255, "red:",   panel, red_count,   new ChangeColorSlideCallback(this, red_count,   COMPONENT_RED));
    MakeSliderWithTitleMeasure(150, 15, panel->GetSizeX() / 8, panel->GetSizeY() / 5 + 120, 0, 255, "green:", panel, green_count, new ChangeColorSlideCallback(this, green_count, COMPONENT_GREEN));
    MakeSliderWithTitleMeasure(150, 15, panel->GetSizeX() / 8, panel->GetSizeY() / 5 + 180, 0, 255, "blue:",  panel, blue_count,  new ChangeColorSlideCallback(this, blue_count,  COMPONENT_BLUE));
};

Plugin::Plugin(Filler* filler, ShapesDrawer* drawer) : filler(filler), drawer(drawer) {}
Filters Plugin::GetFilters() const {
    Filters filter_list;
    filter_list.count = 1;
    filter_list.filters = filler;
    return filter_list;
};
Tools Plugin::GetTools () const {
    Tools tool_list;
    tool_list.count = 1;
    tool_list.tools = drawer;
    return tool_list;
};

}