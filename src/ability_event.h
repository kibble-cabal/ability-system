#ifndef AS_ABILITYEVENT_HPP
#define AS_ABILITYEVENT_HPP

#include <godot_cpp/classes/resource.hpp>

#include "ability.hpp"
#include "status.hpp"

using namespace godot;

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

	void start_effect(AbilitySystem *owner, Ref<Effect> effect_instance);
	void finish_effect(AbilitySystem *owner, Ref<Effect> effect_instance);

	void tick_parallel(AbilitySystem *owner, float delta);
	void tick_sequential(AbilitySystem *owner, float delta);

	void do_loop(AbilitySystem *owner);

public:
	GETSET_RESOURCE(Ref<Ability>, ability)
	GETSET_RESOURCE(TypedArray<Effect>, effect_instances)

	void start(AbilitySystem *owner);
	Status tick(AbilitySystem *owner, float delta);

	String _to_string() const;
};

#endif