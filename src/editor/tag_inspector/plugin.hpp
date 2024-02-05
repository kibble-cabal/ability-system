#ifndef AS_TAGINSPECTORPLUGIN_H
#define AS_TAGINSPECTORPLUGIN_H

#include <godot_cpp/classes/editor_inspector_plugin.hpp>
#include <godot_cpp/classes/editor_plugin.hpp>
using namespace godot;

#include "../../ability.hpp"
#include "../../ability_system.h"
#include "../../effect/tag_effect.h"
#include "property.h"

class TagInspectorPropertyPlugin: public EditorInspectorPlugin {
    GDCLASS(TagInspectorPropertyPlugin, EditorInspectorPlugin);

protected:
    static void _bind_methods() {}

public:
    bool _can_handle(Object *object) const override {
        return (
            Object::cast_to<AbilitySystem>(object) != nullptr
            || Object::cast_to<Ability>(object) != nullptr
            || Object::cast_to<TagEffect>(object) != nullptr
        );
    }

    bool _parse_property(
        Object *object,
        Variant::Type type,
        const String &name,
        PropertyHint hint_type,
        const String &hint_string,
        BitField<PropertyUsageFlags> usage_flags,
        bool wide
    ) override {
        if (type == Variant::Type::ARRAY
            && (name == "tags" || name == "tags_blocking"
                || name == "tags_required")) {
            TagInspectorProperty *property = memnew(TagInspectorProperty);
            add_property_editor(name, property);
            return true;
        }
        return false;
    }
};

class TagInspectorPlugin: public EditorPlugin {
    GDCLASS(TagInspectorPlugin, EditorPlugin);

private:
    EditorInspectorPlugin *plugin;

protected:
    static void _bind_methods() {}

public:
    void _enter_tree() override {
        plugin = memnew(TagInspectorPropertyPlugin);
        add_inspector_plugin(plugin);
    }

    void _exit_tree() override {
        remove_inspector_plugin(plugin);
    }
};

#endif