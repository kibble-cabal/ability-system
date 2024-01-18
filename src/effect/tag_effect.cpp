#include "tag_effect.h"

#include "../ability_system.h"

void TagEffect::_bind_methods() {
	ClassDB::bind_integer_constant(get_class_static(), "Operation", "ADD", Operation::ADD);
	ClassDB::bind_integer_constant(get_class_static(), "Operation", "REMOVE", Operation::REMOVE);

	BIND_GETSET(TagEffect, tags);
	BIND_GETSET(TagEffect, operation);

	ARRAY_PROP(tags, RESOURCE_TYPE_HINT("Tag"));
	ClassDB::add_property(get_class_static(), PropertyInfo(Variant::INT, "operation", PROPERTY_HINT_ENUM, "Add:1,Remove:-1"), "set_operation", "get_operation");
}

void TagEffect::_start(AbilitySystem *owner) {
	for_each(tags, [this, owner](Ref<Tag> tag) {
		switch (operation) {
			case Operation::ADD:
				owner->grant_tag(tag);
			case Operation::REMOVE:
				owner->revoke_tag(tag);
		}
	});
}