#include "property.h"

#include "../../tag.hpp"
#include "../../utils.hpp"

TagInspectorProperty::TagInspectorProperty() {
    editor = memnew(TagInspectorEditor);
    editor->on_add = [&](Ref<Tag> tag) {
        TypedArray<Tag> current_value = get_current_value();
        current_value.append(tag);
        emit_changed(get_edited_property(), current_value);
    };
    editor->on_remove = [&](Ref<Tag> tag) {
        TypedArray<Tag> current_value = get_current_value();
        current_value.erase(tag);
        emit_changed(get_edited_property(), current_value);
    };
    add_child(editor);
    add_focusable(editor);
    set_bottom_editor(editor);
}

void TagInspectorProperty::_bind_methods() {}

TypedArray<Tag> TagInspectorProperty::get_current_value() {
    return get_edited_object()->get(get_edited_property());
}

void TagInspectorProperty::_update_property() {
    editor->update(get_current_value());
}