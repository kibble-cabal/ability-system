#include "ability_system.h"

#include "macros.hpp"
#include "utils.hpp"

void AbilitySystem::_bind_methods() {
	/* Bind methods */
	BIND_GETSET(AbilitySystem, tags);
	BIND_GETSET(AbilitySystem, abilities);
	BIND_GETSET(AbilitySystem, events);
	BIND_GETSET(AbilitySystem, attribute_dict);
	BIND_GETSET(AbilitySystem, update_mode);

	/* Bind constants */
	ClassDB::bind_integer_constant(get_class_static(), "UpdateMode", "DISABLED", UpdateMode::DISABLED);
	ClassDB::bind_integer_constant(get_class_static(), "UpdateMode", "PHYSICS", UpdateMode::PHYSICS);
	ClassDB::bind_integer_constant(get_class_static(), "UpdateMode", "PROCESS", UpdateMode::PROCESS);

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
	ARRAY_PROP(tags, RESOURCE_TYPE_HINT("Tag"));
	ARRAY_PROP(abilities, RESOURCE_TYPE_HINT("Ability"));
	ARRAY_PROP(events, RESOURCE_TYPE_HINT("AbilityEvent"));
	ADD_PROPERTY(PropertyInfo(Variant::DICTIONARY, "attributes", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_NO_EDITOR), "set_attribute_dict", "get_attribute_dict");
	ClassDB::add_property(get_class_static(), PropertyInfo(Variant::INT, "update_mode", PROPERTY_HINT_ENUM, UpdateModePropertyHint), "set_update_mode", "get_update_mode");

	/* Bind signals */
	ADD_SIGNAL(MethodInfo(as_signal::EventBlocked, OBJECT_PROP_INFO(Ability, ability)));
	ADD_SIGNAL(MethodInfo(as_signal::EventStarted, OBJECT_PROP_INFO(AbilityEvent, event)));
	ADD_SIGNAL(MethodInfo(as_signal::EventFinished, OBJECT_PROP_INFO(AbilityEvent, event)));
	ADD_SIGNAL(MethodInfo(as_signal::AttributeGranted, OBJECT_PROP_INFO(Attribute, attribute)));
	ADD_SIGNAL(MethodInfo(as_signal::AttributeRevoked, OBJECT_PROP_INFO(Attribute, attribute)));
	ADD_SIGNAL(MethodInfo(as_signal::AttributeValueChanged, OBJECT_PROP_INFO(Attribute, attribute), PropertyInfo(Variant::FLOAT, "value")));
	ADD_SIGNAL(MethodInfo(as_signal::AbilityGranted, OBJECT_PROP_INFO(Ability, ability)));
	ADD_SIGNAL(MethodInfo(as_signal::AbilityRevoked, OBJECT_PROP_INFO(Ability, ability)));
	ADD_SIGNAL(MethodInfo(as_signal::TagGranted, OBJECT_PROP_INFO(Tag, tag)));
	ADD_SIGNAL(MethodInfo(as_signal::TagRevoked, OBJECT_PROP_INFO(Tag, tag)));
}

void AbilitySystem::_notification(int notification) {
	if (Engine::get_singleton()->is_editor_hint())
		return;
	switch (notification) {
		case NOTIFICATION_READY:
			// Start processing stored events.
			update_process_mode();
			break;
		case NOTIFICATION_INTERNAL_PHYSICS_PROCESS:
			if (update_mode == UpdateMode::PHYSICS) {
				Variant delta = get_process_delta_time();
				update(delta);
			}
			break;
		case NOTIFICATION_INTERNAL_PROCESS:
			if (update_mode == UpdateMode::PROCESS) {
				Variant delta = get_process_delta_time();
				update(delta);
			}
			break;
	}
}

void AbilitySystem::update(float delta) {
	std::vector<int> finished_events;

	// Tick every event.
	for_each_i(events, [this, delta, &finished_events](Ref<AbilityEvent> event, int i) {
		// Make a list of finished events.
		if (event->tick(this, delta) == Status::FINISHED) {
			finished_events.push_back(i);
			emit_signal(as_signal::EventFinished, event);
		}
	});

	// Remove all finished events.
	for (int i : finished_events)
		events.remove_at(i);

	// Stop processing.
	if (events.is_empty()) {
		set_physics_process_internal(false);
	}
}

/*************************
 *** Attribute methods ***
 *************************/

bool AbilitySystem::has_attribute(Ref<Attribute> attribute) const {
	return attribute_map.has(attribute);
}

void AbilitySystem::grant_attribute(Ref<Attribute> attribute) {
	if (!has_attribute(attribute)) {
		attribute_map.add(attribute);
		emit_signal(as_signal::AttributeGranted, attribute);
	}
}

void AbilitySystem::revoke_attribute(Ref<Attribute> attribute) {
	if (has_attribute(attribute)) {
		attribute_map.remove(attribute);
		emit_signal(as_signal::AttributeRevoked, attribute);
	}
}

float AbilitySystem::get_attribute_value(Ref<Attribute> attribute) const {
	if (has_attribute(attribute)) {
		return attribute_map.get_value(attribute);
	}
	return attribute->get_default_value();
}

void AbilitySystem::set_attribute_value(Ref<Attribute> attribute, float value) {
	if (has_attribute(attribute)) {
		attribute_map.try_set_value(attribute, value);
		emit_signal(as_signal::AttributeValueChanged, attribute, value);
	}
}

void AbilitySystem::modify_attribute_value(Ref<Attribute> attribute, float by_amount) {
	set_attribute_value(attribute, get_attribute_value(attribute) + by_amount);
}

/***********************
 *** Ability methods ***
 ***********************/

bool AbilitySystem::has_ability(Ref<Ability> ability_to_check) const {
	return any(abilities, [ability_to_check](Ref<Ability> ability) {
		return ability == ability_to_check;
	});
}

void AbilitySystem::grant_ability(Ref<Ability> ability) {
	if (!has_ability(ability)) {
		abilities.append(ability);
		emit_signal(as_signal::AbilityGranted, ability);
	}
}

void AbilitySystem::revoke_ability(Ref<Ability> ability) {
	if (has_ability(ability)) {
		abilities.erase(ability);
		emit_signal(as_signal::AbilityRevoked, ability);
	}
}

bool AbilitySystem::can_activate(Ref<Ability> ability) const {
	return has_ability(ability) && has_all_tags(ability->get_tags_required()) && !has_some_tags(ability->get_tags_blocking());
}

Ref<AbilityEvent> AbilitySystem::activate(Ref<Ability> ability) {
	if (can_activate(ability)) {
		auto event = memnew(AbilityEvent);
		events.append(event);
		event->set_ability(ability);
		event->start(this);
		emit_signal(as_signal::EventStarted, event);
		update_process_mode();
		return event;
	}
	emit_signal(as_signal::EventBlocked, ability);
	return nullptr;
}

/*******************
 *** Tag methods ***
 *******************/

bool AbilitySystem::has_tag(Ref<Tag> tag_to_check) const {
	return any(tags, [tag_to_check](Ref<Tag> tag) {
		return tag == tag_to_check;
	});
}

bool AbilitySystem::has_some_tags(TypedArray<Tag> tags_to_check) const {
	return any(tags_to_check, [this](Ref<Tag> tag) {
		return has_tag(tag);
	});
}

bool AbilitySystem::has_all_tags(TypedArray<Tag> tags_to_check) const {
	return all(tags_to_check, [this](Ref<Tag> tag) {
		return has_tag(tag);
	});
}

void AbilitySystem::grant_tag(Ref<Tag> tag) {
	if (!has_tag(tag)) {
		tags.append(tag);
		emit_signal(as_signal::TagGranted, tag);
	}
}

void AbilitySystem::revoke_tag(Ref<Tag> tag) {
	if (has_tag(tag)) {
		tags.erase(tag);
		emit_signal(as_signal::TagRevoked, tag);
	}
}

/*********************
 *** Other methods ***
 *********************/

String AbilitySystem::to_string() {
	Dictionary dict;
	dict["tags"] = tags;
	dict["abilities"] = abilities;
	dict["attributes"] = get_attribute_dict();
	dict["events"] = events;
	Array arr;
	arr.append(dict);
	return String("AbilitySystem{0}").format(arr);
}