#ifndef AS_ATTRIBUTEINSPECTORPROPERTY_H
#define AS_ATTRIBUTEINSPECTORPROPERTY_H

#ifndef ABILITY_SYSTEM_MODULE
#include <godot_cpp/classes/editor_property.hpp>
using namespace godot;
#endif

#include "editor.h"

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