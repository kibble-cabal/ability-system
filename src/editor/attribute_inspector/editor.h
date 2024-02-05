#ifndef AS_ATTRIBUTEINSPECTOREDITOR_H
#define AS_ATTRIBUTEINSPECTOREDITOR_H

#include <godot_cpp/classes/button.hpp>
#include <godot_cpp/classes/editor_property.hpp>
#include <godot_cpp/classes/editor_resource_picker.hpp>
#include <godot_cpp/classes/file_dialog.hpp>
#include <godot_cpp/classes/h_box_container.hpp>
#include <godot_cpp/classes/resource_loader.hpp>
#include <godot_cpp/classes/v_box_container.hpp>
#include <godot_cpp/variant/typed_array.hpp>

#include "../../attribute_map.hpp"
#include "editor_row.h"
#include "functional"

using namespace godot;

class Attribute;

class AttributeInspectorEditor: public VBoxContainer {
    GDCLASS(AttributeInspectorEditor, VBoxContainer);

private:
    AttributeHashMap<AttributeInspectorEditorRow *> row_nodes;
    HBoxContainer *h_box = memnew(HBoxContainer);
    Button *add_button = memnew(Button);
    EditorResourcePicker *picker = memnew(EditorResourcePicker);

public:
    std::function<void(Ref<Attribute>)> on_add = nullptr;
    std::function<void(Ref<Attribute>)> on_remove = nullptr;
    std::function<void(Ref<Attribute>, float)> on_change = nullptr;

private:
    void render_attribute(Ref<Attribute> attribute, float value);
    void _on_add_pressed();

protected:
    static void _bind_methods();

public:
    AttributeInspectorEditor();
    void _ready() override;
    void update(Dictionary new_value);
};

#endif