#pragma once
#include "BasicInfo.h"

class VFunctor {
  public:
    virtual ~VFunctor() {};
    virtual bool action() = 0;
};

class MoveFunctor : public VFunctor {
  public:
    MoveFunctor();
    MoveFunctor(ManagerWindow* window, WindowMouse* mouse) : move_window(window), mouse(mouse), on_move(false), old_coord(0, 0) {};
    virtual ~MoveFunctor() {};

    void startMove() { on_move = true; old_coord = mouse->getAbsCoord(); };
    void endMove() { on_move = false; };

    bool action() override {
        if (on_move) {
            move_window->setCoord(move_window->getCoord() + (mouse->getAbsCoord() - old_coord));
            old_coord = mouse->getAbsCoord();
        }

        return true;
    };

  protected:
    ManagerWindow* move_window;
    WindowMouse* mouse;
    Pair<int> old_coord;
    bool on_move;
};

class StartMove : public VFunctor {
  public:
    StartMove();
    StartMove(MoveFunctor* move_f, ManagerWindow* move_window) : move_f(move_f), move_window(move_window) {};
    virtual ~StartMove() {};

    bool action() override { move_window->getParent()->makeFirst(move_window); move_f->startMove(); return true; };

  protected:
    MoveFunctor* move_f;

  private:
    ManagerWindow* move_window;
};

class EndMove : public VFunctor {
  public:
    EndMove();
    EndMove(MoveFunctor* move_f) : move_f(move_f) {};
    virtual ~EndMove() {};

    bool action() override { move_f->endMove(); return true; };

  private:
    MoveFunctor* move_f;
};

class PlaceBar {
  public:
    PlaceBar() {};
    PlaceBar(int min_coord, int max_coord, char type, ManagerWindow* bar, VFunctor* action) : bar(bar), min_coord(min_coord), max_coord(max_coord), action(action), type(type) {};
    ~PlaceBar() {};

    void place(int new_coord) {
        if (new_coord < min_coord) {
            new_coord = min_coord;
        }
        if (new_coord > max_coord) {
            new_coord = max_coord;
        }
        if (type == 'X') {
            bar->setCoordX(new_coord);
        } else if (type == 'Y') {
            bar->setCoordY(new_coord);
        }
        action->action();
    }

    ManagerWindow* getBar() const { return bar; };

  private: 
    VFunctor* action;
    char type;
    int min_coord;
    int max_coord;
    ManagerWindow* bar;
};

class MoveBarLeft : public VFunctor {
  public:
    MoveBarLeft(PlaceBar* placer) : placer(placer) {};
    virtual ~MoveBarLeft() {};

    bool action() override {
        ManagerWindow* bar = placer->getBar();
        placer->place(bar->getCoordX() - 5);
        return true;
    };

  private:
    PlaceBar* placer;  
};

class MoveBarRight : public VFunctor {
  public:
    MoveBarRight(PlaceBar* placer) : placer(placer) {};
    virtual ~MoveBarRight() {};

    bool action() override {
        ManagerWindow* bar = placer->getBar();
        placer->place(bar->getCoordX() + 5);
        return true;
    };

  private:
    PlaceBar* placer;  
};

class MoveBarUp : public VFunctor {
  public:
    MoveBarUp(PlaceBar* placer) : placer(placer) {};
    virtual ~MoveBarUp() {};

    bool action() override {
        ManagerWindow* bar = placer->getBar();
        placer->place(bar->getCoordY() - 5);
        return true;
    };

  private:
    PlaceBar* placer;  
};

class MoveBarDown : public VFunctor {
  public:
    MoveBarDown(PlaceBar* placer) : placer(placer) {};
    virtual ~MoveBarDown() {};

    bool action() override {
        ManagerWindow* bar = placer->getBar();
        placer->place(bar->getCoordY() + 5);
        return true;
    };

  private:
    PlaceBar* placer;  
};

class PlaceBarOnClickX : public StartMove {
  public:
    PlaceBarOnClickX() {};
    PlaceBarOnClickX(PlaceBar* placer, ManagerWindow* window, WindowMouse* mouse, MoveFunctor* functor) : placer(placer), mouse(mouse), StartMove(functor, nullptr) {};
    virtual ~PlaceBarOnClickX() {};

    bool action() override {
        ManagerWindow* bar = placer->getBar();
        placer->place(mouse->getRelCoord().x + placer->getBar()->getSizeX() / 2);
        move_f->startMove();
        return true;
    };

  private:
    PlaceBar* placer;
    WindowMouse* mouse;
};

class PlaceBarOnClickY : public StartMove {
  public:
    PlaceBarOnClickY() {};
    PlaceBarOnClickY(PlaceBar* placer, ManagerWindow* window, WindowMouse* mouse, MoveFunctor* functor) : placer(placer), mouse(mouse), StartMove(functor, nullptr) {};
    virtual ~PlaceBarOnClickY() {};

    bool action() override {
        ManagerWindow* bar = placer->getBar();
        placer->place(mouse->getRelCoord().y + placer->getBar()->getSizeY() / 2);
        move_f->startMove();
        return true;
    };

  private:
    PlaceBar* placer;
    WindowMouse* mouse;
};

class MoveBarRandomX : public MoveFunctor {
  public:
    MoveBarRandomX() {};
    MoveBarRandomX(PlaceBar* placer, ManagerWindow* window, WindowMouse* mouse) : placer(placer), MoveFunctor(window, mouse) {};

    bool action() override {
        if (on_move) {
            placer->place(placer->getBar()->getCoordX() + (mouse->getAbsCoord().x - old_coord.x));
            old_coord = mouse->getAbsCoord();
        }
        if (on_move && !mouse->isLeftClick()) {
            on_move = false;
        }
        return true;
    };

  private:
    PlaceBar* placer;
};

class MoveBarRandomY : public MoveFunctor {
  public:
    MoveBarRandomY() {};
    MoveBarRandomY(PlaceBar* placer, ManagerWindow* window, WindowMouse* mouse) : placer(placer), MoveFunctor(window, mouse) {};

    bool action() override {
        if (on_move && mouse->isLeftClick()) {
            placer->place(placer->getBar()->getCoordY() + (mouse->getAbsCoord().y - old_coord.y));
        }
        if (on_move && !mouse->isLeftClick()) {
            on_move = false;
        }
        return true;
    };

  private:
    PlaceBar* placer;
};

class MakeFirst : public VFunctor {
  public:
    MakeFirst();
    MakeFirst(ManagerWindow* window) : window(window) {};
    virtual ~MakeFirst() {};

    bool action() override { window->getParent()->makeFirst(window); return true; };

  private:
    ManagerWindow* window;
};