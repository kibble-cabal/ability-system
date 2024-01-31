#include "ability_viewer.h"
#include "render.cpp"

void AbilityViewer::_attributes_changed() {}
void AbilityViewer::_tags_changed() {}
void AbilityViewer::_events_changed() {}
void AbilityViewer::_effects_changed() {}
void AbilityViewer::_event_blocked() {}

void AbilityViewer::_draw() {
    auto ability_system = get_ability_system();
    if (ability_system == nullptr)
        return;
    RenderContainer container;
    container.canvas = this;
    container.max_size = get_size();
    container.style = LabelStyle::filled;
    for_each(ability_system->get_abilities(), [&container](Ref<Ability> ability) {
        if (ability.is_valid())
            container.add_label(ability->get_identifier(), ability->get_ui_color());
    });
    container.draw();
}

Vector2 AbilityViewer::_get_minimum_size() const {
    if (get_ability_system() == nullptr)
        return Vector2();
    RenderContainer container;
    container.max_size = Vector2(get_size().x, 0);
    for_each(get_ability_system()->get_abilities(), [&container](Ref<Ability> ability) {
        if (ability.is_valid())
            container.add_label(ability->get_identifier());
    });
    return container.total_size();
}