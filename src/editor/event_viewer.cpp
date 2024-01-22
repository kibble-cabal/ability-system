#include "event_viewer.h"

#include "render.cpp"

RenderContainer EventViewer::make_container_const(float y_offset) const {
    RenderContainer container;
    container.max_size = Vector2(get_size().x, 0);
    container.rect.position = Vector2(0, y_offset);
    container.style = LabelStyle::text;
    return container;
}

RenderContainer EventViewer::make_container(float y_offset) {
    auto container = make_container_const(y_offset);
    container.canvas = this;
    return container;
}

void EventViewer::add_header(RenderContainer *container, Ref<AbilityEvent> event) const {
    Ref<Ability> ability = event->get_ability();
    String header = "Event: " + String(ability->get_identifier());
    container->add_label(header, ability->get_ui_color());
}

void EventViewer::add_effects(RenderContainer *container, Ref<AbilityEvent> event) const {
    container->style = LabelStyle::outlined;
    for_each(event->get_effect_instances(), [&](Ref<Effect> effect) {
        container->add_label(effect->get_ui_name(), effect->get_ui_color());
    });
}

/* Override other event listeners to avoid unnecessary processing */
void EventViewer::_attributes_changed() {}
void EventViewer::_tags_changed() {}
void EventViewer::_abilities_changed() {}

void EventViewer::_draw() {
    auto ability_system = get_ability_system();
    if (ability_system == nullptr) 
        return;
    float y_offset = 0;
    for_each(ability_system->get_events(), [&](Ref<AbilityEvent> event) {
        RenderContainer container = make_container(y_offset);
        add_header(&container, event);
        container.new_line();
        add_effects(&container, event);
        container.draw();
        container.draw_outline(Color(event->get_ability()->get_ui_color(), 0.5));
        y_offset += container.total_size().height + 2;
    });
}

Vector2 EventViewer::_get_minimum_size() const {
    if (get_ability_system() == nullptr)
        return Vector2();
    Rect2 total_rect;
    for_each(get_ability_system()->get_events(), [&](Ref<AbilityEvent> event) {
        RenderContainer container = make_container_const(total_rect.size.height);
        add_header(&container, event);
        container.new_line();
        add_effects(&container, event);
        total_rect = total_rect.merge(container.rect);
    });
    return total_rect.size;
}