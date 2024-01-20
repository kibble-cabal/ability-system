#ifndef AS_EFFECT_H
#define AS_EFFECT_H

#ifdef ABILITY_SYSTEM_MODULE
#include "core/io/resource.h"
#else
#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/variant/variant.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/object.hpp>
using namespace godot;
#endif

#include "../macros.hpp"
#include "../status.hpp"

class AbilitySystem;

class Effect : public Resource
{
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
	virtual int _tick(AbilitySystem *owner, float delta) { return Status::RUNNING; }
	virtual void _finish(AbilitySystem *owner) {}

	/* Methods */

	Status tick(AbilitySystem *owner, float delta);
	void start(AbilitySystem *owner);
	void finish(AbilitySystem *owner);
	
	#ifdef ABILITY_SYSTEM_MODULE
	virtual String to_string() override;
	#else
	String _to_string() const;
	#endif
};

#endif