#ifndef AS_ATTRIBUTE_H
#define AS_ATTRIBUTE_H

#include "core/io/resource.h"
#include "macros.hpp"
#include "utils.hpp"

class Attribute : public Resource {
	GDCLASS(Attribute, Resource);

	friend struct AttributeMap;

private:
	StringName attribute_name;
	float min_value = 0.0;
	float max_value = 1.0;
	float default_value = 0.0;

	/* UI properties */
	Color ui_color;

protected:
	static void _bind_methods() {
		BIND_GETSET(Attribute, attribute_name);
		BIND_GETSET(Attribute, max_value);
		BIND_GETSET(Attribute, min_value);
		BIND_GETSET(Attribute, default_value);
		BIND_GETSET(Attribute, ui_color);

		GROUP("Attribute");

		PROP(Variant::STRING_NAME, attribute_name);
		PROP(Variant::FLOAT, max_value);
		PROP(Variant::FLOAT, min_value);
		PROP(Variant::FLOAT, default_value);

		ADD_GROUP("UI", "ui_");
		PROP(Variant::COLOR, ui_color);
	}

public:
	Attribute() {}
	~Attribute() {}

	GETSET_RESOURCE(StringName, attribute_name)
	GETSET_RESOURCE(float, min_value)
	GETSET_RESOURCE(float, max_value)
	GETSET_RESOURCE(float, default_value)
	GETSET_RESOURCE(Color, ui_color)

	virtual String to_string() override {
		return String("Attribute({0})").format(variant_array(attribute_name));
	}
};

#endif