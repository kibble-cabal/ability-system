#ifndef AS_ABILITYSYSTEM_H
#define AS_ABILITYSYSTEM_H

#include "ability.hpp"
#include "ability_event.h"
#include "attribute_map.hpp"
#include "scene/main/node.h"
#include "tag.hpp"

class AbilitySystemState : public Resource {
	GDCLASS(AbilitySystemState, Resource);

	friend class AbilitySystem;

private:
	Ref<AttributeMap> attribute_map;
	TypedArray<Tag> granted_tags;
	TypedArray<Ability> granted_abilities;
	TypedArray<AbilityEvent> ability_events;

protected:
	static void _bind_methods();

public:
	GETSET_RESOURCE(Ref<AttributeMap>, attribute_map)
	GETSET_RESOURCE(TypedArray<Tag>, granted_tags)
	GETSET_RESOURCE(TypedArray<Ability>, granted_abilities)
	GETSET_RESOURCE(TypedArray<AbilityEvent>, ability_events)
};

class AbilitySystem : public Node {
	GDCLASS(AbilitySystem, Node);

private:
	Ref<AbilitySystemState> state;

protected:
	static void _bind_methods();

public:
	GETSET(Ref<AbilitySystemState>, state)

	/* Methods */

	bool can_activate_ability(Ref<Ability> ability) const;
	Ref<AbilityEvent> activate_ability(Ref<Ability> ability);
	bool is_granted(Ref<Ability> ability_to_check) const;
	bool has_tag(Ref<Tag> tag_to_check) const;
	bool has_some_tags(TypedArray<Tag> tags_to_check) const;
	bool has_all_tags(TypedArray<Tag> tags_to_check) const;
	bool has_no_tags(TypedArray<Tag> tags_to_check) const;
};

#endif