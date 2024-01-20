#ifndef AS_TAG_HPP
#define AS_TAG_HPP

#ifdef ABILITY_SYSTEM_MODULE
#include "core/io/resource.h"
#else
#include <godot_cpp/classes/resource.hpp>
using namespace godot;
#endif

#include "macros.hpp"
#include "utils.hpp"

class Tag : public Resource {
	GDCLASS(Tag, Resource);

private:
	StringName identifier;

	/* UI properties */
	Color ui_color;

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

	#ifdef ABILITY_SYSTEM_MODULE
	virtual String to_string() override {
	#else
	String _to_string() const {
	#endif
		return String("Tag({0})").format(variant_array(identifier));
	}
};

#endif