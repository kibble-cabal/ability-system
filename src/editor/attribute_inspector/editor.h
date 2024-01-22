#ifndef AS_ATTRIBUTEINSPECTOREDITOR_H
#define AS_ATTRIBUTEINSPECTOREDITOR_H

#ifndef ABILITY_SYSTEM_MODULE
#include <godot_cpp/classes/v_box_container.hpp>
#include <godot_cpp/classes/h_box_container.hpp>
#include <godot_cpp/classes/button.hpp>
#include <godot_cpp/classes/editor_property.hpp>
#include <godot_cpp/classes/file_dialog.hpp>
#include <godot_cpp/variant/typed_array.hpp>
#include <godot_cpp/classes/resource_loader.hpp>

using namespace godot;
#endif

#include "../../attribute_map.hpp"
#include "functional"
#include "editor_row.h"

class Attribute;

class AttributeInspectorEditor : public VBoxContainer {
    GDCLASS(AttributeInspectorEditor, VBoxContainer);

private:
    AttributeHashMap<AttributeInspectorEditorRow *> row_nodes;
    FileDialog *file_dialog = memnew(FileDialog);
    Button *add_button = memnew(Button);

public:
    std::function<void (TypedArray<Attribute>)> on_add = nullptr;
    std::function<void (Ref<Attribute>)> on_remove = nullptr;
    std::function<void (Ref<Attribute>, float)> on_change = nullptr;

private:
    void render_attribute(Ref<Attribute> attribute, float value);
    void _on_add_pressed();
    void _on_file_selected(String paths);
    void _on_files_selected(PackedStringArray paths);

protected:
    static void _bind_methods();

public:
    AttributeInspectorEditor();
    void _ready() override;
    void update(Dictionary new_value);
};

#endif