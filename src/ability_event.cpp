#include "ability_event.h"

#include "ability_system.h"
#include "macros.hpp"
#include "utils.hpp"

void AbilityEvent::_bind_methods() {
	BIND_GETSET(AbilityEvent, ability);
	BIND_GETSET(AbilityEvent, effect_instances);
	OBJECT_PROP(Ability, ability);
	ARRAY_PROP(effect_instances, RESOURCE_TYPE_HINT("Effect"));
}

void AbilityEvent::start(AbilitySystem *owner) {
	effect_instances.clear();
	for_each(this->ability->get_effects(), [this, owner](Ref<Effect> effect) {
		Ref<Effect> instance = effect->duplicate(true);
		effect_instances.append(instance);
		instance->start(owner);
	});
}

Status AbilityEvent::tick(AbilitySystem *owner, float delta) {
	std::vector<int> finished_effects;

	// Tick every effect.
	for_each_i(effect_instances, [&owner, delta, &finished_effects](Ref<Effect> effect, int i) {
		// Make a list of all finished effects (succeeded or failed).
		if (effect->tick(owner, delta) == Status::FINISHED)
			finished_effects.push_back(i);
	});

	// Remove all finished effects.
	for (int i : finished_effects)
		effect_instances.remove_at(i);

	// If all effects have been removed, the event is finished processing.
	if (effect_instances.size() == 0) {
		return Status::FINISHED;
	}

	// Otherwise, it is still running.
	return Status::RUNNING;
}
