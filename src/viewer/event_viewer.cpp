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
    String header = ability.is_valid() ? "Event: " + String(ability->get_identifier()) : "Event: <null>";
    container->add_label(header, ability.is_valid() ? ability->get_ui_color() : Color(0, 0, 0, 0));
}

void EventViewer::add_effects(RenderContainer *container, Ref<AbilityEvent> event) const {
    container->style = LabelStyle::outlined;
    Ref<Ability> ability = event->get_ability();
    TypedArray<Effect> effects = ability->get_effects();
    TypedArray<Effect> instances = event->get_effect_instances();

    for (int i = 0; i < effects.size(); i++) {
        Ref<Effect> effect = effects[i];
        Ref<Effect> instance = event->get_effect_instance(effect, i);
        // If the instance does not exist, it's already been finished and removed.
        Status status = instance.is_valid() ? instance->get_last_status() : Status::FINISHED;
        String text = fmt("{0}({1})", effect->get_class(), effect->get_ui_name());
        // For looped effects, add a counter.
        if (instance.is_valid() && instance->is_loop_effect())
            text += stringify(" (loop ", (int)(instance->get("elapsed_loops")) + 1, ")");
        switch (status) {
            case Status::RUNNING:
                container->add_label(text, effect->get_ui_color());
                break;
            case Status::FINISHED:
                container->add_text_strike_label(text, Color(effect->get_ui_color(), 0.5));
                break;
            default:
                container->add_text_label(text, Color(effect->get_ui_color(), 0.5));
        }

        // Add "→" or "&" icon
        if (i < (effects.size() - 1)) {
            String icon = ability->get_effect_mode() == EffectMode::PARALLEL ? " & " : String::utf8(" → ");
            container->add_text_label(icon, Color(ability->get_ui_color(), 0.5));
        }
    }
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
        if (event->get_ability().is_valid())
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