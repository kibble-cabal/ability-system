#ifndef AS_ABILITYEVENT_HPP
#define AS_ABILITYEVENT_HPP

#include "ability.hpp"
#include "core/io/resource.h"
#include "macros.hpp"
#include "status.hpp"

class AbilitySystem;

class AbilityEvent : public Resource {
	GDCLASS(AbilityEvent, Resource);

private:
	Ref<Ability> ability;

protected:
	static void _bind_methods();

public:
	GETSET_RESOURCE(Ref<Ability>, ability)

	Status _tick(AbilitySystem owner, float delta);
};

#endif