# Overview

This is a graphic user interface library to create your own desctop app. This repo includes painting app implemented using this library as an example

## GUI

You can create an instance of already implemented widget, such as PicWindow, ManagerWindow, Slider and more. Or you can create your own class of widget, inheriting it from BasicWinow or any other already implemented widget and overriding and implementing the methods you need

## Painting app

To create new canvas to draw in the app go to "File" -> "Open canvas"

![](screenshots/open_canvas.png)

You will see a canvas window, with usage of tool on the left you can draw

![](screenshots/canvas.png)

The palette with colors and available tools. In the up left corner there's a button "Filters". Also a widget has two scrollbars to navigate within the canvas.

![](screenshots/app.png)

## Plugin API

You can add a new filter or a new tool with no need to recompile the project. To create a tool or a filter you need to write a class with a specific interface and compile it as a dll. The program will dynamically link it and get access to provided methods. The inferface is described below

### Basic types

```C++
typedef uint32_t Color; // RGBA8888
```

### Import classes

There are classes application developer should implement and pass them to plugin functions.
Application developer should call ```Create``` function at start of using plugin, and ```Destroy``` at the end.

### Render API

This struct helps you to work with texture as with buffer of pixels.

```C++
struct Buffer {
    Color* pixels;
    ITexture* texture;
};
```

Class that plugin developer uses for *drawing*

```C++
class ITexture {
  public:
    virtual ~ITexture() {}

    virtual int32_t GetWidth() = 0;
    virtual int32_t GetHieght() = 0;

    virtual Buffer ReadBuffer() = 0;
    virtual void   ReleaseBuffer(Buffer buffer) = 0;
    virtual void   LoadBuffer(Buffer buffer) = 0;

    virtual void Clear(Color color) = 0;
    virtual void Present() = 0;

    virtual void DrawLine(int32_t x0, int32_t y0, int32_t x1, int32_t y1, Color color) = 0;
    virtual void DrawThickLine(int32_t x0, int32_t y0, int32_t x1, int32_t y1, int32_t thickness, Color color) = 0;
    virtual void DrawCircle(int32_t x, int32_t y, int32_t radius, Color color) = 0;
    virtual void DrawRect(int32_t x, int32_t y, int32_t width, int32_t height, Color color) = 0;

    virtual void CopyTexture(ITexture* texture, int32_t x, int32_t y, int32_t width, int32_t height) = 0;
};
```

Class that plugin developer uses for creating textures.

```C++
class ITextureFactory {
  public:
    virtual ~ITextureFactory() {}

    virtual ITexture* CreateTexture(const char* filename) = 0;
    virtual ITexture* CreateTexture(int32_t width, int32_t height) = 0;
};
```

### GUI API

Classes responsible for reacting on clicking a button or moving a slider.

```C++
class IClickCallback {
  public:
    virtual ~IClickCallback() {}

    virtual void RespondOnClick() = 0;
};

class ISliderCallback {
public:
    virtual ~ISliderCallback() {}

    virtual void RespondOnSlide(float old_value, float current_value) = 0;
};
```

Classes implementing preferences widgets functionality.

```C++
class IWidget {
  public:
    virtual ~IWidget() {}

    virtual int32_t GetWidth() = 0;
    virtual int32_t GetHieght() = 0;
};

class IButton : public IWidget {
  public:
    virtual ~IButton() {}

    virtual void SetClickCallback(IClickCallback* callback) = 0;
};

class ISlider : public IWidget {
  public:
    virtual ~ISlider() {}

    virtual void SetSliderCallback(ISliderCallback* callback) = 0;

    virtual float GetValue() = 0;
    virtual void SetValue(float value) = 0;
};

class ILabel : public IWidget {
  public:
    virtual ~ILabel() {}

    virtual void SetText(const char* text) = 0;
};

class IPreferencesPanel : public IWidget {
  public:
    virtual ~IPreferencesPanel() {}

    virtual void Attach(IWidget* widget, int32_t x, int32_t y) = 0;
};
```

Class responsible for creating widgets.

```C++
class IWidgetFactory {
  public:
    virtual ~IWidgetFactory() {}

    virtual IButton* CreateDefaultButtonWithIcon(const char* icon_file_name) = 0;
    virtual IButton* CreateDefaultButtonWithText(const char* text) = 0;
    virtual IButton* CreateButtonWithIcon(int32_t width, int32_t height, const char* icon_file_name) = 0;
    virtual IButton* CreateButtonWithText(int32_t width, int32_t height, const char* text, int32_t char_size) = 0;

    virtual ISlider* CreateDefaultSlider(float range_min, float range_max) = 0;
    virtual ISlider* CreateSlider(int32_t width, int32_t height, float range_min, float range_max) = 0;

    virtual ISlider* CreateSlider(int32_t width, int32_t height, float thumb_width, float thumb_height, float range_min, float range_max) = 0;

    virtual ILabel*  CreateDefaultLabel(const char* text) = 0;
    virtual ILabel*  CreateLabel(int32_t width, int32_t height, const char* text, int32_t char_size) = 0;

    virtual IPreferencesPanel* CreateDefaultPreferencesPanel() = 0;
};
```

### API

Class which unites ```IWidgetFactory``` and ```ITextureFactory``` to implement app api in one class.

```C++
class IAPI {
  public:
    virtual ~IAPI() {}

    virtual IWidgetFactory*  GetWidgetFactory () = 0;
    virtual ITextureFactory* GetTextureFactory() = 0;
};
```

### Export classes

There are classes that plugin developer should implement and return them to app in ```Create``` function.

#### Filter interface

```C++
class IFilter {
  public:
    virtual ~IFilter() {}

    virtual void Apply(ITexture* canvas) = 0;
    virtual const char* GetName() const = 0;
    virtual IPreferencesPanel* GetPreferencesPanel() const = 0;
};
```

#### Tool interface

```ActionBegin``` should be called when mouse button is pressed,
```Action``` when mouse is moving and ```ActionEnd``` when mouse button is released.

```C++
class ITool {
  public:
    virtual ~ITool() {}

    virtual void ActionBegin(ITexture* canvas, int x, int y) = 0;
    virtual void Action     (ITexture* canvas, int x, int y, int dx, int dy) = 0;
    virtual void ActionEnd  (ITexture* canvas, int x, int y) = 0;

    virtual const char* GetIconFileName() const = 0;
    virtual IPreferencesPanel* GetPreferencesPanel() const = 0;
};
```

#### Plugin interface

```C++
class IPlugin {
  public:
    virtual ~IPlugin() {}

    virtual std::list<IFilter*> GetFilters() const = 0;
    virtual std::list<ITool*>   GetTools () const = 0;
};

typedef uint32_t (*VersionFunction)();
typedef IPlugin* (*CreateFunction)(IAPI* api);
typedef void     (*DestroyFunction)(IPlugin* plugin);

extern "C" uint32_t Version();
extern "C" IPlugin* Create (IAPI* api);
extern "C" void     Destroy(IPlugin* plugin);
```
