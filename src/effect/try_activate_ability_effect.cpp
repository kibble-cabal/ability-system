#include "try_activate_ability_effect.h"

#include <godot_cpp/variant/utility_functions.hpp>

#include "../ability_system.h"

void TryActivateAbilityEffect::_bind_methods() {
    BIND_GETSET(TryActivateAbilityEffect, ability);

    OBJECT_PROP(Ability, ability);
}

void TryActivateAbilityEffect::_start(AbilitySystem *owner) {
    if (ability.is_null()) {
        print_error("No ability provided!");
        return;
    }

    if (!owner->has_ability(ability)) {
        String message = fmt(
            "Owner is missing ability {0}! Has abilities: {1}",
            ability->to_string(),
            stringify(owner->get_abilities())
        );
        print_error(message);
    } else if (!owner->can_activate(ability))
        print_error(
            fmt("Owner cannot activate ability {0}!", ability->to_string())
        );

    owner->activate(ability);
}