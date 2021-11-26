#pragma once
#include "BasicInfo.h"

class BasicSliderX {
  public:
    
    float getValue() const { return value; };
    void setValue(float new_value) {
        value = new_value;

    }

  private:
    PlaceBar* placer;
    
    float value;
    float min_val;
    float max_val;
    InvisibleWindow* layout;
};