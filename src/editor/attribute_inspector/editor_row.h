#ifndef AS_ATTRIBUTEINSPECTOREDITORROW_H
#define AS_ATTRIBUTEINSPECTOREDITORROW_H

#ifndef ABILITY_SYSTEM_MODULE
#include <godot_cpp/classes/h_box_container.hpp>
#include <godot_cpp/classes/button.hpp>
#include <godot_cpp/classes/label.hpp>
#include <godot_cpp/classes/style_box_flat.hpp>
#include <godot_cpp/classes/spin_box.hpp>
#include <godot_cpp/classes/font_variation.hpp>

using namespace godot;
#endif

#include "../../attribute_map.hpp"
#include "functional"

class AttributeInspectorEditorRow : public HBoxContainer {
    GDCLASS(AttributeInspectorEditorRow, HBoxContainer);

private:
    Ref<FontVariation> font = memnew(FontVariation);
    Ref<StyleBoxFlat> style_box = memnew(StyleBoxFlat);
    Button *remove_button = memnew(Button);
    Label *name_label = memnew(Label);
    Label *range_label = memnew(Label);
    SpinBox *spin_box = memnew(SpinBox);
    Ref<Attribute> attribute;

public:
    std::function<void (Ref<Attribute>)> on_remove = nullptr;
    std::function<void (Ref<Attribute>, float)> on_change = nullptr;

private:
    void _on_remove_pressed();
    void _on_spin_box_changed(float value);

protected:
    static void _bind_methods();

public:
    AttributeInspectorEditorRow();
    void _ready() override;
    void set_attribute(Ref<Attribute> attribute);
    void set_value(float value);
};


#endif