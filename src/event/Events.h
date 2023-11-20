#pragma once

#include "gui_core/widgets/BasicInfo.h"

enum EventType {
    EVENT_MOUSE_MOVE,
    EVENT_MOUSE_PRESSED_LC,
    EVENT_MOUSE_RELEASED_LC,
    EVENT_BUTTON_PRESSED,
    EVENT_MOUSE_PRESSED_RC,
    EVENT_MOUSE_RELEASED_RC
};

enum EventState {
    EVENT_NEW,
    EVENT_DONE,
    EVENT_IN_PROCESS 
};

struct MouseData {
    MouseData(const Pair<int> start, const Pair<int> d, bool is_pressed) : start(start), d(d), is_pressed(is_pressed) {};
    ~MouseData() {};

    Pair<int> start;
    Pair<int> d;
    bool is_pressed;
};

union EventData {
    int scancode;
    Pair<int> abs_coord;
    MouseData mouse_data;

    EventData() {};
    ~EventData() {};
};

class Event {
  public:
    void setType(int type);
    void setData(Pair<int> abs_coord);
    void setData(int scancode);
    void setData(MouseData mouse_data);

    int getType() const { return event_type; }
    EventData getData() const { return event_data; }

  private:
    int event_type;
    EventData event_data;
};