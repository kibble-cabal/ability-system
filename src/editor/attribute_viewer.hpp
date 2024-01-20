#ifndef AS_ATTRIBUTEVIEWER_HPP
#define AS_ATTRIBUTEVIEWER_HPP

#include "viewer_base.hpp"

class AttributeViewer : public AbilitySystemViewerBase {
	GDCLASS(AttributeViewer, AbilitySystemViewerBase);

public:
	/* Override other event listeners to avoid unnecessary processing */
	void _tags_changed() {}
	void _abilities_changed() {}
	void _events_changed() {}
	void _effects_changed() {}
	void _event_blocked() {}

	void _draw(AbilitySystem *ability_system) override {
		RenderContainer container;
		container.canvas = this;
		container.max_size = get_size();
		container.style = LabelStyle::text;
		auto dict = ability_system->get_attribute_dict();
		auto keys = dict.keys();
		for (int i = 0; i < keys.size(); i++) {
			Ref<Attribute> attribute = keys[i];
			float percent = ability_system->get_attribute_map()->get_percent(attribute);
			container.add_progress(attribute->get_identifier(), percent, attribute->get_ui_color());
		}
		container.draw();
	}

	#ifdef ABILITY_SYSTEM_MODULE
	Vector2 get_minimum_size() const override {
	#else
	Vector2 _get_minimum_size() const override {
	#endif
		if (get_ability_system() == nullptr)
			return Vector2();
		RenderContainer container;
		container.max_size = Vector2(get_size().x, 0);
		auto dict = get_ability_system()->get_attribute_dict();
		auto keys = dict.keys();
		for (int i = 0; i < keys.size(); i++) {
			Ref<Attribute> attribute = keys[i];
			container.add_progress(attribute->get_identifier());
		}
		return container.total_size();
	}
};

#endif