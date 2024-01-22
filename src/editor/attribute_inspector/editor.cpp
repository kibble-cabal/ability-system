#include "editor.h"
#include <godot_cpp/variant/utility_functions.hpp>

AttributeInspectorEditor::AttributeInspectorEditor() {
    add_theme_constant_override("separation", 0);

    // Update resource picker & add button container
    h_box->set_h_size_flags(SIZE_EXPAND_FILL);
    add_child(h_box);

    // Update resource picker
    picker->set_h_size_flags(SIZE_EXPAND_FILL);
    picker->set_base_type("Attribute");
    picker->set_toggle_mode(true);

    // Update button
    add_button->set_text("Add +");
}

void AttributeInspectorEditor::_ready() {
    h_box->add_child(picker);
    h_box->add_child(add_button);
    add_button->connect("pressed", callable_mp(this, &AttributeInspectorEditor::_on_add_pressed));
}

void AttributeInspectorEditor::_bind_methods() {
    ClassDB::bind_method(D_METHOD("_on_add_pressed"), &AttributeInspectorEditor::_on_add_pressed);
}

void AttributeInspectorEditor::render_attribute(Ref<Attribute> attribute, float value) {
    if (row_nodes.has(attribute))
        row_nodes.get(attribute)->set_value(value);
    else {
        AttributeInspectorEditorRow *new_row = memnew(AttributeInspectorEditorRow);
        new_row->on_change = on_change;
        new_row->on_remove = on_remove;
        new_row->set_attribute(attribute);
        new_row->set_value(value);
        row_nodes.insert(attribute, new_row);
        add_child(new_row);
    }
}

void AttributeInspectorEditor::_on_add_pressed() {
    Ref<Resource> resource = picker->get_edited_resource();
    if (resource.is_valid()) {
        Ref<Attribute> attribute = (Ref<Attribute>)resource;
        if (attribute != nullptr) 
            on_add({ attribute });
        picker->set_edited_resource(nullptr);
    }
}

void AttributeInspectorEditor::update(Dictionary new_value) {
    // Render or re-render all attributes.
    for_each(new_value.keys(), [&](Ref<Attribute> attribute) {
        render_attribute(attribute, new_value[attribute]);
    });

    // Remove attributes that are not in the new value.
    std::vector<Ref<Attribute>> attributes_to_erase;
    for (auto pair : row_nodes) {
        if (!new_value.has(pair.key)) {
            pair.value->queue_free();
            attributes_to_erase.push_back(pair.key);
        }
    }
    for (auto attribute : attributes_to_erase)
        row_nodes.erase(attribute);
}