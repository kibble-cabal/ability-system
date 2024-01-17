#include "ability_event.h"

#include "ability_system.h"

void AbilityEvent::_bind_methods() {
	BIND_GETSET(AbilityEvent, ability);
	OBJECT_PROP(Ability, ability);
}

Status AbilityEvent::_tick(AbilitySystem owner, float delta) {
	return Status::RUNNING;
}