#ifndef AS_ATTRIBUTE_H
#define AS_ATTRIBUTE_H

#include <godot_cpp/classes/resource.hpp>

#include "macros.hpp"
#include "utils.hpp"

using namespace godot;

class Attribute: public Resource {
    GDCLASS(Attribute, Resource);

    friend class AttributeMap;

private:
    StringName identifier = "";
    float min_value = 0.0;
    float max_value = 1.0;
    float default_value = 0.0;

    /* UI properties */
    Color ui_color = Color(1.0, 1.0, 1.0);

protected:
    static void _bind_methods() {
        BIND_GETSET(Attribute, identifier);
        BIND_GETSET(Attribute, max_value);
        BIND_GETSET(Attribute, min_value);
        BIND_GETSET(Attribute, default_value);
        BIND_GETSET(Attribute, ui_color);

        GROUP("Attribute");

        PROP(Variant::STRING_NAME, identifier);
        PROP(Variant::FLOAT, max_value);
        PROP(Variant::FLOAT, min_value);
        PROP(Variant::FLOAT, default_value);

        ADD_GROUP("UI", "ui_");
        PROP(Variant::COLOR, ui_color);
    }

public:
    Attribute() {}

    ~Attribute() {}

    GETSET_RESOURCE(StringName, identifier)
    GETSET_RESOURCE(float, min_value)
    GETSET_RESOURCE(float, max_value)
    GETSET_RESOURCE(float, default_value)
    GETSET_RESOURCE(Color, ui_color)

    String _to_string() const {
        return fmt("{0}(\"{1}\")", get_class(), identifier);
    }
};

#endif