#ifndef AS_ATTRIBUTEINSPECTORPLUGIN_H
#define AS_ATTRIBUTEINSPECTORPLUGIN_H

#include <godot_cpp/classes/editor_inspector_plugin.hpp>
#include <godot_cpp/classes/editor_plugin.hpp>

#include "property.h"
#include "../../ability_system.h"

using namespace godot;

class AttributeInspectorPropertyPlugin: public EditorInspectorPlugin {
    GDCLASS(AttributeInspectorPropertyPlugin, EditorInspectorPlugin);

protected:
    static void _bind_methods() {}

public:
    bool _can_handle(Object *object) const override {
        return Object::cast_to<AbilitySystem>(object) != nullptr;
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
        if (type == Variant::Type::DICTIONARY && name == "attributes") {
            AttributeInspectorProperty *property = memnew(AttributeInspectorProperty);
            add_property_editor(name, property);
            return true;
        }
        return false;
    }
};

class AttributeInspectorPlugin: public EditorPlugin {
    GDCLASS(AttributeInspectorPlugin, EditorPlugin);

private:
    EditorInspectorPlugin *plugin;

protected:
    static void _bind_methods() {}

public:
    void _enter_tree() override {
        plugin = memnew(AttributeInspectorPropertyPlugin);
        add_inspector_plugin(plugin);
    }

    void _exit_tree() override {
        remove_inspector_plugin(plugin);
    }
};

#endif