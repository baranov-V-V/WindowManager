#include "event/Events.h"

void Event::setType(int type) {
    event_type = type;
};

void Event::setData(Pair<int> abs_coord) {
    event_data.abs_coord = abs_coord;
};

void Event::setData(int scancode) {
    event_data.scancode = scancode;
};

void Event::setData(MouseData mouse_data) {
    event_data.mouse_data = mouse_data;
};