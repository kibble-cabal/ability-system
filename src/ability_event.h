#ifndef AS_ABILITYEVENT_HPP
#define AS_ABILITYEVENT_HPP

#ifdef ABILITY_SYSTEM_MODULE
#include "core/io/resource.h"
#else
#include <godot_cpp/classes/resource.hpp>
using namespace godot;
#endif

#include "ability.hpp"
#include "status.hpp"

class AbilitySystem;

class AbilityEvent : public Resource
{
	GDCLASS(AbilityEvent, Resource);

private:
	Ref<Ability> ability;
	TypedArray<Effect> effect_instances;
	Status status;

protected:
	static void _bind_methods();

	void tick_parallel(AbilitySystem *owner, float delta);
	void tick_sequential(AbilitySystem *owner, float delta);

public:
	GETSET_RESOURCE(Ref<Ability>, ability)
	GETSET_RESOURCE(TypedArray<Effect>, effect_instances)

	void start(AbilitySystem *owner);
	Status tick(AbilitySystem *owner, float delta);

	#ifdef ABILITY_SYSTEM_MODULE
	String to_string() override;
	#else
	String _to_string() const;
	#endif
};

#endif