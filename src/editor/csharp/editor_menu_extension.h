// Most of this code is borrowed from this addon, with some modifications:
// https://github.com/DmitriySalnikov/godot_debug_draw_3d

#pragma once

#include <godot_cpp/classes/editor_plugin.hpp>

#include "../../macros.hpp"

using namespace godot;

class AbilitySystemMenuExtensionPlugin: public EditorPlugin {
    GDCLASS(AbilitySystemMenuExtensionPlugin, EditorPlugin)
public:
    enum MenuItemId {
        GENERATE_CSHARP_BINDING,
    };

private:
    String menu_item_name = "Ability System";

protected:
    static void _bind_methods();

public:
    String _get_plugin_name() const override;
    void _enter_tree() override;
    void _exit_tree() override;

    void _on_id_pressed(MenuItemId id);

    AbilitySystemMenuExtensionPlugin() {}

    ~AbilitySystemMenuExtensionPlugin() {}
};

// Register but not expose to GDScript
VARIANT_ENUM_CAST(AbilitySystemMenuExtensionPlugin::MenuItemId);
