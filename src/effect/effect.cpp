
#include "effect.h"

#include "../ability_system.h"
#include "../utils.hpp"

void Effect::_bind_methods() {
    /* Bind virtual methods */
    BIND_VIRTUAL_METHOD(Effect, _start);
    BIND_VIRTUAL_METHOD(Effect, _tick);
    BIND_VIRTUAL_METHOD(Effect, _finish);

    /* Bind property getters/setters */
    BIND_GETSET(Effect, elapsed_time);
    BIND_GETSET(Effect, ui_name);
    BIND_GETSET(Effect, ui_color);

    /* Bind constants */
    ClassDB::bind_integer_constant(
        get_class_static(),
        "Status",
        "READY",
        Status::READY
    );
    ClassDB::bind_integer_constant(
        get_class_static(),
        "Status",
        "RUNNING",
        Status::RUNNING
    );
    ClassDB::bind_integer_constant(
        get_class_static(),
        "Status",
        "FINISHED",
        Status::FINISHED
    );

    /* Bind properties */
    NO_EDITOR_PROP(Variant::FLOAT, elapsed_time);

    ADD_GROUP("UI", "ui_");
    PROP(Variant::STRING_NAME, ui_name);
    PROP(Variant::COLOR, ui_color);
}

Status Effect::tick(AbilitySystem *owner, float delta) {
    elapsed_time += delta;
    last_status = (Status)_tick(owner, delta);
    return last_status;
}

void Effect::start(AbilitySystem *owner) {
    elapsed_time = 0;
    last_status = Status::READY;
    _start(owner);
    last_status = Status::RUNNING;
}

void Effect::finish(AbilitySystem *owner) {
    last_status = Status::FINISHED;
    _finish(owner);
}

String Effect::_to_string() const {
    if (String(ui_name).is_empty()) {
        return String("{0}()").format(variant_array(get_class()));
    }
    return String("{0}(\"{1}\")").format(variant_array(get_class(), ui_name));
}