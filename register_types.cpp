/* register_types.cpp */

#include "register_types.h"

#include "core/object/class_db.h"
#include "src/ability.hpp"
#include "src/ability_event.h"
#include "src/ability_system.h"
#include "src/attribute.hpp"
#include "src/attribute_map.hpp"

void initialize_ability_system_module(ModuleInitializationLevel p_level) {
	if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
		return;
	}
	ClassDB::register_class<Ability>();
	ClassDB::register_class<AbilityEvent>();

	ClassDB::register_class<Attribute>();
	ClassDB::register_class<AttributeMap>();

	ClassDB::register_class<Tag>();

	ClassDB::register_class<Effect>();

	ClassDB::register_class<AbilitySystem>();
	ClassDB::register_class<AbilitySystemState>();
}

void uninitialize_ability_system_module(ModuleInitializationLevel p_level) {
	if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
		return;
	}
	// Nothing to do here in this example.
}