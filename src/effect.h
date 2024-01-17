#ifndef AS_EFFECT_H
#define AS_EFFECT_H

#include "core/io/resource.h"
#include "macros.hpp"
#include "status.hpp"

class AbilitySystem;

class Effect : public Resource {
	GDCLASS(Effect, Resource);

private:
	float elapsed_time = 0.0;

	/* UI properties */
	StringName ui_name;
	Color ui_color;

protected:
	static void _bind_methods();

public:
	/* Getters/setters */

	GETSET_RESOURCE(float, elapsed_time)
	GETSET_RESOURCE(StringName, ui_name)
	GETSET_RESOURCE(Color, ui_color)

	/* Virtual methods */

	virtual void _start(AbilitySystem *owner) {}
	virtual Status _tick(AbilitySystem *owner, float delta) { return Status::RUNNING; }
	virtual void _finish(AbilitySystem *owner) {}

	/* Methods */

	Status tick(AbilitySystem *owner, float delta);
	void start(AbilitySystem *owner);
	void finish(AbilitySystem *owner);
};

#endif