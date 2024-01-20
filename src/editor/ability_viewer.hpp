#ifndef AS_ABILITYVIEWER_HPP
#define AS_ABILITYVIEWER_HPP

#include "viewer_base.hpp"

class AbilityViewer : public AbilitySystemViewerBase {
	GDCLASS(AbilityViewer, AbilitySystemViewerBase);

public:
	/* Override other event listeners to avoid unnecessary processing */
	void _attributes_changed() {}
	void _tags_changed() {}
	void _events_changed() {}
	void _effects_changed() {}
	void _event_blocked() {}

	void _draw(AbilitySystem *ability_system) override {
		RenderContainer container;
		container.canvas = this;
		container.max_size = get_size();
		container.style = LabelStyle::filled;
		for_each(ability_system->get_abilities(), [&container](Ref<Ability> ability) {
			container.add_label(ability->get_identifier(), ability->get_ui_color());
		});
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
		for_each(get_ability_system()->get_abilities(), [&container](Ref<Ability> ability) {
			container.add_label(ability->get_identifier());
		});
		return container.total_size();
	}
};

#endif