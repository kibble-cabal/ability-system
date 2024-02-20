// Most of this code is borrowed from this addon, with some modifications:
// https://github.com/DmitriySalnikov/godot_debug_draw_3d

#include "editor_menu_extension.h"

#include <godot_cpp/classes/image.hpp>
#include <godot_cpp/classes/image_texture.hpp>
#include <godot_cpp/classes/os.hpp>
#include <godot_cpp/classes/popup_menu.hpp>

#include "generate_csharp_bindings.h"

void AbilitySystemMenuExtensionPlugin::_bind_methods() {
    ClassDB::bind_method(
        D_METHOD(nameof(_on_id_pressed), "id"),
        &AbilitySystemMenuExtensionPlugin::_on_id_pressed
    );
}

inline String AbilitySystemMenuExtensionPlugin::_get_plugin_name() const {
    return String("Generate C# Binding for AbilitySystem");
}

void AbilitySystemMenuExtensionPlugin::_enter_tree() {
    PopupMenu *menu = memnew(PopupMenu);
    menu->connect("id_pressed", Callable(this, nameof(_on_id_pressed)));
    menu->add_item("Generate C# bindings", GENERATE_CSHARP_BINDING);

    add_tool_submenu_item(menu_item_name, menu);
}

void AbilitySystemMenuExtensionPlugin::_exit_tree() {
    remove_tool_menu_item(menu_item_name);
}

void AbilitySystemMenuExtensionPlugin::_on_id_pressed(MenuItemId id) {
    switch (id) {
        case AbilitySystemMenuExtensionPlugin::GENERATE_CSHARP_BINDING: {
            GenerateCSharpBindingsPlugin().generate();
            break;
        }
        default:
            print_error("Menu item " + String::num_int64(id) + " not implemented.");
            break;
    }
}
