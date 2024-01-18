
#include "effect.h"

void Effect::_bind_methods() {
	/* Bind virtual methods */
	ClassDB::add_virtual_method(get_class_static(), MethodInfo("_start", OBJECT_PROP_INFO(AbilitySystem, owner)), true, PackedStringArray({ "owner" }));
	ClassDB::add_virtual_method(get_class_static(), MethodInfo("_tick", OBJECT_PROP_INFO(AbilitySystem, owner), PropertyInfo(Variant::FLOAT, "delta")), true, PackedStringArray({ "owner", "delta" }));
	ClassDB::add_virtual_method(get_class_static(), MethodInfo("_finish", OBJECT_PROP_INFO(AbilitySystem, owner)), true, PackedStringArray({ "owner" }));

	/* Bind property getters/setters */
	BIND_GETSET(Effect, elapsed_time);
	BIND_GETSET(Effect, ui_name);
	BIND_GETSET(Effect, ui_color);

	/* Bind constants */
	ClassDB::bind_integer_constant(get_class_static(), "Status", "READY", Status::READY);
	ClassDB::bind_integer_constant(get_class_static(), "Status", "RUNNING", Status::RUNNING);
	ClassDB::bind_integer_constant(get_class_static(), "Status", "FINISHED", Status::FINISHED);

	/* Bind properties */
	NO_EDITOR_PROP(Variant::FLOAT, elapsed_time);

	ADD_GROUP("UI", "ui_");
	PROP(Variant::STRING_NAME, ui_name);
	PROP(Variant::COLOR, ui_color);
}

Status Effect::tick(AbilitySystem *owner, float delta) {
	elapsed_time += delta;
	return _tick(owner, delta);
}

void Effect::start(AbilitySystem *owner) {
	elapsed_time = 0;
	_start(owner);
}

void Effect::finish(AbilitySystem *owner) {
	_finish(owner);
}