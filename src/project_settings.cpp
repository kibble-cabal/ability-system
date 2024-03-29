#include "project_settings.h"

#include "utils.hpp"

namespace attributes_editor {
    static const auto ENABLED = "ability_system/attributes/editor_enabled";

    /// Defines a list of class names which will NOT use the attribute editor
    static const auto EXCLUDE_CLASSES = "ability_system/attributes/editor_exclude_classes";

    /// Defines a list of properties that will use the attributes editor
    static const auto PROPERTY_NAMES = "ability_system/attributes/editor_property_names";
}  // namespace attributes_editor

namespace tags_editor {
    static const auto ENABLED = "ability_system/tags/editor_enabled";

    /// Defines a list of class names which will NOT use the tags editor
    static const auto EXCLUDE_CLASSES = "ability_system/tags/editor_exclude_classes";

    /// Defines a list of properties that will use the tags editor
    static const auto PROPERTY_NAMES = "ability_system/tags/editor_property_names";
}  // namespace tags_editor

namespace csharp {
    static const auto GENERATION_DIRECTORY = "ability_system/csharp/generation_directory";
    static const auto GENERATION_FILE_NAME = "ability_system/csharp/generation_file_name";
}  // namespace csharp

void ASProjectSettings::register_settings() {
    register_setting(attributes_editor::ENABLED, true);
    register_setting(attributes_editor::EXCLUDE_CLASSES, PackedStringArray());
    register_setting(attributes_editor::PROPERTY_NAMES, string_array("attributes", "attrs"));
    register_setting(tags_editor::ENABLED, true);
    register_setting(tags_editor::EXCLUDE_CLASSES, PackedStringArray());
    register_setting(
        tags_editor::PROPERTY_NAMES,
        string_array("tags", "tags_blocking", "tags_required")
    );
    register_setting(csharp::GENERATION_DIRECTORY, "res://addons/ability_system/gen");
    register_setting(csharp::GENERATION_FILE_NAME, "AbilitySystemGeneratedAPI.cs");
}

bool ASProjectSettings::get_attributes_editor_enabled() {
    return get_setting<bool>(attributes_editor::ENABLED);
}

PackedStringArray ASProjectSettings::get_attributes_editor_exclude_classes() {
    return get_setting<PackedStringArray>(attributes_editor::EXCLUDE_CLASSES);
}

PackedStringArray ASProjectSettings::get_attributes_editor_property_names() {
    return get_setting<PackedStringArray>(attributes_editor::PROPERTY_NAMES);
}

bool ASProjectSettings::get_tags_editor_enabled() {
    return get_setting<bool>(tags_editor::ENABLED);
}

PackedStringArray ASProjectSettings::get_tags_editor_exclude_classes() {
    return get_setting<PackedStringArray>(tags_editor::EXCLUDE_CLASSES);
}

PackedStringArray ASProjectSettings::get_tags_editor_property_names() {
    return get_setting<PackedStringArray>(tags_editor::PROPERTY_NAMES);
}

String ASProjectSettings::get_csharp_generation_directory() {
    return get_setting<String>(csharp::GENERATION_DIRECTORY);
}

String ASProjectSettings::get_csharp_generation_file_name() {
    return get_setting<String>(csharp::GENERATION_FILE_NAME);
}

String ASProjectSettings::get_csharp_generation_path() {
    return get_csharp_generation_directory().path_join(get_csharp_generation_file_name());
}