#include "attribute_viewer.h"

#include "render.cpp"

/* Override other event listeners to avoid unnecessary processing */
void AttributeViewer::_tags_changed() {}

void AttributeViewer::_abilities_changed() {}

void AttributeViewer::_events_changed() {}

void AttributeViewer::_effects_changed() {}

void AttributeViewer::_event_blocked() {}

void AttributeViewer::_draw() {
    auto ability_system = get_ability_system();
    if (ability_system == nullptr) return;
    RenderContainer container;
    container.canvas = this;
    container.max_size = get_size();
    container.style = LabelStyle::text;
    auto dict = ability_system->get_attributes();
    auto keys = dict.keys();
    for (int i = 0; i < keys.size(); i++) {
        Ref<Attribute> attribute = keys[i];
        if (attribute.is_null()) continue;
        float percent = ability_system->get_attribute_map()->get_percent(attribute);
        container.add_progress(attribute->get_identifier(), percent, attribute->get_ui_color());
    }
    container.draw();
}

Vector2 AttributeViewer::_get_minimum_size() const {
    if (get_ability_system() == nullptr) return Vector2();
    RenderContainer container;
    container.max_size = Vector2(get_size().x, 0);
    auto dict = get_ability_system()->get_attributes();
    auto keys = dict.keys();
    for (int i = 0; i < keys.size(); i++) {
        Ref<Attribute> attribute = keys[i];
        if (attribute.is_null()) continue;
        container.add_progress(attribute->get_identifier());
    }
    return container.total_size();
}