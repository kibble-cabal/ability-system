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

	void _draw(AbilitySystem *ability_system) override {
		RenderLabelContainer container;
		container.canvas = this;
		container.max_size = get_size();
		for_each(ability_system->get_tags(), [&container](Ref<Tag> tag) {
			container.draw_label_outlined(tag->get_identifier(), tag->get_ui_color());
		});
	}

	Vector2 get_minimum_size() const override {
		if (get_ability_system() == nullptr)
			return Vector2();
		RenderLabelContainer container;
		container.max_size = Vector2(get_size().x, 0);
		for_each(get_ability_system()->get_tags(), [&container](Ref<Tag> tag) {
			container.add(tag->get_identifier());
		});
		return container.total_size();
	}
};

#endif