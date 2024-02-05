#ifndef AS_ATTRIBUTEINSPECTORPLUGIN_H
#define AS_ATTRIBUTEINSPECTORPLUGIN_H

#include <godot_cpp/classes/editor_inspector_plugin.hpp>
#include <godot_cpp/classes/editor_plugin.hpp>

#include "../../ability_system.h"
#include "../../project_settings.h"
#include "property.h"

using namespace godot;

class AttributeInspectorPropertyPlugin: public EditorInspectorPlugin {
    GDCLASS(AttributeInspectorPropertyPlugin, EditorInspectorPlugin);

protected:
    static void _bind_methods() {}

public:
    bool _can_handle(Object *object) const override {
        return ASProjectSettings::get_attributes_editor_enabled()
               && !any(
                   (Array)ASProjectSettings::get_attributes_editor_exclude_classes(),
                   [&](String excluded_class) {
                       return ClassDB::is_parent_class(object->get_class(), excluded_class);
                   }
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
        if (!ASProjectSettings::get_attributes_editor_enabled()) return false;
        bool is_prop_name = ASProjectSettings::get_attributes_editor_property_names().has(name);
        if (type == Variant::Type::DICTIONARY && is_prop_name) {
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