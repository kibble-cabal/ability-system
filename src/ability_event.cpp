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
	status = Status::READY;
	effect_instances.clear();
	for_each(this->ability->get_effects(), [this, owner](Ref<Effect> effect) {
		Ref<Effect> instance = effect->duplicate(true);
		effect_instances.append(instance);
		instance->start(owner);
	});
	status = Status::RUNNING;
}

Status AbilityEvent::tick(AbilitySystem *owner, float delta) {
	if (status != Status::RUNNING) {
		return status;
	}

	std::vector<int> finished_effects;

	// Tick every effect.
	for_each_i(effect_instances, [&owner, delta, &finished_effects](Ref<Effect> effect, int i) {
		// Make a list of all finished effects (succeeded or failed).
		if (effect->tick(owner, delta) == Status::FINISHED)
			finished_effects.push_back(i);
	});

	// Remove all finished effects.
	for (auto it = finished_effects.rbegin(); it != finished_effects.rend(); ++it)
		effect_instances.remove_at(*it);

	// If all effects have been removed, the event is finished processing.
	if (effect_instances.size() == 0)
		status = Status::FINISHED;

	// Otherwise, it is still running.
	else
		status = Status::RUNNING;

	return status;
}

String AbilityEvent::to_string() {
	return String("AbilityEvent({0})").format(variant_array(ability->get_identifier()));
}