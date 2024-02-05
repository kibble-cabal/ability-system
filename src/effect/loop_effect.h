#ifndef AS_LOOPEFFECT_HPP
#define AS_LOOPEFFECT_HPP

#include "../utils.hpp"
#include "effect.h"

class LoopEffect: public Effect {
    GDCLASS(LoopEffect, Effect);

private:
    int max_loops = -1;
    int elapsed_loops = 0;

protected:
    static void _bind_methods() {
        BIND_GETSET(LoopEffect, max_loops);
        BIND_GETSET(LoopEffect, elapsed_loops);
        PROP(Variant::INT, max_loops);
        NO_EDITOR_PROP(Variant::INT, elapsed_loops);
    }

public:
    GETSET_RESOURCE(int, max_loops)
    GETSET_RESOURCE(int, elapsed_loops)

    virtual void _start(AbilitySystem *owner) override {}

    virtual int _tick(AbilitySystem *owner, float delta) override {
        elapsed_loops += 1;
        if (has_finished_looping()) return Status::FINISHED;
        return Status::RUNNING;
    }

    virtual bool is_loop_effect() override {
        return true;
    }

    bool has_finished_looping() {
        return elapsed_loops >= max_loops && max_loops >= 0;
    }
};

#endif