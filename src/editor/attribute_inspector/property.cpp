#include "property.h"

#include "../../attribute.hpp"
#include "../../utils.hpp"

AttributeInspectorProperty::AttributeInspectorProperty() {
    editor = memnew(AttributeInspectorEditor);
    editor->on_add = [&](Ref<Attribute> attribute) {
        Dictionary current_value = get_current_value();
        current_value[attribute] = attribute->get_default_value();
        emit_changed(get_edited_property(), current_value);
    };
    editor->on_remove = [&](Ref<Attribute> attribute) {
        Dictionary current_value = get_current_value();
        current_value.erase(attribute);
        emit_changed(get_edited_property(), current_value);
    };
    editor->on_change = [&](Ref<Attribute> attribute, float value) {
        Dictionary current_value = get_current_value();
        current_value[attribute] = value;
        emit_changed(get_edited_property(), current_value);
    };
    add_child(editor);
    add_focusable(editor);
    set_bottom_editor(editor);
}

void AttributeInspectorProperty::_bind_methods() {}

Dictionary AttributeInspectorProperty::get_current_value() {
    return get_edited_object()->get(get_edited_property());
}

void AttributeInspectorProperty::_update_property() {
    editor->update(get_current_value());
}