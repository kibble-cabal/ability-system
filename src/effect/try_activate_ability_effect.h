#ifndef AS_TRYACTIVATEABILITYEFFECT_HPP
#define AS_TRYACTIVATEABILITYEFFECT_HPP

#include "../ability.hpp"
#include "effect.h"

class TryActivateAbilityEffect: public Effect {
    GDCLASS(TryActivateAbilityEffect, Effect);

private:
    Ref<Ability> ability;

protected:
    static void _bind_methods();

public:
    GETSET_RESOURCE(Ref<Ability>, ability)

    virtual void _start(AbilitySystem *owner) override;

    virtual int _tick(AbilitySystem *owner, float delta) override {
        return Status::FINISHED;
    }
};

#endif