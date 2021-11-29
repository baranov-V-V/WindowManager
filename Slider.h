#pragma once
#include "BasicInfo.h"
#include "Functors.h"
#include "PluginApi.h"

class VSliderAction {
  public:
    virtual ~VSliderAction() {}

    virtual void RespondOnSlide(float old_value, float current_value) = 0;
};

class SliderAction : public VSliderAction {
  public:
    SliderAction(float min_val, float max_val) : min_val(min_val), max_val(max_val) {};
    virtual ~SliderAction() {}

    virtual void RespondOnSlide(float old_value, float cur_value) override {
        std::cout << "cur_val: " << cur_value << "\n";
    };

  private:
    float max_val;
    float min_val;
};

class SliderCallbackAction : public VSliderAction {
  public:
    SliderCallbackAction(plugin::ISliderCallback* callback) : callback(callback) {}; 
    virtual ~SliderCallbackAction() { delete callback; };

    virtual void RespondOnSlide(float old_value, float cur_value) override {
        callback->RespondOnSlide(old_value, cur_value);
    }

  private:
    plugin::ISliderCallback* callback;
};

class BasicSliderX : public BorderWindow {
  public:
    BasicSliderX(int size_x, int size_y, int coord_x, int coord_y, float min_val, float max_val, VSliderAction* action = nullptr);
    BasicSliderX(int size_x, int size_y, int coord_x, int coord_y, float min_val, float max_val, int thumb_width, VSliderAction* action = nullptr);
    virtual ~BasicSliderX() { delete action; };

    void place(int new_coord);
    int getCoord() const { return bar->getCoordX(); };

    float getValue() const { return value; };
    void setValue(float new_value) {
        action->RespondOnSlide(value, new_value);
        value = new_value;
    } 

    ManagerWindow* getBar() const { return bar; };
    void SetSliderAction(VSliderAction* new_action) {
        if (action != nullptr) {
            delete action;
        }
        action = new_action;
    };

  private:
    float calcValue(int coord) {
        return min_val + (max_val - min_val) / (max_bar_x - min_bar_x) * (coord - min_bar_x);
    }

    VSliderAction* action;

    float value;
    
    float min_val;
    float max_val;

    int min_bar_x;
    int max_bar_x;
    
    ManagerWindow* bar;
};