#ifndef AS_ABILITY_HPP
#define AS_ABILITY_HPP

#include "core/io/resource.h"
#include "core/variant/typed_array.h"
#include "effect/effect.h"
#include "macros.hpp"
#include "status.hpp"
#include "tag.hpp"

class AbilitySystem;

class Ability : public Resource {
	GDCLASS(Ability, Resource);

private:
	StringName ability_name;

	/* Tags */
	TypedArray<Tag> tags_blocking;
	TypedArray<Tag> tags_required;

	/* Effects */
	TypedArray<Effect> effects;

	/* UI properties */
	Color ui_color;

protected:
	static void _bind_methods() {
		/* Bind methods */
		BIND_GETSET(Ability, ability_name);
		BIND_GETSET(Ability, tags_blocking);
		BIND_GETSET(Ability, tags_required);
		BIND_GETSET(Ability, effects);
		BIND_GETSET(Ability, ui_color);

		/* Bind properties */
		PROP(Variant::STRING_NAME, ability_name);

		ADD_GROUP("Tags", "tags_");
		ARRAY_PROP(tags_blocking, RESOURCE_TYPE_HINT("Tag"));
		ARRAY_PROP(tags_required, RESOURCE_TYPE_HINT("Tag"));

		GROUP("Effects");
		ARRAY_PROP(effects, RESOURCE_TYPE_HINT("Effect"));

		ADD_GROUP("UI", "ui_");
		PROP(Variant::COLOR, ui_color);
	}

public:
	GETSET_RESOURCE(StringName, ability_name)
	GETSET_RESOURCE(TypedArray<Tag>, tags_blocking)
	GETSET_RESOURCE(TypedArray<Tag>, tags_required)
	GETSET_RESOURCE(TypedArray<Effect>, effects)

	GETSET_RESOURCE(Color, ui_color)
};

#endif