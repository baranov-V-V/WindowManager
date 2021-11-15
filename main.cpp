#include "App.h"

int main(int argc, const char* argv[]) {
    int x = 1196;
    int y = 690;

    if (argc == 3) {
        x = atoi(argv[1]);
        y = atoi(argv[2]);
    }

    App app(x, y);
    app.run();

    return 0;
}