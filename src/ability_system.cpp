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
	ARRAY_PROP(tags, RESOURCE_TYPE_HINT("Tag"));
	ARRAY_PROP(abilities, RESOURCE_TYPE_HINT("Ability"));
	ARRAY_PROP(events, RESOURCE_TYPE_HINT("AbilityEvent"));
}

void AbilitySystem::_bind_methods() {
	/* Bind methods */
	BIND_GETSET(AbilitySystem, state);

	ClassDB::bind_method(D_METHOD("has_attribute", "attribute"), &AbilitySystem::has_attribute);
	ClassDB::bind_method(D_METHOD("grant_attribute", "attribute"), &AbilitySystem::grant_attribute);
	ClassDB::bind_method(D_METHOD("revoke_attribute", "attribute"), &AbilitySystem::revoke_attribute);
	ClassDB::bind_method(D_METHOD("get_attribute_value", "attribute"), &AbilitySystem::get_attribute_value);
	ClassDB::bind_method(D_METHOD("set_attribute_value", "attribute", "value"), &AbilitySystem::set_attribute_value);
	ClassDB::bind_method(D_METHOD("modify_attribute_value", "attribute", "by_amount"), &AbilitySystem::modify_attribute_value);
	ClassDB::bind_method(D_METHOD("can_activate", "ability"), &AbilitySystem::can_activate);
	ClassDB::bind_method(D_METHOD("has_ability", "ability"), &AbilitySystem::has_ability);
	ClassDB::bind_method(D_METHOD("grant_ability", "ability"), &AbilitySystem::grant_ability);
	ClassDB::bind_method(D_METHOD("revoke_ability", "ability"), &AbilitySystem::revoke_ability);
	ClassDB::bind_method(D_METHOD("activate", "ability"), &AbilitySystem::activate);
	ClassDB::bind_method(D_METHOD("has_tag", "tag"), &AbilitySystem::has_tag);
	ClassDB::bind_method(D_METHOD("has_some_tags", "tags"), &AbilitySystem::has_some_tags);
	ClassDB::bind_method(D_METHOD("has_all_tags", "tags"), &AbilitySystem::has_all_tags);
	ClassDB::bind_method(D_METHOD("grant_tag", "tag"), &AbilitySystem::grant_tag);
	ClassDB::bind_method(D_METHOD("revoke_tag", "tag"), &AbilitySystem::revoke_tag);

	/* Bind properties */
	OBJECT_PROP(AbilitySystemState, state);

	/* Bind signals */
	ADD_SIGNAL(MethodInfo(as_signal::EventBlocked, OBJECT_PROP_INFO(Ability, ability)));
	ADD_SIGNAL(MethodInfo(as_signal::EventStarted, OBJECT_PROP_INFO(AbilityEvent, event)));
	ADD_SIGNAL(MethodInfo(as_signal::EventFinished, OBJECT_PROP_INFO(AbilityEvent, event)));
}

void AbilitySystem::_notification(int notification) {
	switch (notification) {
		case NOTIFICATION_PHYSICS_PROCESS:
			Variant delta = get_process_delta_time();
			update(delta);
	}
}

void AbilitySystem::update(float delta) {
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

void AbilitySystem::grant_attribute(Ref<Attribute> attribute) {
	ERR_FAIL_NULL(state);
	if (!has_attribute(attribute)) {
		state->attribute_map->add(attribute);
	}
}

void AbilitySystem::revoke_attribute(Ref<Attribute> attribute) {
	ERR_FAIL_NULL(state);
	if (has_attribute(attribute)) {
		state->attribute_map->remove(attribute);
	}
}

float AbilitySystem::get_attribute_value(Ref<Attribute> attribute) const {
	ERR_FAIL_NULL_V(state, attribute->get_default_value());
	if (has_attribute(attribute)) {
		return state->attribute_map->get_value(attribute);
	}
	return attribute->get_default_value();
}

void AbilitySystem::set_attribute_value(Ref<Attribute> attribute, float value) {
	ERR_FAIL_NULL(state);
	if (has_attribute(attribute)) {
		state->attribute_map->set_value(attribute, value);
	}
}

void AbilitySystem::modify_attribute_value(Ref<Attribute> attribute, float by_amount) {
	set_attribute_value(attribute, get_attribute_value(attribute) + by_amount);
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
	return has_ability(ability) && has_all_tags(ability->get_tags_required()) && !has_some_tags(ability->get_tags_blocking());
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