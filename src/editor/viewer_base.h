#ifndef AS_VIEWERBASE_HPP
#define AS_VIEWERBASE_HPP

#ifdef ABILITY_SYSTEM_MODULE
#include "scene/gui/control.h"
#else
#include <godot_cpp/classes/control.hpp>
using namespace godot;
#endif

#include "../ability_system.h"

class AbilitySystemViewerBase : public Control {
	GDCLASS(AbilitySystemViewerBase, Control);

private:
	NodePath ability_system_path;

protected:
	static void _bind_methods();

	/// @brief Connects all existing tags to prompt a redraw when any property is changed.
	void connect_to_tags();

	/// @brief Connects all existing attributes to prompt a redraw when any property is changed.
	void connect_to_attributes();

	/// @brief Connects all existing abilities to prompt a redraw when any property is changed.
	void connect_to_abilities();

	/// @brief Connects all existing ability events to prompt a redraw when any property is changed.
	void connect_to_events();

public:
	GETSET(NodePath, ability_system_path)

	AbilitySystem *get_ability_system() const;

	void _notification(int what);

	virtual void _draw() override {}

	void _tags_changed();
	void _attributes_changed();
	void _abilities_changed();
	void _events_changed();
	void _effects_changed();
	void _event_blocked(Ref<Ability> ability);

	void draw();

	// FIXME: overriding _get_minimum_size() crashes when run as GDExtension
	#ifdef ABILITY_SYSTEM_MODULE
	Vector2 get_minimum_size() const override { return Vector2(); }
	#endif
};

#endif