#include "tag_viewer.h"

#include "render.cpp"

void TagViewer::_attributes_changed() {}

void TagViewer::_abilities_changed() {}

void TagViewer::_events_changed() {}

void TagViewer::_effects_changed() {}

void TagViewer::_event_blocked() {}

void TagViewer::_draw() {
    auto ability_system = get_ability_system();
    if (ability_system == nullptr) return;
    RenderContainer container;
    container.canvas = this;
    container.max_size = get_size();
    container.style = LabelStyle::outlined;
    for_each(ability_system->get_tags(), [&container](Ref<Tag> tag) {
        if (tag.is_valid())
            container.add_label(tag->get_identifier(), tag->get_ui_color());
    });
    container.draw();
}

Vector2 TagViewer::_get_minimum_size() const {
    if (get_ability_system() == nullptr || !is_node_ready()) return Vector2();
    RenderContainer container;
    container.max_size = Vector2(get_size().x, 0);
    for_each(get_ability_system()->get_tags(), [&container](Ref<Tag> tag) {
        if (tag.is_valid()) container.add_label(tag->get_identifier());
    });
    return container.total_size();
}