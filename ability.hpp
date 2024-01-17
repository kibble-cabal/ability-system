#ifndef AS_ABILITY_HPP
#define AS_ABILITY_HPP

#include "core/io/resource.h"
#include "core/variant/typed_array.h"
#include "effect.hpp"
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
	TypedArray<Effect> effects_on_started;
	TypedArray<Effect> effects_on_blocked;
	TypedArray<Effect> effects_on_finished;

	/* UI properties */
	Color ui_color;

protected:
	static void _bind_methods() {
		/* Bind methods */
		BIND_GETSET(Ability, ability_name);
		BIND_GETSET(Ability, tags_blocking);
		BIND_GETSET(Ability, tags_required);
		BIND_GETSET(Ability, effects_on_started);
		BIND_GETSET(Ability, effects_on_blocked);
		BIND_GETSET(Ability, effects_on_finished);
		BIND_GETSET(Ability, ui_color);

		/* Bind properties */
		PROP(Variant::STRING_NAME, ability_name);

		ADD_GROUP("Tags", "tags_");
		PROP(Variant::ARRAY, tags_blocking);
		PROP(Variant::ARRAY, tags_required);

		ADD_GROUP("Effects", "effects_");
		PROP(Variant::ARRAY, effects_on_started);
		PROP(Variant::ARRAY, effects_on_blocked);
		PROP(Variant::ARRAY, effects_on_finished);

		ADD_GROUP("UI", "ui_");
		PROP(Variant::COLOR, ui_color);
	}

public:
	GETSET_RESOURCE(StringName, ability_name)
	GETSET_RESOURCE(TypedArray<Tag>, tags_blocking)
	GETSET_RESOURCE(TypedArray<Tag>, tags_required)

	GETSET_RESOURCE(TypedArray<Effect>, effects_on_started)
	GETSET_RESOURCE(TypedArray<Effect>, effects_on_blocked)
	GETSET_RESOURCE(TypedArray<Effect>, effects_on_finished)

	GETSET_RESOURCE(Color, ui_color)
};

#endif