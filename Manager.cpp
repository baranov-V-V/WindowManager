#include "WindowManager.h"

Manager::Manager(int max_count) : size(max_count), count(0) {
    Manager::figures = (PhysShape**) calloc(max_count, sizeof(PhysShape*));
};

Manager::~Manager() {
    for (int i = 0; i < count; ++i) {
        this->delLast();
    }
    free(figures);
};

void Manager::drawChilds(const Renderer& renderer) const {
    for (int i = 0; i < count; ++i) {
        figures[i]->draw(renderer);
    }
};

void Manager::addFigure(PhysShape* figure) {
    if (count < size) {
        figures[count++] = figure;
    }
};

void Manager::delFigure(const PhysShape* figure) {
    int pos = 0;
    for (int i = 0; i < count; ++i) {
        if (figures[i] == figure) {
            for (int j = i; j < count - 1; ++j) {
                figures[j] = figures[j + 1];   
            }
            count--;
            break;
            delete figure;
        }
    }
};

void Manager::delLast() {
    figures[--count] = nullptr;
};

void Manager::moveAll(double time) const {
    for (int i = 0; i < count; ++i) {
        (figures[i])->move(time);
    } 
};