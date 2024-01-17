
#include "effect.h"

void Effect::_bind_methods() {
	/* Bind virtual methods */
	BIND_VIRTUAL(_start, "owner");
	BIND_VIRTUAL(_tick, "owner", "delta");
	BIND_VIRTUAL(_finish, "owner");

	/* Bind property getters/setters */
	BIND_GETSET(Effect, elapsed_time);
	BIND_GETSET(Effect, ui_name);
	BIND_GETSET(Effect, ui_color);

	/* Bind constants */
	ClassDB::bind_integer_constant(get_class_static(), "Status", "READY", Status::READY);
	ClassDB::bind_integer_constant(get_class_static(), "Status", "RUNNING", Status::RUNNING);
	ClassDB::bind_integer_constant(get_class_static(), "Status", "FINISHED", Status::FINISHED);

	/* Bind properties */
	PROP(Variant::FLOAT, elapsed_time);

	ADD_GROUP("UI", "ui_");
	PROP(Variant::STRING_NAME, ui_name);
	PROP(Variant::COLOR, ui_color);
}

Status Effect::tick(AbilitySystem *owner, float delta) {
	return _tick(owner, delta);
}

void Effect::start(AbilitySystem *owner) {
	_start(owner);
}

void Effect::finish(AbilitySystem *owner) {
	_finish(owner);
}