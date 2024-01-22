#ifndef AS_ATTRIBUTEINSPECTORPROPERTY_H
#define AS_ATTRIBUTEINSPECTORPROPERTY_H

#include <godot_cpp/classes/editor_property.hpp>

#include "editor.h"

using namespace godot;

class AttributeInspectorProperty : public EditorProperty {
    GDCLASS(AttributeInspectorProperty, EditorProperty);

private:
    AttributeInspectorEditor *editor;

protected:
    static void _bind_methods();

public:
    AttributeInspectorProperty();
    void _update_property() override;
    Dictionary get_current_value();
};

#endif