#include "editor.h"

AttributeInspectorEditor::AttributeInspectorEditor() {
    add_theme_constant_override("separation", 0);

    // Update file dialog
    file_dialog->set_file_mode(FileDialog::FileMode::FILE_MODE_OPEN_FILES);
    file_dialog->set_filters(variant_array("*.tres", "*.res"));
    add_child(file_dialog);

    // Update button
    add_button->set_text("Add +");
    add_child(add_button);
}

void AttributeInspectorEditor::_ready() {
    file_dialog->connect("files_selected", callable_mp(this, &AttributeInspectorEditor::_on_files_selected));
    add_button->connect("pressed", callable_mp(this, &AttributeInspectorEditor::_on_add_pressed));
}

void AttributeInspectorEditor::_bind_methods() {
    ClassDB::bind_method(D_METHOD("_on_add_pressed"), &AttributeInspectorEditor::_on_add_pressed);
    ClassDB::bind_method(D_METHOD("_on_file_selected", "path"), &AttributeInspectorEditor::_on_file_selected);
    ClassDB::bind_method(D_METHOD("_on_files_selected", "paths"), &AttributeInspectorEditor::_on_files_selected);
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
    file_dialog->show();
}

void AttributeInspectorEditor::_on_files_selected(PackedStringArray paths) {
    if (on_add == nullptr)
        return;
    TypedArray<Attribute> attributes_to_add;
    for (String path: paths) {
        Ref<Resource> resource = ResourceLoader::get_singleton()->load(path);
        if (auto attribute = Object::cast_to<Attribute>(resource.ptr())) {
            attributes_to_add.push_back(attribute);
        }
    }
    on_add(attributes_to_add);
}

void AttributeInspectorEditor::_on_file_selected(String path) {
    _on_files_selected(variant_array(path));
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