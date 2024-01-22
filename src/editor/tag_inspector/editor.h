#ifndef AS_TAGINSPECTOREDITOR_H
#define AS_TAGINSPECTOREDITOR_H

#include <godot_cpp/classes/v_box_container.hpp>
#include <godot_cpp/classes/h_box_container.hpp>
#include <godot_cpp/classes/h_flow_container.hpp>
#include <godot_cpp/classes/button.hpp>
#include <godot_cpp/variant/typed_array.hpp>
#include <godot_cpp/classes/editor_resource_picker.hpp>

#include "functional"

using namespace godot;

class Tag;

class TagInspectorEditor : public VBoxContainer {
    GDCLASS(TagInspectorEditor, VBoxContainer);

private:
    HBoxContainer *h_box = memnew(HBoxContainer);
    Button *add_button = memnew(Button);
    EditorResourcePicker *picker = memnew(EditorResourcePicker);
    HFlowContainer *tag_container = memnew(HFlowContainer);

public:
    std::function<void (Ref<Tag>)> on_add = nullptr;
    std::function<void (Ref<Tag>)> on_remove = nullptr;

private:
    void render_tag(Ref<Tag> tag);
    void _on_add_pressed();
    void _on_remove_pressed(Ref<Tag> tag);

protected:
    static void _bind_methods();

public:
    TagInspectorEditor();
    void _ready() override;
    void update(TypedArray<Tag> new_value);
};

#endif