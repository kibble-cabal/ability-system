/* register_types.cpp */

#include "register_types.h"

#include <gdextension_interface.h>

#include <godot_cpp/classes/editor_plugin_registration.hpp>
#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/godot.hpp>

#include "ability.hpp"
#include "ability_event.h"
#include "ability_system.h"
#include "attribute.hpp"
#include "attribute_map.hpp"
#include "effect/attribute_effect.h"
#include "effect/loop_effect.h"
#include "effect/tag_effect.h"
#include "effect/try_activate_ability_effect.h"
#include "effect/wait_effect.hpp"
#include "project_settings.h"

// Viewer
#include "viewer/ability_system_viewer.hpp"
#include "viewer/ability_viewer.h"
#include "viewer/attribute_viewer.h"
#include "viewer/event_viewer.h"
#include "viewer/tag_viewer.h"
#include "viewer/viewer_base.h"

// Editor: Attribute Inspector
#include "editor/attribute_inspector/editor.h"
#include "editor/attribute_inspector/editor_row.h"
#include "editor/attribute_inspector/plugin.hpp"
#include "editor/attribute_inspector/property.h"

// Editor: Tag Inspector
#include "editor/tag_inspector/editor.h"
#include "editor/tag_inspector/plugin.hpp"
#include "editor/tag_inspector/property.h"

// Editor: CSharp binding generator
#include "editor/csharp/editor_menu_extension.h"

void initialize_ability_system_module(ModuleInitializationLevel p_level) {
    if (p_level == MODULE_INITIALIZATION_LEVEL_SCENE) {
        ASProjectSettings::register_settings();

        ClassDB::register_class<Ability>();
        ClassDB::register_class<AbilityEvent>();
        ClassDB::register_class<Attribute>();
        ClassDB::register_class<Tag>();
        ClassDB::register_class<AbilitySystem>();
        ClassDB::register_class<Effect>();

        // Effects
        ClassDB::register_class<WaitEffect>();
        ClassDB::register_class<AttributeEffect>();
        ClassDB::register_class<TagEffect>();
        ClassDB::register_class<TryActivateAbilityEffect>();
        ClassDB::register_class<LoopEffect>();

        // Viewers
        ClassDB::register_internal_class<AbilitySystemViewerBase>();
        ClassDB::register_class<AbilitySystemViewer>();
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

        // Editor: CSharp binding generator
        ClassDB::register_class<AbilitySystemMenuExtensionPlugin>();
        EditorPlugins::add_by_type<AbilitySystemMenuExtensionPlugin>();
    }
}

void uninitialize_ability_system_module(ModuleInitializationLevel p_level) {}

extern "C" {
    // Initialization.
    GDExtensionBool GDE_EXPORT ability_system_init(
        GDExtensionInterfaceGetProcAddress p_get_proc_address,
        const GDExtensionClassLibraryPtr p_library,
        GDExtensionInitialization* r_initialization
    ) {
        godot::GDExtensionBinding::InitObject init_obj(
            p_get_proc_address,
            p_library,
            r_initialization
        );

        init_obj.register_initializer(initialize_ability_system_module);
        init_obj.register_terminator(uninitialize_ability_system_module);
        init_obj.set_minimum_library_initialization_level(MODULE_INITIALIZATION_LEVEL_EDITOR);

        return init_obj.init();
    }
}