/* register_types.cpp */

#include "register_types.h"

#include "core/object/class_db.h"
#include "src/ability.hpp"
#include "src/ability_event.h"
#include "src/ability_system.h"
#include "src/attribute.hpp"
#include "src/attribute_map.hpp"
#include "src/effect/attribute_effect.h"
#include "src/effect/tag_effect.h"
#include "src/effect/wait_effect.hpp"

// Editor
#include "src/editor/ability_viewer.hpp"
#include "src/editor/attribute_viewer.hpp"
#include "src/editor/event_viewer.hpp"
#include "src/editor/tag_viewer.hpp"

void initialize_ability_system_module(ModuleInitializationLevel p_level) {
	if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
		return;
	}
	ClassDB::register_class<Ability>();
	ClassDB::register_class<AbilityEvent>();
	ClassDB::register_class<Attribute>();
	ClassDB::register_class<Tag>();
	ClassDB::register_class<Effect>();
	ClassDB::register_class<WaitEffect>();
	ClassDB::register_class<AttributeEffect>();
	ClassDB::register_class<TagEffect>();
	ClassDB::register_class<AbilitySystem>();

	// Editor
	ClassDB::register_class<TagViewer>();
	ClassDB::register_class<AttributeViewer>();
	ClassDB::register_class<AbilityViewer>();
	ClassDB::register_class<EventViewer>();
}

void uninitialize_ability_system_module(ModuleInitializationLevel p_level) {
	if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
		return;
	}
	// Nothing to do here in this example.
}