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
	ADD_SIGNAL(MethodInfo(as_signal::Started, OBJECT_PROP_INFO(AbilitySystem, owner)));
	ADD_SIGNAL(MethodInfo(as_signal::Finished, OBJECT_PROP_INFO(AbilitySystem, owner)));
	ADD_SIGNAL(MethodInfo(as_signal::EffectStarted, OBJECT_PROP_INFO(AbilitySystem, owner), OBJECT_PROP_INFO(Effect, effect_instance)));
	ADD_SIGNAL(MethodInfo(as_signal::EffectFinished, OBJECT_PROP_INFO(AbilitySystem, owner), OBJECT_PROP_INFO(Effect, effect_instance)));
}

void AbilityEvent::start_effect(AbilitySystem *owner, Ref<Effect> effect_instance) {
	effect_instance->start(owner);
	emit_signal(as_signal::EffectStarted, owner, effect_instance);
	owner->emit_signal(as_signal::EffectsChanged);
}

void AbilityEvent::finish_effect(AbilitySystem *owner, Ref<Effect> effect_instance) {
	effect_instance->finish(owner);
	emit_signal(as_signal::EffectFinished, owner, effect_instance);
	owner->emit_signal(as_signal::EffectsChanged);
}

void AbilityEvent::start(AbilitySystem *owner) {
	status = Status::READY;
	effect_instances.clear();
	for_each_i(this->ability->get_effects(), [this, owner](Ref<Effect> effect, int i) {
		Ref<Effect> instance = effect->instantiate(i);
		effect_instances.append(instance);
		// Start the first effect (when in sequential mode) or all effects (when in parallel mode)
		if (i == 0 || ability->effect_mode == EffectMode::PARALLEL)
			start_effect(owner, instance);
	});
	emit_signal(as_signal::Started, owner);
	status = Status::RUNNING;
}

Status AbilityEvent::tick(AbilitySystem *owner, float delta) {
	if (status != Status::RUNNING)
		return status;
	
	if (effect_instances.size() > 0 && ((Ref<Effect>)effect_instances[0])->is_loop_effect()) {
		Ref<Effect> current_effect = effect_instances[0];
		// If there's a running loop effect, perform the loop.
		switch (current_effect->tick(owner, delta)) {
			case Status::READY:
				break;
			case Status::RUNNING:
				do_loop(owner);
				break;
			case Status::FINISHED: {
				effect_instances.pop_front();
				finish_effect(owner, current_effect);
			} break;
		}
	} else {
		switch (ability->effect_mode) {
			case EffectMode::PARALLEL:
				tick_parallel(owner, delta);
				break;
			case EffectMode::SEQUENTIAL:
				tick_sequential(owner, delta);
				break;
		}
	}

	// If all effects have been removed, the event is finished processing.
	if (effect_instances.size() == 0) {
		status = Status::FINISHED;
		emit_signal(as_signal::Finished, owner);
	// Otherwise, it is still running.
	} else
		status = Status::RUNNING;

	return status;
}

void AbilityEvent::tick_parallel(AbilitySystem *owner, float delta) {
	std::vector<int> finished_effects;

	// Tick every effect.
	for_each_i(effect_instances, [&](Ref<Effect> effect, int i) {
		// Make a list of all finished effects.
		if (effect->is_loop_effect()) return;
		if (effect->tick(owner, delta) == Status::FINISHED) {
			finished_effects.push_back(i);
			finish_effect(owner, effect);
		}
	});

	// Remove all finished effects.
	for (int i = finished_effects.size() - 1; i >= 0; i--)
		effect_instances.remove_at(finished_effects[i]);
}

void AbilityEvent::tick_sequential(AbilitySystem *owner, float delta) {
	Status current_status = Status::READY;
	// Tick the first effect, if it exists.
	if (effect_instances.size()) {
		Ref<Effect> effect = effect_instances[0];
		current_status = effect->tick(owner, delta);
		// If the first effect is finished, remove it from the list.
		if (current_status == Status::FINISHED) {
			effect_instances.pop_front();
			finish_effect(owner, effect);
		}
	}

	if (effect_instances.size() && current_status != Status::RUNNING) {
		Ref<Effect> current_effect = effect_instances[0];
		start_effect(owner, current_effect);
	}
}

void AbilityEvent::do_loop(AbilitySystem *owner) {
	Ref<Effect> loop_effect = effect_instances[0];
	
	for (int i = loop_effect->index - 1; i >= 0; i--) {
		Ref<Effect> effect = ability->get_effects()[i];
		Ref<Effect> instance = effect->instantiate(i);
		effect_instances.push_front(instance);
		if (i == 0 || ability->effect_mode == EffectMode::PARALLEL)
			start_effect(owner, instance);
	}

	emit_signal(as_signal::EffectsChanged);
}

Ref<Effect> AbilityEvent::get_effect_instance(Ref<Effect> effect, int index) const {
	for (int i = 0; i < effect_instances.size(); i++) {
		Ref<Effect> instance = effect_instances[i];
		if (instance->instantiated_from == effect && instance->index == index)
			return instance;
	}
	return nullptr;
}

String AbilityEvent::_to_string() const {
	return String("AbilityEvent({0})").format(variant_array(ability->get_identifier()));
}