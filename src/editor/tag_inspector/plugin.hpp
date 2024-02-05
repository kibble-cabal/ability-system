#ifndef AS_TAGINSPECTORPLUGIN_H
#define AS_TAGINSPECTORPLUGIN_H

#include <godot_cpp/classes/editor_inspector_plugin.hpp>
#include <godot_cpp/classes/editor_plugin.hpp>
using namespace godot;

#include "../../ability.hpp"
#include "../../ability_system.h"
#include "../../effect/tag_effect.h"
#include "../../project_settings.h"
#include "property.h"

class TagInspectorPropertyPlugin: public EditorInspectorPlugin {
    GDCLASS(TagInspectorPropertyPlugin, EditorInspectorPlugin);

protected:
    static void _bind_methods() {}

public:
    bool _can_handle(Object *object) const override {
        return ASProjectSettings::get_tags_editor_enabled()
               && !any(
                   (Array)ASProjectSettings::get_tags_editor_exclude_classes(),
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
        if (!ASProjectSettings::get_tags_editor_enabled()) return false;
        bool is_prop_name = ASProjectSettings::get_tags_editor_property_names().has(name);
        if (type == Variant::Type::ARRAY && is_prop_name) {
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