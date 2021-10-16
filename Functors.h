//functors for windows

class VFunctor {
  public:
    virtual void action() = 0;
};

class DummyFunctor : public VFunctor {
  public:
    void action() override {};
};