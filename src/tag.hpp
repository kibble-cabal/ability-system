#ifndef AS_TAG_HPP
#define AS_TAG_HPP

#include <godot_cpp/classes/resource.hpp>
using namespace godot;

#include "macros.hpp"
#include "utils.hpp"

class Tag: public Resource {
    GDCLASS(Tag, Resource);

private:
    StringName identifier;

    /* UI properties */
    Color ui_color = Color(1.0, 1.0, 1.0);

protected:
    static void _bind_methods() {
        BIND_GETSET(Tag, identifier);
        BIND_GETSET(Tag, ui_color);

        GROUP("Tag");
        PROP(Variant::STRING_NAME, identifier);

        ADD_GROUP("UI", "ui_");
        PROP(Variant::COLOR, ui_color)
    }

public:
    Tag() {}

    Tag(StringName identifier) {
        this->identifier = identifier;
    }

    GETSET_RESOURCE(StringName, identifier)
    GETSET_RESOURCE(Color, ui_color);

    String _to_string() const {
        return fmt("{0}(\"{1}\")", get_class(), identifier);
    }
};

#endif