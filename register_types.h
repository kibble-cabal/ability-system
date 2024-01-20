/* register_types.h */

#ifdef ABILITY_SYSTEM_MODULE
#include "modules/register_module_types.h"
#else
#include <gdextension_interface.h>
#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/godot.hpp>

using namespace godot;
#endif

void initialize_ability_system_module(ModuleInitializationLevel p_level);
void uninitialize_ability_system_module(ModuleInitializationLevel p_level);
/* yes, the word in the middle must be the same as the module folder name */