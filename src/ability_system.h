#ifndef AS_ABILITYSYSTEM_H
#define AS_ABILITYSYSTEM_H

#include "ability.hpp"
#include "ability_event.h"
#include "attribute_map.hpp"
#include "scene/main/node.h"
#include "tag.hpp"

namespace as_signal {
static const auto EventBlocked = "ability_event_blocked";
static const auto EventFinished = "ability_event_finished";
static const auto EventStarted = "ability_event_started";
static const auto EventsChanged = "events_changed";
static const auto TagGranted = "tag_granted";
static const auto TagRevoked = "tag_revoked";
static const auto TagsChanged = "tags_changed";
static const auto AbilityGranted = "ability_granted";
static const auto AbilityRevoked = "ability_revoked";
static const auto AbilitiesChanged = "abilities_changed";
static const auto AttributeGranted = "attribute_granted";
static const auto AttributeRevoked = "attribute_revoked";
static const auto AttributeValueChanged = "attribute_value_changed";
static const auto AttributesChanged = "attributes_changed";
}; //namespace as_signal

enum UpdateMode {
	PHYSICS = 1,
	PROCESS = 2,
	DISABLED = 0
};

static const auto UpdateModePropertyHint = "Physics:1,Process:2,Disabled:0";

class AbilitySystem : public Node {
	GDCLASS(AbilitySystem, Node);

private:
	AttributeMap attribute_map = AttributeMap();
	TypedArray<Tag> tags;
	TypedArray<Ability> abilities;
	TypedArray<AbilityEvent> events;
	UpdateMode update_mode = UpdateMode::PHYSICS;

protected:
	static void _bind_methods();

public:
	GETTER(TypedArray<Tag>, tags)
	GETTER(TypedArray<Ability>, abilities)
	GETTER(TypedArray<AbilityEvent>, events)

	AttributeMap *get_attribute_map() {
		return &attribute_map;
	}

	int get_update_mode() const {
		return (int)update_mode;
	}

	void set_update_mode(int value) {
		update_mode = (UpdateMode)value;
		update_process_mode();
	}

	void update_process_mode() {
		if (!is_ready() || Engine::get_singleton()->is_editor_hint())
			return;
		set_process_internal(false);
		set_physics_process_internal(false);
		if (events.size()) {
			switch (update_mode) {
				case UpdateMode::PHYSICS:
					set_physics_process_internal(true);
					break;
				case UpdateMode::PROCESS:
					set_process_input(true);
					break;
				default:
					return;
			}
		}
	}

	void _notification(int notification);

	void update(float delta);

	/* Attribute methods */

	Dictionary get_attribute_dict() const {
		return attribute_map.get_attribute_dict();
	}
	void set_attribute_dict(Dictionary value) {
		attribute_map.set_attribute_dict(value);
		emit_signal(as_signal::AttributesChanged);
	}
	bool has_attribute(Ref<Attribute> attribute) const;
	void grant_attribute(Ref<Attribute> attribute);
	void revoke_attribute(Ref<Attribute> attribute);
	float get_attribute_value(Ref<Attribute> attribute) const;
	void set_attribute_value(Ref<Attribute> attribute, float value);
	void modify_attribute_value(Ref<Attribute> attribute, float by_amount);

	/* Ability methods */

	void set_abilities(TypedArray<Ability> abilities) {
		this->abilities = abilities;
		emit_signal(as_signal::AbilitiesChanged);
	}
	bool can_activate(Ref<Ability> ability) const;
	bool has_ability(Ref<Ability> ability_to_check) const;
	void grant_ability(Ref<Ability> ability);
	void revoke_ability(Ref<Ability> ability);
	Ref<AbilityEvent> activate(Ref<Ability> ability);
	void set_events(TypedArray<AbilityEvent> events) {
		this->events = events;
		emit_signal(as_signal::EventsChanged);
	}

	/* Tag methods */

	void set_tags(TypedArray<Tag> tags) {
		this->tags = tags;
		emit_signal(as_signal::TagsChanged);
	}
	bool has_tag(Ref<Tag> tag_to_check) const;
	bool has_some_tags(TypedArray<Tag> tags_to_check) const;
	bool has_all_tags(TypedArray<Tag> tags_to_check) const;
	void grant_tag(Ref<Tag> tag);
	void revoke_tag(Ref<Tag> tag);

	/* Other */
	virtual String to_string() override;
};

#endif