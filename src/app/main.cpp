#include "app/App.h"

App* App::instance = nullptr;

ResizeCursors cursors;

int main(int argc, const char* argv[]) {
    int x = 1196;
    int y = 690;

    DisableCursorInCmd();

    if (argc == 3) {
        x = atoi(argv[1]);
        y = atoi(argv[2]);
    }

    App::getInstance()->initBasicTools();
    
    App::getInstance()->initWindows();
    
    App::getInstance()->run();
    
    return 0;
}