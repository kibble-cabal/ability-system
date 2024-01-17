#ifndef AS_EFFECT_HPP
#define AS_EFFECT_HPP

#include "core/io/resource.h"
#include "macros.hpp"
#include "scene/resources/texture.h"
#include "status.hpp"

class Effect : public Resource {
	GDCLASS(Effect, Resource);

private:
	float elapsed_time = 0.0;

	/* UI properties */
	StringName ui_name;
	Color ui_color;

protected:
	static void _bind_methods() {
		/* Bind virtual methods */
		BIND_VIRTUAL(_tick, { "delta" });

		/* Bind property getters/setters */
		BIND_GETSET(Effect, elapsed_time);
		BIND_GETSET(Effect, ui_name);
		BIND_GETSET(Effect, ui_color);

		/* Bind constants */
		ClassDB::bind_integer_constant(get_class_static(), "Status", "READY", Status::READY);
		ClassDB::bind_integer_constant(get_class_static(), "Status", "RUNNING", Status::RUNNING);
		ClassDB::bind_integer_constant(get_class_static(), "Status", "FAILED", Status::FAILURE);
		ClassDB::bind_integer_constant(get_class_static(), "Status", "FINISHED", Status::FINISHED);

		/* Bind properties */
		PROP(Variant::FLOAT, elapsed_time);

		ADD_GROUP("UI", "ui_");
		PROP(Variant::STRING_NAME, ui_name);
		PROP(Variant::COLOR, ui_color);
	}

public:
	GETSET_RESOURCE(float, elapsed_time)
	GETSET_RESOURCE(StringName, ui_name)
	GETSET_RESOURCE(Color, ui_color)

	virtual Status _tick(float delta) {
		return Status::RUNNING;
	}

	Ref<Effect> instantiate() {
		return duplicate(true);
	}
};

#endif