#ifndef AS_TAG_HPP
#define AS_TAG_HPP

#include "core/io/resource.h"
#include "macros.hpp"
#include "scene/resources/texture.h"

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

	bool operator==(Ref<Tag> other) {
		return identifier == other->identifier;
	}

	GETSET_RESOURCE(StringName, identifier)
	GETSET_RESOURCE(Color, ui_color);
};

#endif