#include "ability_event.h"

#include "ability_system.h"
#include "macros.hpp"
#include "utils.hpp"

void AbilityEvent::_bind_methods() {
	/* Bind methods */
	BIND_GETSET(AbilityEvent, ability);
	BIND_GETSET(AbilityEvent, effect_instances);

	/* Bind properties */
	OBJECT_PROP(Ability, ability);
	ARRAY_PROP(effect_instances, RESOURCE_TYPE_HINT("Effect"));

	/* Bind signals */
	ADD_SIGNAL(MethodInfo(as_signal::EffectFinished, OBJECT_PROP_INFO(Effect, effect)));
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
	if (status != Status::RUNNING)
		return status;

	switch (ability->effect_mode) {
		case EffectMode::PARALLEL:
			tick_parallel(owner, delta);
			break;
		case EffectMode::SEQUENTIAL:
			tick_sequential(owner, delta);
			break;
	}

	// If all effects have been removed, the event is finished processing.
	if (effect_instances.size() == 0)
		status = Status::FINISHED;

	// Otherwise, it is still running.
	else
		status = Status::RUNNING;

	return status;
}

void AbilityEvent::tick_parallel(AbilitySystem *owner, float delta) {
	std::vector<int> finished_effects;

	// Tick every effect.
	for_each_i(effect_instances, [&](Ref<Effect> effect, int i) {
		// Make a list of all finished effects.
		if (effect->tick(owner, delta) == Status::FINISHED) {
			finished_effects.push_back(i);
			emit_signal(as_signal::EffectFinished, effect);
		}
	});

	// Remove all finished effects.
	for (int i = finished_effects.size() - 1; i >= 0; i--)
		effect_instances.remove_at(finished_effects[i]);
}

void AbilityEvent::tick_sequential(AbilitySystem *owner, float delta) {
	// Tick the first effect, if it exists.
	if (effect_instances.size()) {
		Ref<Effect> effect = effect_instances[0];
		Status effect_status = effect->tick(owner, delta);
		// If the first effect is finished, remove it from the list.
		if (effect_status == Status::FINISHED) {
			effect_instances.pop_front();
			emit_signal(as_signal::EffectFinished, effect);
			owner->emit_signal(as_signal::EffectsChanged);
		}
	}
}

String AbilityEvent::_to_string() const {
	return String("AbilityEvent({0})").format(variant_array(ability->get_identifier()));
}