#include "sphere.h"

Button::Button(int x_size, int y_size, int x, int y) :
    Frame(x_size, y_size), coord(x, y) {};

TextButton::TextButton(int x_size, int y_size, int x, int y, COLORREF color, char* text, VFunctor* functor) : 
    Button(x_size, y_size, x, y), color(color), functor(functor) {
    len = strlen(text);
    TextButton::text = new char[len + 1];
    strcpy(TextButton::text, text);
};

TextButton::TextButton() : 
    Button(0, 0, 0, 0), color(TX_WHITE) {
    len = 0;
    TextButton::text = nullptr;
};

TextButton::TextButton(const TextButton& source) : 
    Button(source.getSizeX(), source.getSizeY() , source.getCoordX(), source.getCoordY()), color(source.getColor()) {
    len = source.getLen();
    text = new char[len + 1];
    strcpy(text, source.getText());
};

TextButton::~TextButton() {
    delete[] text;
};

void TextButton::action() const {
    functor->action();
}

ButtonManager::ButtonManager(int max_count) : size(max_count), count(0) {
    ButtonManager::buttons = (Button**) calloc(max_count, sizeof(Button*));
};

ButtonManager::~ButtonManager() {
    for (int i = 0; i < count; ++i) {
        this->delLast();
    }
    free(buttons);
};

void ButtonManager::drawAll(const Renderer& renderer) const {
    for (int i = 0; i < count; ++i) {
        buttons[i]->draw(renderer);
    }
};

void ButtonManager::checkAll(const Renderer& render, Manager& manager) const {

    static clock_t time = 0;
    static bool is_pressed = false;

    if (is_pressed) {
        if (clock() - time < CLOCKS_PER_SEC * 0.2) {
            return;    
        }
        else {
            is_pressed = false;
        }
    }

    for (int i = 0; i < this->getCount(); ++i) {
        TextButton* button = (TextButton*) this->getButton(i);
        if (button->checkPress(render) && !is_pressed) {
            time = clock();
            is_pressed = true;
            button->action();
        }
    }
};

void ButtonManager::addButton(Button* button) {
    if (count < size) {
        buttons[count++] = button;
    }
};

void ButtonManager::delButton(const Button* button) {
    int pos = 0;
    for (int i = 0; i < count; ++i) {
        if (buttons[i] == button) {
            for (int j = i; j < count - 1; ++j) {
                buttons[j] = buttons[j + 1];   
            }
            count--;
            break;
        }
    }
};

void ButtonManager::delLast() {
    buttons[--count] = nullptr;
};

void AddCircle::action() {
    double x = Random(1, 9);
    double y = Random(1, 9);
    double r = 0.2;
    PhysSphere* tmp = new PhysSphere(x, y, r, Random(1, 3), Random(1, 3), 1, RGB(128, Random(0, 255), Random(0, 255)),
                                     RGB(20, 100, 40), RGB(0, 160, 150), vec3d(li_x, li_y, li_z), vec3d(vi_x, vi_y, vi_z));
    manager->addFigure(tmp);
}

void AddRect::action() {
    double x = Random(1, 9);
    double y = Random(1, 9);
    PhysRect* tmp = new PhysRect(x, y, x + 0.2, y + 0.2, Random(1, 3), Random(1, 3), Random(1, 3), RGB(255, Random(0, 255), Random(0, 255)));
    manager->addFigure(tmp);
}

void IncreaseTemp::action() {
    for (int i = 0; i < manager->getCount(); ++i) {
        PhysShape* shape = manager->get_figure(i);
        shape->setSpeed(shape->get_speed().getX() * 11 / 10, shape->get_speed().getY() * 11 / 10); 
    }
}

void DecreaseTemp::action() {
    for (int i = 0; i < manager->getCount(); ++i) {
        PhysShape* shape = manager->get_figure(i);
        shape->setSpeed(shape->get_speed().getX() * 9 / 10, shape->get_speed().getY() * 9 / 10); 
    }
}

void AdjustScale::action() {
    render->getCoordinates()->setMaxY(manager->getCount() * 2);
    render->calcScale();
}