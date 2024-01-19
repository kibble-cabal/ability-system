#ifndef AS_TAGVIEWER_HPP
#define AS_TAGVIEWER_HPP

#include "viewer_base.hpp"

class TagViewer : public AbilitySystemViewerBase {
	GDCLASS(TagViewer, AbilitySystemViewerBase);

public:
	/* Override other event listeners to avoid unnecessary processing */
	void _attributes_changed() {}
	void _abilities_changed() {}
	void _events_changed() {}
	void _effects_changed() {}
	void _event_blocked() {}

	void _draw(AbilitySystem *ability_system) override {
		RenderContainer container;
		container.canvas = this;
		container.max_size = get_size();
		container.style = LabelStyle::outlined;
		for_each(ability_system->get_tags(), [&container](Ref<Tag> tag) {
			container.add_label(tag->get_identifier(), tag->get_ui_color());
		});
		container.draw();
	}

	Vector2 get_minimum_size() const override {
		if (get_ability_system() == nullptr)
			return Vector2();
		RenderContainer container;
		container.max_size = Vector2(get_size().x, 0);
		for_each(get_ability_system()->get_tags(), [&container](Ref<Tag> tag) {
			container.add_label(tag->get_identifier());
		});
		return container.total_size();
	}
};

#endif