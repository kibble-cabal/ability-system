#include "ability_system.h"

#include "macros.hpp"

template <typename Ty, typename Fn>
bool any(TypedArray<Ty> array, Fn fn) {
	for (int i = 0; i < array.size(); i++) {
		if (fn(array[i]))
			return true;
	}
	return false;
}

template <typename Ty, typename Fn>
bool all(TypedArray<Ty> array, Fn fn) {
	for (int i = 0; i < array.size(); i++) {
		if (!fn(array[i]))
			return false;
	}
	return true;
}
void AbilitySystemState::_bind_methods() {
	/* Bind getter/setter methods */
	BIND_GETSET(AbilitySystemState, attribute_map);
	BIND_GETSET(AbilitySystemState, granted_tags);
	BIND_GETSET(AbilitySystemState, granted_abilities);
	BIND_GETSET(AbilitySystemState, ability_events);

	/* Bind properties */
	OBJECT_PROP(AttributeMap, attribute_map);
	PROP(Variant::ARRAY, granted_tags);
	PROP(Variant::ARRAY, granted_abilities);
	PROP(Variant::ARRAY, ability_events);
}

void AbilitySystem::_bind_methods() {
	/* Bind methods */
	BIND_GETSET(AbilitySystem, state);

	/* Bind properties */
	OBJECT_PROP(AbilitySystemState, state);
}
bool AbilitySystem::can_activate_ability(Ref<Ability> ability) const {
	return is_granted(ability) && has_all_tags(ability->get_tags_required()) && has_no_tags(ability->get_tags_blocking());
}

Ref<AbilityEvent> AbilitySystem::activate_ability(Ref<Ability> ability) {
	// TODO!
	auto event = memnew(AbilityEvent);
	event->set_ability(ability);
	return event;
}

bool AbilitySystem::is_granted(Ref<Ability> ability_to_check) const {
	ERR_FAIL_NULL_V(state, false);
	return any(state->granted_abilities, [ability_to_check](Ref<Ability> ability) {
		return ability == ability_to_check;
	});
}

bool AbilitySystem::has_tag(Ref<Tag> tag_to_check) const {
	ERR_FAIL_NULL_V(state, false);
	return any(state->granted_tags, [tag_to_check](Ref<Tag> tag) {
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