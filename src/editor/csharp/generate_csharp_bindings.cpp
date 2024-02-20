// Most of this code is borrowed from this addon, with some modifications:
// https://github.com/DmitriySalnikov/godot_debug_draw_3d

#include "generate_csharp_bindings.h"

#include <algorithm>
#include <godot_cpp/classes/dir_access.hpp>
#include <godot_cpp/classes/input.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

#include "../../macros.hpp"
#include "../../project_settings.h"

#define version "0.1.0-alpha"

// add auto-braces
#define TAB() auto tab_##__LINE__ = tab()

static godot::String pascal_case(godot::String string) {
    return string.to_pascal_case().replace("Ui", "UI");
}

static bool is_group(int usage) {
    return (
        usage & PROPERTY_USAGE_GROUP || usage & PROPERTY_USAGE_SUBGROUP
        || usage & PROPERTY_USAGE_CATEGORY
    );
}

GenerateCSharpBindingsPlugin::IndentGuard::IndentGuard(GenerateCSharpBindingsPlugin *_owner) {
    owner = _owner;
    owner->line("{");
    owner->indent += owner->indent_template;
}

GenerateCSharpBindingsPlugin::IndentGuard::~IndentGuard() {
    if (owner) {
        owner->indent
            = owner->indent.substr(0, owner->indent.length() - owner->indent_template.length());
        owner->line("}");
    }
}

bool GenerateCSharpBindingsPlugin::is_need_to_update() {
    if (!ClassDB::class_exists("CSharpScript")) return false;

    const String api_path = ASProjectSettings::get_csharp_generation_path();
    if (FileAccess::file_exists(api_path)) {
        auto file = FileAccess::open(api_path, FileAccess::READ);
        ERR_FAIL_COND_V(FileAccess::get_open_error() != Error::OK, false);

        String first_line = file->get_line();
        if (first_line.begins_with("/// ")) {
            String ver = first_line.substr(4);
            return ver.strip_edges() != version;
        }
    }
    return true;
}

void GenerateCSharpBindingsPlugin::generate() {
    String output_directory = ASProjectSettings::get_csharp_generation_directory();
    String api_file_name = ASProjectSettings::get_csharp_generation_file_name();

    const String out_path = ASProjectSettings::get_csharp_generation_path();
    const String out_log_path = output_directory.path_join(log_file_name);

    ERR_FAIL_COND(DirAccess::make_dir_recursive_absolute(output_directory) != Error::OK);
    auto dir = DirAccess::open(output_directory);

    // Clear tmp files
    PackedStringArray files = dir->get_files();
    for (int i = 0; i < files.size(); i++) {
        String f = files[i];
        if ((f.begins_with(api_file_name) || f.begins_with("log")) && f.ends_with(".tmp"))
            dir->remove(f);
    }

    // First, delete the old file to check for locks
    if (FileAccess::file_exists(out_path)) {
        print("Attempt to delete an old file: " + out_path);
        ERR_FAIL_COND(dir->remove(out_path) != Error::OK);
    }

    opened_file = FileAccess::open(out_path, FileAccess::ModeFlags::WRITE);
    ERR_FAIL_COND(FileAccess::get_open_error() != Error::OK);

    opened_log_file = FileAccess::open(out_log_path, FileAccess::ModeFlags::WRITE);
    ERR_FAIL_COND(FileAccess::get_open_error() != Error::OK);

    property_method_prefix = TypedArray<String>(Array::make("set", "get", "is"));

    generate_for_classes = TypedArray<StringName>(Array::make(
        "Ability",
        "AbilityEvent",
        "AbilitySystem",
        "Attribute",
        "Tag",
        "Effect",
        "AttributeEffect",
        "LoopEffect",
        "TagEffect",
        "TryActivateAbilityEffect",
        "WaitEffect",
        "AbilitySystemViewer",
        "AttributeViewer",
        "AbilityViewer",
        "EventViewer",
        "TagViewer"
    ));

    avoid_caching_for_classes = TypedArray<StringName>();

    additional_statics_for_classes = extend_class_strings {};

    override_disposable_for_classes = extend_class_strings {};

    singletons = Engine::get_singleton()->get_singleton_list();

    generate_header();

    line("using Godot;");
    line("using System;");
    line("using System.Linq;");
    line("#nullable disable");

    force_log("Generation of bindings started, output file: " + out_path);
    force_log("Log file: " + out_log_path);
    force_log(
        "Hold Shift to print information on the Output panel when manually starting generation via "
        "the 'Project - Tools - Ability System' menu"
    );

    is_shift_pressed = Input::get_singleton()->is_key_pressed(KEY_SHIFT);

    remap_data remapped_data;
    for (int i = 0; i < generate_for_classes.size(); i++) {
        generate_class(generate_for_classes[i], remapped_data);
    }

    line();
    generate_class_utilities(remapped_data);

    force_log("The generation process is completed!");
}

void GenerateCSharpBindingsPlugin::generate_header() {
    line(fmt("/// {0}", version));
    line("/// ////////////////////////////////////////////////");
    line("/// THIS FILE HAS BEEN GENERATED.");
    line("/// THE CHANGES IN THIS FILE WILL BE OVERWRITTEN");
    line("/// AFTER THE UPDATE OR AFTER THE RESTART!");
    line("/// ////////////////////////////////////////////////");
    line();
}

void GenerateCSharpBindingsPlugin::generate_class(
    const StringName &cls, remap_data &remapped_data
) {
    log("Class: " + cls, 1);

    StringName parent_name = ClassDB::get_parent_class(cls);
    PackedStringArray enum_names = ClassDB::class_get_enum_list(cls, true);
    TypedArray<Dictionary> methods = ClassDB::class_get_method_list(cls, true);
    TypedArray<Dictionary> signals = ClassDB::class_get_signal_list(cls, true);
    TypedArray<Dictionary> properties = ClassDB::class_get_property_list(cls, true);
    std::map<String, ArgumentData> properties_map;

    bool is_preserved_inheritance = generate_for_classes.has(parent_name);
    bool is_singleton = singletons.has(cls);

    line();
    // class AbilitySystemFPSGraph : AbilitySystemGraph
    String static_mod_str = is_singleton ? "static " : "";
    String additional_inheritance = "";

    if (override_disposable_for_classes.find(cls) != override_disposable_for_classes.end()) {
        additional_inheritance += ", IDisposable";
    }

    if (is_preserved_inheritance) {
        line(fmt("public partial class {0} : {1}{2}", cls, parent_name, additional_inheritance));
    } else {
        line(
            fmt("{0}public partial class {1}{2}{3}",
                static_mod_str,
                cls,
                is_singleton
                    ? ""
                    : fmt(" : _AbilitySystemInstanceWrapper_<{0}>", get_native_base_class(cls)),
                additional_inheritance)
        );
    }

    {
        TAB();

        // Wrapper
        {
            log("Wrapper...", 2);
            generate_wrapper(cls, is_singleton, is_preserved_inheritance);
        }

        // Consts
        {
            log("Constants...", 2);
            generate_constants(cls);
        }

        // Enums
        {
            log("Enums...", 2);
            for (int i = 0; i < enum_names.size(); i++) {
                generate_enum(cls, enum_names[i]);
            }
        }

        // Methods
        {
            log("Methods...", 2);

            TypedArray<String> is_property;

            for (int i = 0; i < methods.size(); i++) {
                String name = ((Dictionary)methods[i])["name"];
                auto split = name.split("_", true, 1);

                for (int j = 0; j < properties.size(); j++) {
                    String prop_name = ((Dictionary)properties[j])["name"];

                    // check for prefix (set, get, is) and name of property
                    if (split.size() == 2 && split[1] == prop_name
                        && (property_method_prefix.has(split[0].to_lower()))) {
                        is_property.append(name);

                        std::map<String, ArgumentData>::iterator it
                            = properties_map.find(prop_name);
                        if (it == properties_map.end()) {
                            properties_map[prop_name] = ArgumentData();
                            it = properties_map.find(prop_name);
                        }

                        if (name.begins_with("set")) {
                        } else {
                            ArgumentData data = argument_parse(((Dictionary)methods[i])["return"]);
                            data.name = prop_name;
                            (*it).second = data;
                        }
                    }
                }
            }

            int added = 0;
            line("public static new class MethodName");
            {
                TAB();
                for (int i = 0; i < methods.size(); i++) {
                    String name = ((Dictionary)methods[i])["name"];
                    if (!name.begins_with("_") && !is_property.has(name)) {
                        added++;
                        line(
                            fmt("public static readonly StringName {0} = \"{1}\";",
                                pascal_case(name),
                                name)
                        );
                    }
                }
            }
            line();

            const auto &it_add = additional_statics_for_classes.find(cls);
            if (it_add != additional_statics_for_classes.end()) {
                line("// Additional custom statics");
                for (const auto &i : it_add->second) {
                    added++;
                    line(i);
                }
            }

            if (added) line();

            const auto &it_over = override_disposable_for_classes.find(cls);
            if (it_over != override_disposable_for_classes.end()) {
                line("// Custom Disposable");
                line("public new void Dispose()");
                {
                    TAB();
                    for (const auto &i : it_over->second) {
                        line(i);
                    }
                }
                line();
            }

            for (int i = 0; i < methods.size(); i++) {
                String name = ((Dictionary)methods[i])["name"];

                if (name.begins_with("_")) {
                    continue;
                }

                if (!is_property.has(name))
                    generate_method(cls, methods[i], is_singleton, remapped_data);
            }
        }

        // Properties
        {
            log("Properties...", 2);
            generate_properties(cls, properties, properties_map, is_singleton);
        }
    }
}

void GenerateCSharpBindingsPlugin::generate_class_utilities(const remap_data &remapped_data) {
    log("AbilitySystem utilities:", 1);

    line("public interface _IAbilitySystemInstanceWrapper_ : IDisposable");
    {
        TAB();
        line("void ClearNativePointer();");
    }
    line();
    line(
        "public partial class _AbilitySystemInstanceWrapper_<T> : "
        "_IAbilitySystemInstanceWrapper_ where T: GodotObject"
    );
    {
        TAB();
        line("public static class Enum { }");
        line("public static class PropertyName { }");
        line("public static class MethodName { }");
        line("public static class SignalName { }");
        line("public T Instance { get; protected set; }");
        line();
        line("public _AbilitySystemInstanceWrapper_(T _instance)");
        {
            TAB();
            line("if (_instance == null) throw new ArgumentNullException(nameof(_instance));");
            line(
                "if (!ClassDB.IsParentClass(_instance.GetClass(), GetType().Name)) throw new "
                "ArgumentException(\"\\\"_instance\\\" has the wrong type.\");"
            );
            line("Instance = _instance;");
            if (is_generate_unload_event) {
                line("_AbilitySystemUtils_.ExtensionUnloading += OnUnloading;");
            }
        }
        if (is_generate_unload_event) {
            line();
            line("void OnUnloading()");
            {
                TAB();
                line("if (Instance != null)");
                {
                    TAB();
                    line("GD.Print($\"Unload {GetType()}, {Instance.NativeInstance}\");");
                }
                line("try");
                {
                    TAB();
                    line("_AbilitySystemUtils_.ExtensionUnloading -= OnUnloading;");
                }
                line("catch {}");
                line("Instance = null;");
            }
        }
        line();
        line("public void Dispose()");
        {
            TAB();
            line("Instance?.Dispose();");
            line("Instance = null;");
        }
        line();
        line("public void ClearNativePointer() => Instance = null;");
    }
    line();

    line("internal static class _AbilitySystemUtils_");
    {
        TAB();

        if (is_generate_unload_event) {
            line("public static event Action ExtensionUnloading");
            {
                TAB();
                line("add");
                {
                    TAB();
                    line(
                        "Engine.GetSingleton(\"AbilitySystemManager\").Connect(\"extension_"
                        "unloading\","
                        " Callable.From(value), (uint)GodotObject.ConnectFlags.OneShot);"
                    );
                }

                line("remove");
                {
                    TAB();
                    line(
                        "Engine.GetSingleton(\"AbilitySystemManager\").Disconnect(\"extension_"
                        "unloading\", Callable.From(value));"
                    );
                }
            }
            line();
        }
        line();

        log("Arguments remap...", 2);
        // Default data
        generate_default_arguments_remap(remapped_data);

        // Factory
        log("Class factory...", 2);
        {
            line(
                "static System.Collections.Generic.Dictionary<ulong, "
                "_IAbilitySystemInstanceWrapper_> "
                "cached_instances = new();"
            );
            line("static DateTime previous_clear_time = DateTime.Now;");
            line();

            line("public static object CreateWrapperFromObject(GodotObject _instance)");
            {
                TAB();
                line("if (_instance == null)");
                {
                    TAB();
                    line("return null;");
                }

                line("");
                line("ulong id = _instance.GetInstanceId();");
                line("if (cached_instances.ContainsKey(id))");
                {
                    TAB();
                    line("return cached_instances[id];");
                }
                line("");
                line("if ((DateTime.Now - previous_clear_time).TotalSeconds > 1)");
                {
                    TAB();
                    line(
                        "var query = cached_instances.Where((i) => "
                        "GodotObject.IsInstanceIdValid(i.Key)).ToArray();"
                    );
                    line("foreach (var i in query)");
                    {
                        TAB();
                        line("i.Value.ClearNativePointer();");
                        line("cached_instances.Remove(i.Key);");
                    }
                    line("previous_clear_time = DateTime.Now;");
                }
                line("");

                line("switch(_instance.GetClass())");
                {
                    TAB();
                    for (int i = 0; i < generate_for_classes.size(); i++) {
                        StringName cls = generate_for_classes[i];
                        StringName base_cls = get_native_base_class(cls);
                        if (!singletons.has(cls)) {
                            line(fmt("case \"{0}\":", cls));
                            {
                                TAB();
                                line(
                                    fmt("_IAbilitySystemInstanceWrapper_ new_instance = new "
                                        "{0}(({1})_instance);",
                                        cls,
                                        base_cls)
                                );
                                if (!avoid_caching_for_classes.has(cls)) {
                                    line("cached_instances[id] = new_instance;");
                                }
                                line("return new_instance;");
                            }
                        }
                    }
                }
                line("throw new NotImplementedException();");
            }
        }
    }
}

void GenerateCSharpBindingsPlugin::generate_wrapper(
    const StringName &cls, bool is_static, bool inheritance
) {
    if (is_static) {
        String lowered_name = cls;

        if (is_generate_unload_event) {
            line("static bool _is_connected = false;");
        }

        line("private static GodotObject _instance;");
        line("public static GodotObject Instance");

        {
            TAB();
            line("get");
            {
                TAB();
                if (is_generate_unload_event) {
                    line("if (!_is_connected)");
                    {
                        TAB();
                        line("_AbilitySystemUtils_.ExtensionUnloading += OnUnloading;");
                        line("_is_connected = true;");
                    }
                    line();
                }
                {
                    line("if (!GodotObject.IsInstanceValid(_instance))");
                    {
                        TAB();
                        line(fmt("_instance = Engine.GetSingleton(\"{0}\");", cls));
                    }
                    line("return _instance;");
                }
            }
        }

        if (is_generate_unload_event) {
            line();

            line("static void OnUnloading()");
            {
                TAB();
                line("try");
                {
                    TAB();
                    line("_AbilitySystemUtils_.ExtensionUnloading -= OnUnloading;");
                }
                line("catch {}");
                line("_instance = null;");
                line("_is_connected = false;");
            }
            line("#endif", 0);
            line();
        }
    } else {
        StringName base_cls = get_native_base_class(cls);
        line(fmt("public {0}({1} _instance) : base (_instance) {}", cls, base_cls));
        line();
        line(fmt("public {0}() : this(({1})ClassDB.Instantiate(\"{0}\")) { }", cls, base_cls));
    }
    line();
}

void GenerateCSharpBindingsPlugin::generate_constants(const StringName &cls) {
    int added_items = 0;
    PackedStringArray consts = ClassDB::class_get_integer_constant_list(cls, true);

    for (int i = 0; i < consts.size(); i++) {
        StringName belongs_to = ClassDB::class_get_integer_constant_enum(cls, consts[i], true);
        if (belongs_to.is_empty()) {
            log(consts[i], 3);

            line(
                fmt("public const long {0} = {1};",
                    consts[i],
                    ClassDB::class_get_integer_constant(cls, consts[i]))
            );
            added_items++;
        }
    }
    if (added_items) {
        line();
    }
}

void GenerateCSharpBindingsPlugin::generate_enum(const StringName &cls, const StringName &enm) {
    log(enm, 3);

    int added_items = 0;
    line("public static new class Enum");
    {
        TAB();

        line(fmt("public enum {0} : long", enm));
        {
            TAB();

            PackedStringArray enum_values = ClassDB::class_get_enum_constants(cls, enm, true);

            String prefix;
            bool has_prefix = false;
            if (enum_values.size()) {
                String first_enum = enum_values[0];
                auto test_split = first_enum.split("_");

                for (int i = 0; i < enum_values.size() - 1; i++) {
                    auto split = first_enum.rsplit("_", i);
                    String test_prefix = split[0];

                    bool full_match = true;
                    for (int j = 1; j < enum_values.size(); j++) {
                        if (!enum_values[j].begins_with(test_prefix)) {
                            full_match = false;
                            break;
                        }
                    }

                    if (full_match) {
                        prefix = test_prefix;
                        has_prefix = true;
                    }
                }
            }

            for (int i = 0; i < enum_values.size(); i++) {
                String enum_new_name = pascal_case(enum_values[i].substr(prefix.length()));
                line(
                    fmt("{0} = {1},",
                        enum_new_name,
                        ClassDB::class_get_integer_constant(cls, enum_values[i]))
                );
                added_items++;
            }
        }
    }
    line();
}

void GenerateCSharpBindingsPlugin::generate_method(
    const StringName &cls, const Dictionary &method, bool is_static, remap_data &remapped_data
) {
    String name = (String)method["name"];

    log(name, 3);

    Dictionary return_dict = method["return"];
    ArgumentData return_data = argument_parse(return_dict, true);
    bool is_need_wrapper = generate_for_classes.has(return_data.type_name);

    String static_mod_str = is_static ? "static " : "";

    std::vector<DefaultData> default_args
        = arguments_parse_values(method["args"], method["default_args"], remapped_data);

    String method_body = "";

    String method_signature = fmt(
        "public {0}{1} {2}({3})",
        static_mod_str,
        return_data.type_name,
        pascal_case(method["name"]),
        arguments_string_decl(method["args"], true, default_args)
    );

    String call_args = arguments_string_call(method["args"], default_args);
    if (!call_args.is_empty()) {
        call_args = ", " + call_args;
    }

    if (!return_data.is_void) {
        String int_convert = return_data.is_enum ? "(long)" : "";

        if (is_need_wrapper) {
            method_body = fmt(
                "({0})_AbilitySystemUtils_.CreateWrapperFromObject((GodotObject)"
                "Instance?."
                "Call(MethodName.{1}{2}));",
                return_data.type_name,
                pascal_case(name),
                call_args
            );
        } else {
            method_body = fmt(
                "({0})({1}Instance?.Call(MethodName.{2}{3}));",
                return_data.type_name,
                int_convert,
                pascal_case(name),
                call_args
            );
        }
    } else {
        method_body = fmt("Instance?.Call(MethodName.{0}{1});", pascal_case(name), call_args);
    }

    line(fmt("{0} => {1}", method_signature, method_body));

    line();
}

void GenerateCSharpBindingsPlugin::generate_default_arguments_remap(const remap_data &remapped_data
) {
    if (!remapped_data.size()) return;

    line("public static class DefaultArgumentsData");
    {
        TAB();
        for (const auto &it : remapped_data) {
            line(
                fmt("public static readonly {0} {1} = {2};",
                    it.second.type_name,
                    it.first,
                    it.second.arg_string)
            );
        }
    }
    line();
}

void GenerateCSharpBindingsPlugin::generate_properties(
    const StringName &cls,
    const TypedArray<Dictionary> &props,
    std::map<String, ArgumentData> setget_map,
    bool is_static
) {
    // ClassDB SetGet is faster than calling set/get methods
    // Iterations: 100000
    // Methods Get: 50,967 ms
    // Methods Set: 60,477 ms
    // ClassDB_SetGet Get: 28,660 ms
    // ClassDB_SetGet Set: 26,918 ms

    line("public static new class PropertyName");
    {
        TAB();
        for (int i = 0; i < props.size(); i++) {
            Dictionary dict = props[i];
            String name = dict["name"];
            int usage = dict["usage"];
            if (is_group((int)dict["usage"])) continue;
            line(fmt("public static readonly StringName {0} = \"{1}\";", pascal_case(name), name));
        }
    }
    line();

    for (int i = 0; i < props.size(); i++) {
        Dictionary dict = props[i];
        if (is_group(dict["usage"])) continue;

        ArgumentData setget = setget_map[dict["name"]];
        bool is_need_wrapper = generate_for_classes.has(setget.type_name);
        String static_mod_str = is_static ? "static " : "";

        log(setget.name, 3);

        line(fmt("public {0}{1} {2}", static_mod_str, setget.type_name, pascal_case(setget.name)));
        {
            TAB();
            if (is_need_wrapper) {
                line(
                    fmt("get => "
                        "({0})_AbilitySystemUtils_.CreateWrapperFromObject((GodotObject)ClassDB."
                        "ClassGetProperty(Instance, PropertyName.{1}));",
                        setget.type_name,
                        pascal_case(setget.name))
                );
                line(fmt(
                    "set => ClassDB.ClassSetProperty(Instance, PropertyName.{0}, value.Instance);",
                    pascal_case(setget.name)
                ));
            } else if (setget.is_enum) {
                line(
                    fmt("get => ({0})(long)ClassDB.ClassGetProperty(Instance, PropertyName.{1});",
                        setget.type_name,
                        pascal_case(setget.name))
                );
                line(
                    fmt("set => ClassDB.ClassSetProperty(Instance, PropertyName.{0}, (long)value);",
                        pascal_case(setget.name))
                );
            } else {
                line(
                    fmt("get => ({0})ClassDB.ClassGetProperty(Instance, PropertyName.{1});",
                        setget.type_name,
                        pascal_case(setget.name))
                );
                line(
                    fmt("set => ClassDB.ClassSetProperty(Instance, PropertyName.{0}, value);",
                        pascal_case(setget.name))
                );
            }
        }
        line();
    }
}

GenerateCSharpBindingsPlugin::ArgumentData GenerateCSharpBindingsPlugin::argument_parse(
    const Dictionary &arg, bool is_return
) {
    StringName class_name = arg["class_name"];
    String name = arg["name"];
    Variant::Type var_type = (Variant::Type)(int)arg["type"];
    PropertyHint hint = (PropertyHint)(int)arg["hint"];
    String hint_string = arg["hint_string"];

    if (!class_name.is_empty()) {
        // Enum
        if (var_type == Variant::INT) {
            return ArgumentData(name, var_type, class_name, hint, hint_string, true);
        }
        // Rename Object
        else if (class_name == StringName("Object"))
            return ArgumentData(name, var_type, StringName("GodotObject"), hint, hint_string);

        return ArgumentData(name, var_type, class_name, hint, hint_string);
    } else if (var_type) {
        return ArgumentData(name, var_type, types_map[var_type], hint, hint_string);
    }

    if (is_return) {
        return ArgumentData(name, var_type, "void", hint, hint_string);
    } else {
        return ArgumentData(name, var_type, "Variant", hint, hint_string);
    }
}

GenerateCSharpBindingsPlugin::ArgumentData GenerateCSharpBindingsPlugin::argument_parse(
    const StringName &class_name,
    const String &name,
    const Variant::Type type,
    const PropertyHint hint,
    const String &hint_string
) {
    return argument_parse(dictionary(
        "class_name",
        class_name,
        "name",
        name,
        "type",
        type,
        "hint",
        hint,
        "hint_string",
        hint_string
    ));
}

std::vector<GenerateCSharpBindingsPlugin::DefaultData>
GenerateCSharpBindingsPlugin::arguments_parse_values(
    const TypedArray<Dictionary> &args, const Array &def_args, remap_data &remapped_data
) {
    std::vector<DefaultData> res;

    int start_idx = (int)(args.size() - def_args.size());
    for (int i = start_idx; i < args.size(); i++) {
        DefaultData dd
            = arguments_get_formatted_value(argument_parse(args[i]), def_args[i - start_idx]);
        if (dd.is_need_remap) {
            String remap_name = fmt("arg_{0}", (int)remapped_data.size());
            for (const auto &it : remapped_data) {
                if (it.second.is_equal_data(dd)) {
                    remap_name = it.first;
                    log(fmt("{0} will be remapped to {1}", dd.name, it.first), 4);
                }
            }

            remapped_data[remap_name] = dd;
            dd.arg_string = "_AbilitySystemUtils_.DefaultArgumentsData." + remap_name;
            res.push_back(dd);
        } else {
            res.push_back(dd);
        }
    }

    return res;
}

GenerateCSharpBindingsPlugin::DefaultData
GenerateCSharpBindingsPlugin::arguments_get_formatted_value(
    const ArgumentData &arg_data, const Variant &def_val
) {
    if (arg_data.is_enum) {
        return DefaultData(
            arg_data.name,
            arg_data.type_name,
            false,
            fmt("({0}){1}", arg_data.type_name, def_val)
        );
    } else {
#define IS_DEF(_type)    \
    _type val = def_val; \
    bool _need_remap = (val != _type())

        auto fp = [](const real_t &f) {
            if (f == INFINITY) {
                return String("float.PositiveInfinity");
            } else if (f == -INFINITY) {
                return String("float.NegativeInfinity");
            } else {
                return fmt("{0}f", f);
            }
        };

        switch (arg_data.type) {
            case godot::Variant::NIL:  // aka Variant
                if (def_val.get_type() == 0) {
                    return DefaultData(arg_data, true, "default");
                } else {
                    ArgumentData tmp_arg = arg_data;
                    tmp_arg.type = def_val.get_type();
                    DefaultData dd = arguments_get_formatted_value(tmp_arg, def_val);

                    return DefaultData(
                        dd.name,
                        arg_data.type_name,
                        true,
                        fmt("(Variant)({0})", dd.arg_string)
                    );
                }
                break;
            case godot::Variant::BOOL:
                return DefaultData(arg_data, false, fmt("{0}", def_val ? "true" : "false"));
            case godot::Variant::INT:
                return DefaultData(arg_data, false, fmt("{0}", def_val));
            case godot::Variant::FLOAT: {
                return DefaultData(arg_data, false, fmt("{0}", fp(def_val)));
            }
            case godot::Variant::STRING: {
                IS_DEF(String);
                return DefaultData(arg_data, _need_remap, fmt("\"{0}\"", def_val), true);
            }
            case godot::Variant::STRING_NAME: {
                IS_DEF(StringName);
                return DefaultData(
                    arg_data,
                    _need_remap,
                    fmt("new StringName(\"{0}\")", val),
                    true
                );
            }
            case godot::Variant::NODE_PATH: {
                IS_DEF(NodePath);
                return DefaultData(arg_data, _need_remap, fmt("new NodePath(\"{0}\")", val), true);
            }

            case godot::Variant::VECTOR2: {
                IS_DEF(Vector2);
                return DefaultData(
                    arg_data,
                    _need_remap,
                    fmt("new Vector2({0}, {1})", fp(val.x), fp(val.y))
                );
            }
            case godot::Variant::VECTOR2I: {
                IS_DEF(Vector2i);
                return DefaultData(
                    arg_data,
                    _need_remap,
                    fmt("new Vector2I({0}, {1})", val.x, val.y)
                );
            }
            case godot::Variant::RECT2: {
                IS_DEF(Rect2);
                return DefaultData(
                    arg_data,
                    _need_remap,
                    fmt("new Rect2({0}, {1}, {2}, {3})",
                        fp(val.position.x),
                        fp(val.position.y),
                        fp(val.size.x),
                        fp(val.size.y))
                );
            }
            case godot::Variant::RECT2I: {
                IS_DEF(Rect2i);
                return DefaultData(
                    arg_data,
                    _need_remap,
                    fmt("new Rect2I({0}, {1}, {2}, {3})",
                        val.position.x,
                        val.position.y,
                        val.size.x,
                        val.size.y)
                );
            }
            case godot::Variant::VECTOR3: {
                IS_DEF(Vector3);
                return DefaultData(
                    arg_data,
                    _need_remap,
                    fmt("new Vector3({0}, {1}, {2})", fp(val.x), fp(val.y), fp(val.z))
                );
            }
            case godot::Variant::VECTOR3I: {
                IS_DEF(Vector3i);
                return DefaultData(
                    arg_data,
                    _need_remap,
                    fmt("new Vector3I({0}, {1}, {2})", val.x, val.y, val.z)
                );
            }
            case godot::Variant::TRANSFORM2D: {
                IS_DEF(Transform2D);
                return DefaultData(
                    arg_data,
                    _need_remap,
                    fmt("new Transform2D(new Vector2({0}, {1}), new Vector2({2}, {3}), new "
                        "Vector2({4}, {5}))",
                        fp(val.columns[0].x),
                        fp(val.columns[0].y),
                        fp(val.columns[1].x),
                        fp(val.columns[1].y),
                        fp(val.columns[2].x),
                        fp(val.columns[2].y))
                );
            }
            case godot::Variant::VECTOR4: {
                IS_DEF(Vector4);
                return DefaultData(
                    arg_data,
                    _need_remap,
                    fmt("new Vector4({0}, {1}, {2}, {3})",
                        fp(val.x),
                        fp(val.y),
                        fp(val.z),
                        fp(val.w))
                );
            }
            case godot::Variant::VECTOR4I: {
                IS_DEF(Vector4i);
                return DefaultData(
                    arg_data,
                    _need_remap,
                    fmt("new Vector4I({0}, {1}, {2}, {3})", val.x, val.y, val.z, val.w)
                );
            }
            case godot::Variant::PLANE: {
                IS_DEF(Plane);
                return DefaultData(
                    arg_data,
                    _need_remap,
                    fmt("new Plane({0}, {1}, {2}, {3})",
                        fp(val.normal.x),
                        fp(val.normal.y),
                        fp(val.normal.z),
                        fp(val.d))
                );
            }
            case godot::Variant::QUATERNION: {
                IS_DEF(Quaternion);
                return DefaultData(
                    arg_data,
                    _need_remap,
                    fmt("new Quaternion({0}, {1}, {2}, {3})",
                        fp(val.x),
                        fp(val.y),
                        fp(val.z),
                        fp(val.w))
                );
            }
            case godot::Variant::AABB: {
                IS_DEF(AABB);
                return DefaultData(
                    arg_data,
                    _need_remap,
                    fmt("new Aabb(new Vector3({0}, {1}, {2}), new Vector3({3}, {4}, {5}))",
                        fp(val.position.x),
                        fp(val.position.y),
                        fp(val.position.z),
                        fp(val.size.x),
                        fp(val.size.y),
                        fp(val.size.z))
                );
            }
            case godot::Variant::BASIS: {
                IS_DEF(Basis);
                return DefaultData(
                    arg_data,
                    _need_remap,
                    fmt("new Basis(new Vector3({0}, {1}, {2}), new Vector3({3}, {4}, {5}), new "
                        "Vector3({6}, {7}, {8}))",
                        fp(val.rows[0].x),
                        fp(val.rows[1].x),
                        fp(val.rows[2].x),
                        fp(val.rows[0].y),
                        fp(val.rows[1].y),
                        fp(val.rows[2].y),
                        fp(val.rows[0].z),
                        fp(val.rows[1].z),
                        fp(val.rows[2].z))
                );
            }
            case godot::Variant::TRANSFORM3D: {
                IS_DEF(Transform3D);
                return DefaultData(
                    arg_data,
                    _need_remap,
                    fmt("new Transform3D(new Vector3({0}, {1}, {2}), new Vector3({3}, {4}, {5}), "
                        "new Vector3({6}, {7}, {8}), new Vector3({9}, {10}, {11}))",
                        fp(val.basis.rows[0].x),
                        fp(val.basis.rows[1].x),
                        fp(val.basis.rows[2].x),
                        fp(val.basis.rows[0].y),
                        fp(val.basis.rows[1].y),
                        fp(val.basis.rows[2].y),
                        fp(val.basis.rows[0].z),
                        fp(val.basis.rows[1].z),
                        fp(val.basis.rows[2].z),
                        fp(val.origin.x),
                        fp(val.origin.y),
                        fp(val.origin.z))
                );
            }
            case godot::Variant::PROJECTION: {
                IS_DEF(Projection);
                return DefaultData(
                    arg_data,
                    _need_remap,
                    fmt("new Projection(new Vector4({0}, {1}, {2}, {3}), new Vector4({4}, {5}, "
                        "{6}, {7}), new Vector4({8}, {9}, {10}, {11}), new Vector4({12}, {13}, "
                        "{14}, {15}))",
                        fp(val.columns[0].x),
                        fp(val.columns[0].y),
                        fp(val.columns[0].z),
                        fp(val.columns[0].w),
                        fp(val.columns[1].x),
                        fp(val.columns[1].y),
                        fp(val.columns[1].z),
                        fp(val.columns[1].w),
                        fp(val.columns[2].x),
                        fp(val.columns[2].y),
                        fp(val.columns[2].z),
                        fp(val.columns[2].w),
                        fp(val.columns[3].x),
                        fp(val.columns[3].y),
                        fp(val.columns[3].z),
                        fp(val.columns[3].w))
                );
            }
            case godot::Variant::COLOR: {
                IS_DEF(Color);
                return DefaultData(
                    arg_data,
                    _need_remap,
                    fmt("new Color({0}, {1}, {2}, {3})", fp(val.r), fp(val.g), fp(val.b), fp(val.a))
                );
            }
            case godot::Variant::RID: {
                IS_DEF(RID);
                if (_need_remap)
                    log_warning(fmt("'RID' can't have a default value: {0}", arg_data.name), 4);
                return DefaultData(arg_data, false, "default");
            }
            case godot::Variant::OBJECT: {
                if ((Object *)def_val != nullptr)
                    log_warning(fmt("'Object' can't have a default value: {0}", arg_data.name), 4);
                return DefaultData(arg_data, false, "null");
            }
            case godot::Variant::CALLABLE: {
                IS_DEF(Callable);
                if (_need_remap)
                    log_warning(
                        fmt("'Callable' can't have a default value: {0}", arg_data.name),
                        4
                    );
                return DefaultData(arg_data, false, "default");
            }
            case godot::Variant::SIGNAL: {
                IS_DEF(Signal);
                if (_need_remap)
                    log_warning(fmt("'Signal' can't have a default value: {0}", arg_data.name), 4);
                return DefaultData(arg_data, false, "default");
            }

#define PACKED_ARRAY(_type, _var_type)                                                          \
    {                                                                                           \
        _type val = def_val;                                                                    \
        if (!val.size()) {                                                                      \
            return DefaultData(arg_data, false, "default");                                     \
        } else {                                                                                \
            PackedStringArray strs;                                                             \
            for (int i = 0; i < val.size(); i++) {                                              \
                DefaultData data                                                                \
                    = arguments_get_formatted_value(argument_parse("", "", _var_type), val[i]); \
                strs.append(data.arg_string);                                                   \
            }                                                                                   \
            return DefaultData(                                                                 \
                arg_data,                                                                       \
                true,                                                                           \
                fmt("new {0} { {1} }", types_map[def_val.get_type()], String(", ").join(strs)), \
                true                                                                            \
            );                                                                                  \
        }                                                                                       \
    }

            case godot::Variant::DICTIONARY: {
                Dictionary val = def_val;
                if (!val.size()) {
                    return DefaultData(arg_data, false, "default");
                } else {
                    PackedStringArray strs;
                    Array keys = val.keys();
                    Array values = val.values();
                    for (int i = 0; i < val.size(); i++) {
                        DefaultData data_k = arguments_get_formatted_value(
                            argument_parse("", "", keys[i].get_type()),
                            keys[i]
                        );
                        DefaultData data_v = arguments_get_formatted_value(
                            argument_parse("", "", values[i].get_type()),
                            values[i]
                        );
                        strs.append(fmt("{ {0}, {1} }", data_k.arg_string, data_v.arg_string));
                    }
                    return DefaultData(
                        arg_data,
                        true,
                        String("new {0} { {1} }")
                            .format(
                                Array::make(types_map[def_val.get_type()], String(", ").join(strs))
                            ),
                        true
                    );
                }
            };
            case godot::Variant::ARRAY:
                PACKED_ARRAY(Array, val[i].get_type());
            case godot::Variant::PACKED_BYTE_ARRAY:
                PACKED_ARRAY(PackedByteArray, Variant::INT);
            case godot::Variant::PACKED_INT32_ARRAY:
                PACKED_ARRAY(PackedInt32Array, Variant::INT);
            case godot::Variant::PACKED_INT64_ARRAY:
                PACKED_ARRAY(PackedInt64Array, Variant::INT);
            case godot::Variant::PACKED_FLOAT32_ARRAY:
                PACKED_ARRAY(PackedFloat32Array, Variant::FLOAT);
            case godot::Variant::PACKED_FLOAT64_ARRAY:
                PACKED_ARRAY(PackedFloat64Array, Variant::FLOAT);
            case godot::Variant::PACKED_STRING_ARRAY:
                PACKED_ARRAY(PackedStringArray, Variant::STRING);
            case godot::Variant::PACKED_VECTOR2_ARRAY:
                PACKED_ARRAY(PackedVector2Array, Variant::VECTOR2);
            case godot::Variant::PACKED_VECTOR3_ARRAY:
                PACKED_ARRAY(PackedVector3Array, Variant::VECTOR3);
            case godot::Variant::PACKED_COLOR_ARRAY:
                PACKED_ARRAY(PackedColorArray, Variant::COLOR);
            default:
                break;
#undef PACKED_ARRAY
        }
#undef IS_DEF
    }

    return DefaultData("[no name]", "[no type]", false, "\"Error\"");
}

String GenerateCSharpBindingsPlugin::arguments_string_decl(
    const TypedArray<Dictionary> &args, bool with_defaults, std::vector<DefaultData> def_args_data
) {
    PackedStringArray arg_strs;
    for (int i = 0; i < args.size(); i++) {
        ArgumentData arg_data = argument_parse(args[i]);

        DefaultData *def_data = nullptr;
        for (auto &it : def_args_data) {
            if (it.name == arg_data.name) {
                def_data = &it;
                break;
            }
        }

        if (def_data) {
            if (def_data->is_need_remap) {
                String nullable_str = def_data->is_nullable ? "" : "?";
                arg_strs.append(
                    fmt("{0}{1} {2} = null", arg_data.type_name, nullable_str, arg_data.name)
                );
            } else {
                arg_strs.append(
                    fmt("{0} {1} = {2}", arg_data.type_name, arg_data.name, def_data->arg_string)
                );
            }
        } else {
            arg_strs.append(fmt("{0} {1}", arg_data.type_name, arg_data.name));
        }
    }

    return String(", ").join(arg_strs);
}

String GenerateCSharpBindingsPlugin::arguments_string_call(
    const TypedArray<Dictionary> &args, const std::vector<DefaultData> &def_remap
) {
    PackedStringArray arg_strs;
    for (int i = 0; i < args.size(); i++) {
        ArgumentData arg_data = argument_parse((Dictionary)args[i]);
        StringName name = ((Dictionary)args[i])["name"];

        auto _def_res
            = std::find_if(def_remap.begin(), def_remap.end(), [&name](const DefaultData &i) {
                  return i.name == name;
              });

        if (generate_for_classes.has(arg_data.type_name)) {
            arg_strs.append(fmt("{0}.Instance", name));
        } else if (arg_data.is_enum) {
            arg_strs.append(fmt("(long){0}", name));
        } else if (_def_res != def_remap.end() && (*_def_res).is_need_remap) {
            arg_strs.append(fmt("{0} ?? {1}", name, (*_def_res).arg_string));
        } else {
            arg_strs.append(name);
        }
    }

    return String(", ").join(arg_strs);
}

void GenerateCSharpBindingsPlugin::line(const String &str, int indent_override) {
    if (indent_override < 0) opened_file->store_string(indent);
    else opened_file->store_string(indent_template.repeat(indent_override));
    opened_file->store_string(str);
    opened_file->store_8('\n');
}

void GenerateCSharpBindingsPlugin::force_log(const String &str, const int &indent) {
    String val = String("  ").repeat(indent) + str;
    print(val);
    opened_log_file->store_string(val);
    opened_log_file->store_8('\n');
}

void GenerateCSharpBindingsPlugin::log(const String &str, const int &indent) {
    String val = String("  ").repeat(indent) + str;
    if (is_shift_pressed) print(val);
    opened_log_file->store_string(val);
    opened_log_file->store_8('\n');
}

void GenerateCSharpBindingsPlugin::log_warning(const String &str, const int &indent) {
    String s = String("  ").repeat(indent) + "WARNING: " + str;
    if (is_shift_pressed) print(s);
    opened_log_file->store_string(String("  ").repeat(indent) + str);
    opened_log_file->store_8('\n');
}

void GenerateCSharpBindingsPlugin::force_log_warning(const String &str, const int &indent) {
    String s = String("  ").repeat(indent) + "WARNING: " + str;
    print_warning(s);
    opened_log_file->store_string(String("  ").repeat(indent) + str);
    opened_log_file->store_8('\n');
}

GenerateCSharpBindingsPlugin::IndentGuard GenerateCSharpBindingsPlugin::tab() {
    return IndentGuard(this);
}