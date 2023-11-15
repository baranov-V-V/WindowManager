#pragma once
#include "BasicInfo.h"
#include "Functors.h"
#include "NewPluginApi.h"

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

class VSlider {
  public:
    VSlider(float min_val, float max_val, VSliderAction* action = nullptr) : min_val(min_val), max_val(max_val), action(action) {};
    virtual ~VSlider() {};

    virtual void place(int new_coord) = 0;
    virtual int getCoord() const = 0;

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

    float calcValue(int coord) {
        return min_val + (max_val - min_val) / (max_bar - min_bar) * (coord - min_bar);
    }
    int calcCoord(float value) {
        return min_bar + (max_bar - min_bar) * (value - min_val) / (max_val - min_val);
    }
  protected:

    VSliderAction* action;

    float value;
    
    float min_val;
    float max_val;

    int min_bar;
    int max_bar;
    
    ManagerWindow* bar;
};

class SliderBasicX : public VSlider, public BorderWindow {
  public:
    SliderBasicX(int size_x, int size_y, int coord_x, int coord_y, float min_val, float max_val, VSliderAction* action = nullptr);
    SliderBasicX(int size_x, int size_y, int coord_x, int coord_y, float min_val, float max_val, int thumb_width, VSliderAction* action = nullptr);
    virtual ~SliderBasicX() { delete action; };

    virtual void place(int new_coord) override;
    virtual int getCoord() const override { return bar->getCoordX(); };

    ManagerWindow* getBar() const { return bar; };
    void SetSliderAction(VSliderAction* new_action) {
        if (action != nullptr) {
            delete action;
        }
        action = new_action;
    };

  private:
};

class SliderX : public VSlider, public InvisibleWindow {
  public:
    SliderX(int size_x, int size_y, int coord_x, int coord_y, float min_val, float max_val, float delta, VSliderAction* action = nullptr);
    virtual ~SliderX() { delete action; };

    virtual void place(int new_coord) override;
    virtual int getCoord() const override { return bar->getCoordX(); };

    ManagerWindow* getBar() const { return bar; };
    void SetSliderAction(VSliderAction* new_action) {
        if (action != nullptr) {
            delete action;
        }
        action = new_action;
    };

    void moveBarLeft() {
        this->place(this->calcCoord(this->getValue() - delta));
    }
    void moveBarRight() {
        this->place(this->calcCoord(this->getValue() + delta));
    } 

  private:
    float delta;
};