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
}; //namespace as_signal

class AbilitySystemState : public Resource {
	GDCLASS(AbilitySystemState, Resource);

	friend class AbilitySystem;

private:
	Ref<AttributeMap> attribute_map;
	TypedArray<Tag> tags;
	TypedArray<Ability> abilities;
	TypedArray<AbilityEvent> events;

protected:
	static void _bind_methods();

public:
	GETSET_RESOURCE(Ref<AttributeMap>, attribute_map)
	GETSET_RESOURCE(TypedArray<Tag>, tags)
	GETSET_RESOURCE(TypedArray<Ability>, abilities)
	GETSET_RESOURCE(TypedArray<AbilityEvent>, events)
};

class AbilitySystem : public Node {
	GDCLASS(AbilitySystem, Node);

private:
	Ref<AbilitySystemState> state;

protected:
	static void _bind_methods();

public:
	GETSET(Ref<AbilitySystemState>, state)

	void _physics_process(float delta);

	/* Attribute methods */

	bool has_attribute(Ref<Attribute> attribute) const;
	void grant_attribute(Ref<Attribute> attribute, float value);
	void revoke_attribute(Ref<Attribute> attribute);

	/* Ability methods */

	bool can_activate(Ref<Ability> ability) const;
	bool has_ability(Ref<Ability> ability_to_check) const;
	void grant_ability(Ref<Ability> ability);
	void revoke_ability(Ref<Ability> ability);
	Ref<AbilityEvent> activate(Ref<Ability> ability);

	/* Tag methods */

	bool has_tag(Ref<Tag> tag_to_check) const;
	bool has_some_tags(TypedArray<Tag> tags_to_check) const;
	bool has_all_tags(TypedArray<Tag> tags_to_check) const;
	bool has_no_tags(TypedArray<Tag> tags_to_check) const;
	void grant_tag(Ref<Tag> tag);
	void revoke_tag(Ref<Tag> tag);
};

#endif