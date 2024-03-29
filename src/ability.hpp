#ifndef AS_ABILITY_HPP
#define AS_ABILITY_HPP

#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/variant/typed_array.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

#include "effect/effect.h"
#include "macros.hpp"
#include "status.hpp"
#include "tag.hpp"
#include "utils.hpp"

using namespace godot;

class AbilitySystem;

enum EffectMode { PARALLEL = 1, SEQUENTIAL = 2 };

static const auto EffectModePropertyHint = "Parallel:1,Sequential:2";

class Ability: public Resource {
    GDCLASS(Ability, Resource)

    friend class AbilityEvent;

private:
    StringName identifier = "";

    /* Tags */
    TypedArray<Tag> tags_blocking;
    TypedArray<Tag> tags_required;

    /* Effects */
    TypedArray<Effect> effects;
    EffectMode effect_mode = EffectMode::PARALLEL;

    /* UI properties */
    Color ui_color = Color(1.0, 1.0, 1.0);

protected:
    static void _bind_methods() {
        /* Bind methods */
        BIND_GETSET(Ability, identifier);
        BIND_GETSET(Ability, tags_blocking);
        BIND_GETSET(Ability, tags_required);
        BIND_GETSET(Ability, effects);
        BIND_GETSET(Ability, effect_mode);
        BIND_GETSET(Ability, ui_color);

        /* Bind constants */
        ClassDB::bind_integer_constant(
            get_class_static(),
            "Mode",
            "PARALLEL",
            EffectMode::PARALLEL
        );
        ClassDB::bind_integer_constant(
            get_class_static(),
            "Mode",
            "SEQUENTIAL",
            EffectMode::SEQUENTIAL
        );

        /* Bind properties */
        PROP(Variant::STRING_NAME, identifier);

        ADD_GROUP("Tags", "tags_");
        ARRAY_PROP(tags_blocking, RESOURCE_TYPE_HINT("Tag"));
        ARRAY_PROP(tags_required, RESOURCE_TYPE_HINT("Tag"));

        GROUP("Effects");
        ARRAY_PROP(effects, RESOURCE_TYPE_HINT("Effect"));
        ClassDB::add_property(
            get_class_static(),
            PropertyInfo(Variant::INT, "effect_mode", PROPERTY_HINT_ENUM, EffectModePropertyHint),
            "set_effect_mode",
            "get_effect_mode"
        );

        ADD_GROUP("UI", "ui_");
        PROP(Variant::COLOR, ui_color);
    }

public:
    GETSET_RESOURCE(StringName, identifier)
    GETSET_RESOURCE(TypedArray<Tag>, tags_blocking)
    GETSET_RESOURCE(TypedArray<Tag>, tags_required)
    GETSET_RESOURCE(TypedArray<Effect>, effects)

    GETSET_RESOURCE(Color, ui_color)

    int get_effect_mode() const {
        return (int)effect_mode;
    }

    void set_effect_mode(int value) {
        effect_mode = (EffectMode)value;
        emit_changed();
    }

    String _to_string() const {
        return fmt("{0}(\"{1}\")", get_class(), this->get_identifier());
    }
};

#endif