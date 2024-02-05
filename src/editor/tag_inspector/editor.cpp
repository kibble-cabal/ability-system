#include "editor.h"

#include <godot_cpp/classes/label.hpp>
#include <godot_cpp/classes/panel_container.hpp>
#include <godot_cpp/classes/style_box_flat.hpp>

#include "../../tag.hpp"

TagInspectorEditor::TagInspectorEditor() {
    add_theme_constant_override("separation", 0);

    // Update tag container
    tag_container->set_h_size_flags(SIZE_EXPAND_FILL);
    add_child(tag_container);

    // Update resource picker & add button container
    h_box->set_h_size_flags(SIZE_EXPAND_FILL);
    add_child(h_box);

    // Update resource picker
    picker->set_h_size_flags(SIZE_EXPAND_FILL);
    picker->set_base_type("Tag");
    picker->set_toggle_mode(true);

    // Update button
    add_button->set_text("Add +");
}

void TagInspectorEditor::_ready() {
    h_box->add_child(picker);
    h_box->add_child(add_button);
    add_button->connect(
        "pressed",
        callable_mp(this, &TagInspectorEditor::_on_add_pressed)
    );
}

void TagInspectorEditor::_bind_methods() {
    ClassDB::bind_method(
        D_METHOD("_on_add_pressed"),
        &TagInspectorEditor::_on_add_pressed
    );
    ClassDB::bind_method(
        D_METHOD("_on_remove_pressed", "tag"),
        &TagInspectorEditor::_on_remove_pressed
    );
}

void TagInspectorEditor::render_tag(Ref<Tag> tag) {
    for (int i = 0; i < tag_container->get_child_count(); i++) {
        Node *child = tag_container->get_child(i);
        if (child->get_meta("tag_identifier") == tag->get_identifier()) return;
    }
    // Create panel style box
    Ref<StyleBoxFlat> style_box = memnew(StyleBoxFlat);
    style_box->set_bg_color(Color(tag->get_ui_color(), 0.15));
    style_box->set_content_margin_all(1.0);
    style_box->set_corner_radius_all(4.0);
    style_box->set_border_color(tag->get_ui_color());
    style_box->set_border_width_all(1.0);

    // Create panel
    PanelContainer *panel = memnew(PanelContainer);
    panel->set_meta("tag_identifier", tag->get_identifier());
    panel->add_theme_stylebox_override("panel", style_box);
    tag_container->add_child(panel);

    // Create tag children container
    HBoxContainer *tag_h_box = memnew(HBoxContainer);
    tag_h_box->add_theme_constant_override("separation", 2.0);
    panel->add_child(tag_h_box);

    // Create tag label
    Label *tag_label = memnew(Label);
    tag_label->set_text(tag->get_identifier());
    tag_label->add_theme_color_override("font_color", tag->get_ui_color());
    tag_h_box->add_child(tag_label);

    // Create tag remove button
    Button *tag_remove_button = memnew(Button);
    tag_remove_button->set_text(String::utf8("✕"));
    tag_remove_button->set_flat(true);
    tag_remove_button->add_theme_color_override(
        "font_color",
        tag->get_ui_color()
    );
    tag_remove_button->connect(
        "pressed",
        Callable(this, "_on_remove_pressed").bind(tag)
    );
    tag_h_box->add_child(tag_remove_button);
}

void TagInspectorEditor::_on_add_pressed() {
    Ref<Resource> resource = picker->get_edited_resource();
    if (resource.is_valid()) {
        Ref<Tag> tag = (Ref<Tag>)resource;
        if (tag != nullptr) on_add({tag});
        picker->set_edited_resource(nullptr);
    }
}

void TagInspectorEditor::_on_remove_pressed(Ref<Tag> tag) {
    on_remove(tag);
}

void TagInspectorEditor::update(TypedArray<Tag> new_value) {
    // Render or re-render all tags.
    for_each(new_value, [&](Ref<Tag> tag) { render_tag(tag); });

    // Remove tags that are not in the new value.
    for (int i = 0; i < tag_container->get_child_count(); i++) {
        Node *child = tag_container->get_child(i);
        String child_tag_identifier = child->get_meta("tag_identifier");
        bool is_in_new_value = false;
        for (int j = 0; j < new_value.size(); j++) {
            Ref<Tag> tag = new_value[j];
            if (tag->get_identifier() == child_tag_identifier) {
                is_in_new_value = true;
                break;
            }
        }
        if (!is_in_new_value) child->queue_free();
    }
}