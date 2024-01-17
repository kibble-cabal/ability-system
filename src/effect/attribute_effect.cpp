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
	ERR_FAIL_NULL_V(attribute, Status::FINISHED);
	if (owner->has_attribute(attribute)) {
		float effect = Math::random(min_effect, max_effect);
		owner->modify_attribute_value(attribute, effect);
	}
	return Status::FINISHED;
}