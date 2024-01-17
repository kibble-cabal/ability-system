#include "ability_system.h"

#include "macros.hpp"
#include "utils.hpp"

void AbilitySystemState::_bind_methods() {
	/* Bind getter/setter methods */
	BIND_GETSET(AbilitySystemState, attribute_map);
	BIND_GETSET(AbilitySystemState, tags);
	BIND_GETSET(AbilitySystemState, abilities);
	BIND_GETSET(AbilitySystemState, events);

	/* Bind properties */
	OBJECT_PROP(AttributeMap, attribute_map);
	PROP(Variant::ARRAY, tags);
	PROP(Variant::ARRAY, abilities);
	PROP(Variant::ARRAY, events);
}

void AbilitySystem::_bind_methods() {
	/* Bind methods */
	BIND_GETSET(AbilitySystem, state);

	/* Bind properties */
	OBJECT_PROP(AbilitySystemState, state);

	/* Bind signals */
	ADD_SIGNAL(MethodInfo(as_signal::EventBlocked, OBJECT_PROP_INFO(Ability, ability)));
	ADD_SIGNAL(MethodInfo(as_signal::EventStarted, OBJECT_PROP_INFO(AbilityEvent, event)));
	ADD_SIGNAL(MethodInfo(as_signal::EventFinished, OBJECT_PROP_INFO(AbilityEvent, event)));
}

void AbilitySystem::_physics_process(float delta) {
	if (state.is_valid()) {
		std::vector<int> finished_events;

		// Tick every event.
		for_each_i(state->events, [this, delta, &finished_events](Ref<AbilityEvent> event, int i) {
			// Make a list of finished events.
			if (event->tick(this, delta) == Status::FINISHED) {
				finished_events.push_back(i);
				emit_signal(as_signal::EventFinished, event);
			}
		});

		// Remove all finished events.
		for (int i : finished_events)
			state->events.remove_at(i);
	}
}

/*************************
 *** Attribute methods ***
 *************************/

bool AbilitySystem::has_attribute(Ref<Attribute> attribute) const {
	ERR_FAIL_NULL_V(state, false);
	return state->attribute_map->has(attribute);
}

void AbilitySystem::grant_attribute(Ref<Attribute> attribute, float value) {
	ERR_FAIL_NULL(state);
	if (!has_attribute(attribute)) {
		state->attribute_map->add(attribute);
		state->attribute_map->set_value(attribute, value);
	}
}

void AbilitySystem::revoke_attribute(Ref<Attribute> attribute) {
	ERR_FAIL_NULL(state);
	if (has_attribute(attribute)) {
		state->attribute_map->remove(attribute);
	}
}

/***********************
 *** Ability methods ***
 ***********************/

bool AbilitySystem::has_ability(Ref<Ability> ability_to_check) const {
	ERR_FAIL_NULL_V(state, false);
	return any(state->abilities, [ability_to_check](Ref<Ability> ability) {
		return ability == ability_to_check;
	});
}

void AbilitySystem::grant_ability(Ref<Ability> ability) {
	ERR_FAIL_NULL(state);
	if (!has_ability(ability)) {
		state->abilities.append(ability);
	}
}

void AbilitySystem::revoke_ability(Ref<Ability> ability) {
	ERR_FAIL_NULL(state);
	if (has_ability(ability)) {
		state->abilities.erase(ability);
	}
}

bool AbilitySystem::can_activate(Ref<Ability> ability) const {
	return has_ability(ability) && has_all_tags(ability->get_tags_required()) && has_no_tags(ability->get_tags_blocking());
}

Ref<AbilityEvent> AbilitySystem::activate(Ref<Ability> ability) {
	ERR_FAIL_NULL_V(state, nullptr);
	if (can_activate(ability)) {
		auto event = memnew(AbilityEvent);
		state->events.append(event);
		event->set_ability(ability);
		event->start(this);
		return event;
	}
	emit_signal(as_signal::EventBlocked, ability);
	return nullptr;
}

/*******************
 *** Tag methods ***
 *******************/

bool AbilitySystem::has_tag(Ref<Tag> tag_to_check) const {
	ERR_FAIL_NULL_V(state, false);
	return any(state->tags, [tag_to_check](Ref<Tag> tag) {
		return tag == tag_to_check;
	});
}

bool AbilitySystem::has_some_tags(TypedArray<Tag> tags_to_check) const {
	ERR_FAIL_NULL_V(state, false);
	return any(tags_to_check, [this](Ref<Tag> tag) {
		return has_tag(tag);
	});
}

bool AbilitySystem::has_all_tags(TypedArray<Tag> tags_to_check) const {
	ERR_FAIL_NULL_V(state, false);
	return all(tags_to_check, [this](Ref<Tag> tag) {
		return has_tag(tag);
	});
}

bool AbilitySystem::has_no_tags(TypedArray<Tag> tags_to_check) const {
	ERR_FAIL_NULL_V(state, false);
	return !has_some_tags(tags_to_check);
}

void AbilitySystem::grant_tag(Ref<Tag> tag) {
	ERR_FAIL_NULL(state);
	if (!has_tag(tag)) {
		state->tags.append(tag);
	}
}

void AbilitySystem::revoke_tag(Ref<Tag> tag) {
	ERR_FAIL_NULL(state);
	if (has_tag(tag)) {
		state->tags.erase(tag);
	}
}