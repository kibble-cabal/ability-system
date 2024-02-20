#ifndef AS_WAITEFFECT_HPP
#define AS_WAITEFFECT_HPP

#include <godot_cpp/variant/utility_functions.hpp>

#include "../macros.hpp"
#include "effect.h"

class WaitEffect: public Effect {
    GDCLASS(WaitEffect, Effect);

private:
    float min_wait_time = 0.0;
    float max_wait_time = 1.0;
    float wait_time = 0.0;

protected:
    static void _bind_methods() {
        BIND_GETSET(WaitEffect, min_wait_time);
        BIND_GETSET(WaitEffect, max_wait_time);
        BIND_GETSET(WaitEffect, wait_time);

        PROP(Variant::FLOAT, min_wait_time);
        PROP(Variant::FLOAT, max_wait_time);
        NO_EDITOR_PROP(Variant::FLOAT, wait_time);
    }

public:
    GETSET_RESOURCE(float, min_wait_time)
    GETSET_RESOURCE(float, max_wait_time)
    GETSET(float, wait_time)

    virtual void _start(AbilitySystem *owner) override {
        wait_time = RANDF_RANGE(min_wait_time, max_wait_time);
    }

    virtual int _tick(AbilitySystem *owner, float delta) override {
        if (get_elapsed_time() >= wait_time) {
            return Status::FINISHED;
        }
        return Status::RUNNING;
    }
};

#endif