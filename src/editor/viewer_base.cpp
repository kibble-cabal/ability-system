#include "viewer_base.h"
#include "render.cpp"

void AbilitySystemViewerBase::_bind_methods() {
    BIND_GETSET(AbilitySystemViewerBase, ability_system_path);
    PROP(Variant::NODE_PATH, ability_system_path);

    ClassDB::bind_method(D_METHOD("_abilities_changed"), &AbilitySystemViewerBase::_abilities_changed);
    ClassDB::bind_method(D_METHOD("_attributes_changed"), &AbilitySystemViewerBase::_attributes_changed);
    ClassDB::bind_method(D_METHOD("_tags_changed"), &AbilitySystemViewerBase::_tags_changed);
    ClassDB::bind_method(D_METHOD("_events_changed"), &AbilitySystemViewerBase::_events_changed);
    ClassDB::bind_method(D_METHOD("_effects_changed"), &AbilitySystemViewerBase::_effects_changed);
    ClassDB::bind_method(D_METHOD("_event_blocked"), &AbilitySystemViewerBase::_event_blocked);
}

void AbilitySystemViewerBase::connect_all() {
    auto ability_system = get_ability_system();
    if (ability_system != nullptr) {
        // Connect signals
        try_connect(ability_system, as_signal::AbilitiesChanged, callable_mp(this, &AbilitySystemViewerBase::_abilities_changed));
        try_connect(ability_system, as_signal::AttributesChanged, callable_mp(this, &AbilitySystemViewerBase::_attributes_changed));
        try_connect(ability_system, as_signal::TagsChanged, callable_mp(this, &AbilitySystemViewerBase::_tags_changed));
        try_connect(ability_system, as_signal::EventsChanged, callable_mp(this, &AbilitySystemViewerBase::_events_changed));
        try_connect(ability_system, as_signal::EffectsChanged, callable_mp(this, &AbilitySystemViewerBase::_effects_changed));
        try_connect(ability_system, as_signal::EventBlocked, callable_mp(this, &AbilitySystemViewerBase::_event_blocked));
        connect_to_tags();
        connect_to_attributes();
        connect_to_events();
        connect_to_abilities();
    }
}

void AbilitySystemViewerBase::connect_to_tags() {
    if (get_ability_system() != nullptr)
        for_each(get_ability_system()->get_tags(), [&](Ref<Tag> tag) {
            try_connect(tag.ptr(), "changed", Callable(this, "queue_redraw"));
        });
}

void AbilitySystemViewerBase::connect_to_attributes() {
    if (get_ability_system() != nullptr) {
        TypedArray<Attribute> keys = get_ability_system()->get_attribute_dict().keys();
        for_each(keys, [&](Ref<Attribute> attribute) {
            try_connect(attribute.ptr(), "changed", Callable(this, "queue_redraw"));
        });
    }
}

void AbilitySystemViewerBase::connect_to_abilities() {
    if (get_ability_system() != nullptr)
        for_each(get_ability_system()->get_abilities(), [&](Ref<Ability> ability) {
            try_connect(ability.ptr(), "changed", Callable(this, "queue_redraw"));
        });
}

void AbilitySystemViewerBase::connect_to_events() {
    if (get_ability_system() != nullptr)
        for_each(get_ability_system()->get_events(), [&](Ref<AbilityEvent> event) {
            try_connect(event.ptr(), "changed", Callable(this, "queue_redraw"));
        });
}


void AbilitySystemViewerBase::set_ability_system_path(NodePath value) {
    ability_system_path = value;
    connect_all();
}

AbilitySystem *AbilitySystemViewerBase::get_ability_system() const {
    #ifdef ABILITY_SYSTEM_MODULE
    if (is_inside_tree() && has_node(ability_system_path))
        return Object::cast_to<AbilitySystem>(get_node(ability_system_path));
    #else
    if (is_inside_tree() && is_node_ready() && has_node(ability_system_path))
        return get_node<AbilitySystem>(ability_system_path);
    #endif
    return nullptr;
}

void AbilitySystemViewerBase::_notification(int what) {
    switch (what) {
        case NOTIFICATION_ENTER_TREE:
        case NOTIFICATION_READY: 
            connect_all();
            break;
        case NOTIFICATION_DRAW:
            draw();
            break;
        case NOTIFICATION_VISIBILITY_CHANGED:
        case NOTIFICATION_RESIZED:
            queue_redraw();
            break;
    }
}

void AbilitySystemViewerBase::_tags_changed() {
    connect_to_tags();
    queue_redraw();
}

void AbilitySystemViewerBase::_attributes_changed() {
    connect_to_attributes();
    queue_redraw();
}

void AbilitySystemViewerBase::_abilities_changed() {
    connect_to_abilities();
    queue_redraw();
}

void AbilitySystemViewerBase::_events_changed() {
    connect_to_events();
    queue_redraw();
}

void AbilitySystemViewerBase::_effects_changed() {
    connect_to_events();
    queue_redraw();
}

void AbilitySystemViewerBase::_event_blocked(Ref<Ability> ability) {
    queue_redraw();
}

void AbilitySystemViewerBase::draw() {
    if (get_ability_system() != nullptr) {
        update_minimum_size();
        _draw();
    }
}