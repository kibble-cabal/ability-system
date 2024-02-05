#ifndef AS_PROJECTSETTINGS_H
#define AS_PROJECTSETTINGS_H

#include <godot_cpp/classes/global_constants.hpp>
#include <godot_cpp/classes/project_settings.hpp>
#include <godot_cpp/core/error_macros.hpp>
#include <godot_cpp/variant/variant.hpp>

using namespace godot;

static void register_setting(
    const String& name,
    const Variant& value,
    bool needs_restart = false,
    PropertyHint hint = PROPERTY_HINT_NONE,
    const String& hint_string = ""
) {
    ProjectSettings* project_settings = ProjectSettings::get_singleton();

    if (!project_settings->has_setting(name)) project_settings->set(name, value);

    Dictionary property_info;
    property_info["name"] = name;
    property_info["type"] = value.get_type();
    property_info["hint"] = hint;
    property_info["hint_string"] = hint_string;

    project_settings->add_property_info(property_info);
    project_settings->set_initial_value(name, value);
    project_settings->set_restart_if_changed(name, needs_restart);
}

template <typename TType>
static TType get_setting(const char* setting) {
    const ProjectSettings* project_settings = ProjectSettings::get_singleton();
    const Variant setting_value = project_settings->get_setting_with_override(setting);
    const Variant::Type setting_type = setting_value.get_type();
    const Variant::Type expected_type = Variant(TType()).get_type();

    ERR_FAIL_COND_V_EDMSG(
        setting_type != expected_type,
        TType(),
        vformat(
            "Unexpected type for setting '%s'. Expected type '%s' but found '%s'.",
            setting,
            Variant::get_type_name(expected_type),
            Variant::get_type_name(setting_type)
        )
    );
    return setting_value;
}

class ASProjectSettings {
public:
    static void register_settings();
    static bool get_attributes_editor_enabled();
    static PackedStringArray get_attributes_editor_exclude_classes();
    static PackedStringArray get_attributes_editor_property_names();
    static bool get_tags_editor_enabled();
    static PackedStringArray get_tags_editor_exclude_classes();
    static PackedStringArray get_tags_editor_property_names();
};

#endif