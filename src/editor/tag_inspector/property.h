#ifndef AS_TAGINSPECTORPROPERTY_H
#define AS_TAGINSPECTORPROPERTY_H

#ifndef ABILITY_SYSTEM_MODULE
#include <godot_cpp/classes/editor_property.hpp>
using namespace godot;
#endif

#include "editor.h"

class TagInspectorProperty : public EditorProperty {
    GDCLASS(TagInspectorProperty, EditorProperty);

private:
    TagInspectorEditor *editor;

protected:
    static void _bind_methods();

public:
    TagInspectorProperty();
    void _update_property() override;
    TypedArray<Tag> get_current_value();
};

#endif