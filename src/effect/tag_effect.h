#ifndef AS_TAGEFFECT_HPP
#define AS_TAGEFFECT_HPP

#include "../tag.hpp"
#include "../utils.hpp"
#include "effect.h"

enum Operation { ADD = 1, REMOVE = -1 };

class TagEffect: public Effect {
    GDCLASS(TagEffect, Effect);

private:
    TypedArray<Tag> tags;
    Operation operation = Operation::ADD;

protected:
    static void _bind_methods();

public:
    GETSET_RESOURCE(TypedArray<Tag>, tags)

    int get_operation() const {
        return operation;
    }

    void set_operation(int value) {
        operation = (Operation)value;
        emit_changed();
    }

    virtual void _start(AbilitySystem *owner) override;

    virtual int _tick(AbilitySystem *owner, float delta) override {
        return Status::FINISHED;
    }
};

#endif