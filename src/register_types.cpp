/* register_types.cpp */

#include "register_types.h"

#ifdef ABILITY_SYSTEM_MODULE
#include "core/object/class_db.h"
#else
#include <gdextension_interface.h>
#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/godot.hpp>
#include <godot_cpp/classes/editor_plugin_registration.hpp>
#endif

#include "ability.hpp"
#include "ability_event.h"
#include "ability_system.h"
#include "attribute.hpp"
#include "attribute_map.hpp"
#include "effect/attribute_effect.h"
#include "effect/tag_effect.h"
#include "effect/wait_effect.hpp"

// Editor
#include "editor/viewer_base.h"
#include "editor/ability_viewer.h"
#include "editor/attribute_viewer.h"
#include "editor/event_viewer.h"
#include "editor/tag_viewer.h"

// Editor: Attribute Inspector
#include "editor/attribute_inspector/plugin.hpp"
#include "editor/attribute_inspector/property.h"
#include "editor/attribute_inspector/editor.h"
#include "editor/attribute_inspector/editor_row.h"

// Editor: Tag Inspector
#include "editor/tag_inspector/plugin.hpp"
#include "editor/tag_inspector/property.h"
#include "editor/tag_inspector/editor.h"


void initialize_ability_system_module(ModuleInitializationLevel p_level) {
	if (p_level == MODULE_INITIALIZATION_LEVEL_SCENE) {
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
		ClassDB::register_class<AbilitySystemViewerBase>();
		ClassDB::register_class<TagViewer>();
		ClassDB::register_class<AttributeViewer>();
		ClassDB::register_class<AbilityViewer>();
		ClassDB::register_class<EventViewer>();
	}

	if (p_level == MODULE_INITIALIZATION_LEVEL_EDITOR) {
		// Editor: Attribute Inspector
		ClassDB::register_class<AttributeInspectorEditor>();
		ClassDB::register_class<AttributeInspectorEditorRow>();
		ClassDB::register_class<AttributeInspectorProperty>();
		ClassDB::register_class<AttributeInspectorPropertyPlugin>();
		ClassDB::register_class<AttributeInspectorPlugin>();
		EditorPlugins::add_by_type<AttributeInspectorPlugin>();
		// Editor: Tag Inspector
		ClassDB::register_class<TagInspectorEditor>();
		ClassDB::register_class<TagInspectorProperty>();
		ClassDB::register_class<TagInspectorPropertyPlugin>();
		ClassDB::register_class<TagInspectorPlugin>();
		EditorPlugins::add_by_type<TagInspectorPlugin>();
	}
}

void uninitialize_ability_system_module(ModuleInitializationLevel p_level) {}

#ifndef ABILITY_SYSTEM_MODULE
extern "C"
{
	// Initialization.
	GDExtensionBool GDE_EXPORT ability_system_init(GDExtensionInterfaceGetProcAddress p_get_proc_address, const GDExtensionClassLibraryPtr p_library, GDExtensionInitialization *r_initialization)
	{
		godot::GDExtensionBinding::InitObject init_obj(p_get_proc_address, p_library, r_initialization);

		init_obj.register_initializer(initialize_ability_system_module);
		init_obj.register_terminator(uninitialize_ability_system_module);
		init_obj.set_minimum_library_initialization_level(MODULE_INITIALIZATION_LEVEL_EDITOR);

		return init_obj.init();
	}
}
#endif