#include "editor_row.h"

AttributeInspectorEditorRow::AttributeInspectorEditorRow() {
    // Update remove button
    remove_button->set_text(String::utf8("✕"));
    remove_button->set_h_size_flags(SIZE_SHRINK_CENTER);
    remove_button->set_v_size_flags(SIZE_SHRINK_CENTER);
    add_child(remove_button);

    // Update name label
    name_label->set_v_size_flags(SIZE_SHRINK_CENTER);
    name_label->set_h_size_flags(SIZE_EXPAND_FILL);
    name_label->set_vertical_alignment(VerticalAlignment::VERTICAL_ALIGNMENT_CENTER);
    name_label->add_theme_stylebox_override("normal", style_box);
    name_label->add_theme_font_override("font", font);
    style_box->set_content_margin_all(4);
    style_box->set_corner_radius_all(4);
    font->set_variation_embolden(0.5);
    add_child(name_label);

    // Update range label
    range_label->set_v_size_flags(SIZE_EXPAND_FILL);
    range_label->set_h_size_flags(SIZE_FILL);
    range_label->set_vertical_alignment(VerticalAlignment::VERTICAL_ALIGNMENT_CENTER);
    range_label->set_anchors_and_offsets_preset(PRESET_RIGHT_WIDE);

    // Update spin box
    add_child(spin_box);
}

void AttributeInspectorEditorRow::_bind_methods() {
    ClassDB::bind_method(D_METHOD("_on_spin_box_changed", "value"), &AttributeInspectorEditorRow::_on_spin_box_changed);
    ClassDB::bind_method(D_METHOD("_on_remove_pressed"), &AttributeInspectorEditorRow::_on_remove_pressed);
}

void AttributeInspectorEditorRow::_ready() {
    remove_button->connect("pressed", callable_mp(this, &AttributeInspectorEditorRow::_on_remove_pressed));
    spin_box->connect("value_changed", callable_mp(this, &AttributeInspectorEditorRow::_on_spin_box_changed));
    name_label->add_child(range_label);
    set_attribute(attribute);
}

void AttributeInspectorEditorRow::set_attribute(Ref<Attribute> attribute) {
    this->attribute = attribute;

    if (attribute.is_null())
        return;

    Color ui_color = attribute->get_ui_color();
    Color contrast_color = contrast(ui_color);
    float min_value = attribute->get_min_value();
    float max_value = attribute->get_max_value();
    
    // Update name label
    name_label->set_text(attribute->get_identifier());
    name_label->add_theme_color_override("font_color", contrast_color);
    style_box->set_bg_color(attribute->get_ui_color());
    
    // Update range label
    range_label->set_text(String("({0} - {1}) ").format(variant_array(min_value, max_value)));
    range_label->add_theme_color_override("font_color", Color(contrast_color, 0.5));
    range_label->set_anchors_and_offsets_preset(PRESET_RIGHT_WIDE);

    // Update spin box
    spin_box->set_min(min_value);
    spin_box->set_max(max_value);
    spin_box->set_step((max_value - min_value) / 1000);
    spin_box->set_custom_arrow_step((max_value - min_value) / 100);
    spin_box->set_value(attribute->get_default_value());
}

void AttributeInspectorEditorRow::set_value(float value) {
    spin_box->set_value(value);
}


void AttributeInspectorEditorRow::_on_spin_box_changed(float new_value) {
    if (attribute.is_valid() && on_change != nullptr)
        on_change(attribute, new_value);
}

void AttributeInspectorEditorRow::_on_remove_pressed() {
    if (attribute.is_valid() && on_remove != nullptr)
        on_remove(attribute);
}