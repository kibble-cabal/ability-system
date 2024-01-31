#ifndef AS_EFFECT_H
#define AS_EFFECT_H

#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/variant/variant.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/object.hpp>

#include "../macros.hpp"
#include "../status.hpp"

using namespace godot;

class AbilitySystem;

class Effect : public Resource
{
	GDCLASS(Effect, Resource);

	friend class AbilityEvent;

private:
	float elapsed_time = 0.0;

	/* UI properties */
	StringName ui_name;
	Color ui_color;

	int index;

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

	Ref<Effect> instantiate(int index) const {
		Ref<Effect> instance = duplicate(false);
		instance->index = index;
		return instance;
	}

	Status tick(AbilitySystem *owner, float delta);
	void start(AbilitySystem *owner);
	void finish(AbilitySystem *owner);

	virtual bool is_loop_effect() { return false; }
	
	String _to_string() const;
};

#endif