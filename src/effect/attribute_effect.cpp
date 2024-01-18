#include "attribute_effect.h"

#include "../ability_system.h"

void AttributeEffect::_bind_methods() {
	BIND_GETSET(AttributeEffect, attribute);
	BIND_GETSET(AttributeEffect, min_effect);
	BIND_GETSET(AttributeEffect, max_effect);

	OBJECT_PROP(Attribute, attribute);
	PROP(Variant::FLOAT, min_effect);
	PROP(Variant::FLOAT, max_effect);
}

Status AttributeEffect::_tick(AbilitySystem *owner, float delta) {
	if (attribute.is_null()) {
		print_error("No attribute provided!");
		return Status::FINISHED;
	}
	if (owner->has_attribute(attribute)) {
		float effect = Math::random(min_effect, max_effect);
		owner->modify_attribute_value(attribute, effect);
	} else {
		print_error("Searching for attribute: " + stringify_variants(attribute));
		print_error("Owner missing attribute! (attributes are " + stringify_variants(owner->get_state()->get_attribute_map()->get_attribute_dict()) + ")");
	}
	return Status::FINISHED;
}