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

	void _draw(AbilitySystem *ability_system) override {
		RenderLabelContainer container;
		container.canvas = this;
		container.max_size = get_size();
		for_each(ability_system->get_abilities(), [&container](Ref<Ability> ability) {
			container.draw_label_filled(ability->get_identifier(), ability->get_ui_color());
		});
	}

	Vector2 get_minimum_size() const override {
		if (get_ability_system() == nullptr)
			return Vector2();
		RenderLabelContainer container;
		container.max_size = Vector2(get_size().x, 0);
		for_each(get_ability_system()->get_abilities(), [&container](Ref<Ability> ability) {
			container.add(ability->get_identifier());
		});
		return container.total_size();
	}
};

#endif