#ifndef AS_VIEWERBASE_HPP
#define AS_VIEWERBASE_HPP

#include "../ability_system.h"
#include "render.hpp"
#include "scene/gui/control.h"

class AbilitySystemViewerBase : public Control {
	GDCLASS(AbilitySystemViewerBase, Control);

private:
	NodePath ability_system_path;

protected:
	static void _bind_methods() {
		BIND_GETSET(AbilitySystemViewerBase, ability_system_path);
		PROP(Variant::NODE_PATH, ability_system_path);

		ClassDB::bind_method(D_METHOD("_abilities_changed"), &AbilitySystemViewerBase::_abilities_changed);
		ClassDB::bind_method(D_METHOD("_attributes_changed"), &AbilitySystemViewerBase::_attributes_changed);
		ClassDB::bind_method(D_METHOD("_tags_changed"), &AbilitySystemViewerBase::_tags_changed);
		ClassDB::bind_method(D_METHOD("_events_changed"), &AbilitySystemViewerBase::_events_changed);
	}

	/// @brief Connects all existing tags to prompt a redraw when any property is changed.
	void connect_to_tags() {
		if (get_ability_system())
			for_each(get_ability_system()->get_tags(), [&](Ref<Tag> tag) {
				try_connect(tag.ptr(), "changed", Callable(this, "queue_redraw"));
			});
	}

	/// @brief Connects all existing attributes to prompt a redraw when any property is changed.
	void connect_to_attributes() {
		if (get_ability_system()) {
			TypedArray<Attribute> keys = get_ability_system()->get_attribute_dict().keys();
			for_each(keys, [&](Ref<Attribute> attribute) {
				try_connect(attribute.ptr(), "changed", Callable(this, "queue_redraw"));
			});
		}
	}

	/// @brief Connects all existing abilities to prompt a redraw when any property is changed.
	void connect_to_abilities() {
		if (get_ability_system())
			for_each(get_ability_system()->get_abilities(), [&](Ref<Ability> ability) {
				try_connect(ability.ptr(), "changed", Callable(this, "queue_redraw"));
			});
	}

	/// @brief Connects all existing ability events to prompt a redraw when any property is changed.
	void connect_to_events() {
		if (get_ability_system())
			for_each(get_ability_system()->get_events(), [&](Ref<AbilityEvent> event) {
				try_connect(event.ptr(), "changed", Callable(this, "queue_redraw"));
			});
	}

public:
	GETSET(NodePath, ability_system_path)

	AbilitySystem *get_ability_system() const {
		if (is_inside_tree())
			return Object::cast_to<AbilitySystem>(get_node(ability_system_path));
		return nullptr;
	}

	void _notification(int what) {
		switch (what) {
			case NOTIFICATION_ENTER_TREE:
			case NOTIFICATION_READY: {
				auto ability_system = get_ability_system();
				if (ability_system != nullptr) {
					// Connect signals
					try_connect(ability_system, as_signal::AbilitiesChanged, callable_mp(this, &AbilitySystemViewerBase::_abilities_changed));
					try_connect(ability_system, as_signal::AttributesChanged, callable_mp(this, &AbilitySystemViewerBase::_attributes_changed));
					try_connect(ability_system, as_signal::TagsChanged, callable_mp(this, &AbilitySystemViewerBase::_tags_changed));
					try_connect(ability_system, as_signal::EventsChanged, callable_mp(this, &AbilitySystemViewerBase::_events_changed));
					connect_to_tags();
					connect_to_attributes();
					connect_to_events();
					connect_to_abilities();
				}
			} break;
			case NOTIFICATION_DRAW:
				draw();
				break;
			case NOTIFICATION_VISIBILITY_CHANGED:
			case NOTIFICATION_RESIZED:
				queue_redraw();
				break;
		}
	}

	virtual void _draw(AbilitySystem *ability_system) {}

	void _tags_changed() {
		connect_to_tags();
		queue_redraw();
	}

	void _attributes_changed() {
		connect_to_attributes();
		queue_redraw();
	}

	void _abilities_changed() {
		connect_to_abilities();
		queue_redraw();
	}

	void _events_changed() {
		connect_to_events();
		queue_redraw();
	}

	void draw() {
		auto ability_system = get_ability_system();
		if (ability_system) {
			update_minimum_size();
			_draw(ability_system);
		}
	}
};

#endif