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

    generate_header();

    line("using Godot;");
    line("using System.Linq;");
    line("using System.Collections.Generic;");
    line();
    line("#nullable enable");

    force_log("Generation of bindings started, output file: " + out_path);
    force_log("Log file: " + out_log_path);

    line();
    line("namespace AS");
    {
        TAB();

        generate_class_names();
        line();

        remap_data remapped_data;
        for (int i = 0; i < generate_for_classes.size(); i++) {
            generate_class(generate_for_classes[i], remapped_data);
        }
    }

    line();

    force_log("The generation process is completed!");
}

void GenerateCSharpBindingsPlugin::generate_class_names() {
    line("public static class ClassNames");
    {
        TAB();
        for_each(generate_for_classes, [&](StringName cls) {
            line(fmt("public static readonly StringName {0} = \"{0}\";", cls));
        });
    }
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

    StringName base_cls = get_native_base_class(cls);
    StringName parent_name = ClassDB::get_parent_class(cls);
    PackedStringArray enum_names = ClassDB::class_get_enum_list(cls, true);
    TypedArray<Dictionary> methods = ClassDB::class_get_method_list(cls, true);
    TypedArray<Dictionary> signals = ClassDB::class_get_signal_list(cls, true);
    TypedArray<Dictionary> properties = ClassDB::class_get_property_list(cls, true);
    std::map<String, ArgumentData> properties_map;

    bool is_preserved_inheritance = generate_for_classes.has(parent_name);

    line();
    String additional_inheritance = "";

    if (is_preserved_inheritance) {
        line(fmt("public partial class {0} : {1}{2}", cls, parent_name, additional_inheritance));
    } else {
        line(
            fmt("public partial class {0}{1}{2}",
                cls,
                fmt(" : IInstanceWrapper<{0}>", base_cls),
                additional_inheritance)
        );
    }

    {
        TAB();

        // Wrapper
        {
            log("Wrapper...", 2);
            generate_wrapper(cls, is_preserved_inheritance);
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
            if (is_preserved_inheritance) line("public static new class Method");
            else line("public static class Method");
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

            if (added) line();

            for (int i = 0; i < methods.size(); i++) {
                String name = ((Dictionary)methods[i])["name"];

                if (name.begins_with("_")) {
                    continue;
                }

                if (!is_property.has(name)) generate_method(cls, methods[i], remapped_data);
            }
        }

        // Properties
        {
            log("Properties...", 2);
            generate_properties(cls, properties, properties_map);
        }
    }
}

void GenerateCSharpBindingsPlugin::generate_wrapper(const StringName &cls, bool inheritance) {
    StringName base_cls = get_native_base_class(cls);
    bool is_node = ClassDB::is_parent_class(cls, "Node");
    bool is_resource = ClassDB::is_parent_class(cls, "Resource");
    if (!generate_for_classes.has(ClassDB::get_parent_class(cls))) {
        if (is_node || is_resource) line("[Export]");
        line(fmt("public {0} Instance { get; set; }", base_cls));
        line();
        line(
            fmt("public {0}({1} instance) => (this as "
                "IInstanceWrapper<{1}>).SetInstance(instance);",
                cls,
                base_cls)
        );
    } else {
        line(fmt("public {0}({1} instance) : base (instance) {}", cls, base_cls));
    }
    line();
    line(
        fmt("public static implicit operator {0}({1}? instance) => instance is null ? new() : "
            "new(instance);",
            cls,
            base_cls)
    );
    line(fmt("public static implicit operator {1}({0} obj) => obj.Instance;", cls, base_cls));
    line();
    line(fmt(
        "public static implicit operator Variant({0}? obj) => obj is null ? new() : obj.Instance;",
        cls
    ));
    line();
    line(fmt("public {0}() : this(({1})ClassDB.Instantiate(ClassNames.{0})) { }", cls, base_cls));
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
    if (is_inherited(cls)) line("public static new class Enum");
    else line("public static class Enum");
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
    const StringName &cls, const Dictionary &method, remap_data &remapped_data
) {
    String name = (String)method["name"];

    log(name, 3);

    Dictionary return_dict = method["return"];
    ArgumentData return_data = argument_parse(return_dict, true);
    bool is_need_wrapper = generate_for_classes.has(return_data.type_name);

    std::vector<DefaultData> default_args
        = arguments_parse_values(method["args"], method["default_args"], remapped_data);

    String method_body = "";

    String method_signature = fmt(
        "public {0} {1}({2})",
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
                "Utils.CreateWrapperFromObject<{0}>((GodotObject)"
                "Instance?."
                "Call(Method.{1}{2}));",
                return_data.type_name,
                pascal_case(name),
                call_args
            );
        } else {
            auto return_type_name = return_data.type_name.replace("?", "");
            if (generate_for_classes.has(return_type_name))
                method_body = fmt(
                    "Instance?.Call(Method.{0}{1}).TryAs<{2}>();",
                    pascal_case(name),
                    call_args,
                    get_native_base_class(return_type_name)
                );
            else
                method_body = fmt(
                    "{0}Instance?.Call(Method.{1}{2}).As<{3}>() ?? default;",
                    int_convert,
                    pascal_case(name),
                    call_args,
                    return_data.type_name
                );
        }
    } else {
        method_body = fmt("Instance?.Call(Method.{0}{1});", pascal_case(name), call_args);
    }

    line(fmt("{0} => {1}", method_signature, method_body));

    line();
}

void GenerateCSharpBindingsPlugin::generate_properties(
    const StringName &cls,
    const TypedArray<Dictionary> &props,
    std::map<String, ArgumentData> setget_map
) {
    // ClassDB SetGet is faster than calling set/get methods
    // Iterations: 100000
    // Methods Get: 50,967 ms
    // Methods Set: 60,477 ms
    // ClassDB_SetGet Get: 28,660 ms
    // ClassDB_SetGet Set: 26,918 ms

    if (is_inherited(cls)) line("public static new class Property");
    else line("public static class Property");
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

        log(setget.name, 3);

        String type_name = is_need_wrapper ? setget.type_name + "?" : setget.type_name;

        line(fmt("public {0} {1}", type_name, pascal_case(setget.name)));
        {
            TAB();
            if (is_need_wrapper) {
                line(
                    fmt("get => "
                        "Utils.CreateWrapperFromObject<{0}>((GodotObject)ClassDB."
                        "ClassGetProperty(Instance, Property.{1}));",
                        setget.type_name,
                        pascal_case(setget.name))
                );
                line(
                    fmt("set => ClassDB.ClassSetProperty(Instance, Property.{0}, "
                        "Variant.From(value?.Instance));",
                        pascal_case(setget.name))
                );
            } else if (setget.is_enum) {
                line(
                    fmt("get => ({0})(long)ClassDB.ClassGetProperty(Instance, Property.{1});",
                        setget.type_name,
                        pascal_case(setget.name))
                );
                line(
                    fmt("set => ClassDB.ClassSetProperty(Instance, Property.{0}, (long)value);",
                        pascal_case(setget.name))
                );
            } else {
                if (generate_for_classes.has(setget.hint_string)) {
                    auto base_cls = get_native_base_class(setget.hint_string);
                    if (setget.hint == PROPERTY_HINT_ARRAY_TYPE) {
                        // Handle typed array with generated class
                        // e.g. `Array<Tag>`
                        auto prop_name = pascal_case(setget.name);
                        line(
                            fmt("get => ClassDB.ClassGetProperty(Instance, "
                                "Property.{0}).Convert<{1}, {2}>().ToList();",
                                prop_name,
                                base_cls,
                                setget.hint_string)
                        );
                        line(
                            fmt("set => ClassDB.ClassSetProperty(Instance, Property.{0}, "
                                "new Godot.Collections.Array<{2}>(value.Convert<{1}, {2}>()));",
                                prop_name,
                                setget.hint_string,
                                base_cls)
                        );
                    } else {
                        // Handle generated class, e.g. `Tag`
                        line(fmt(
                            "get => ClassDB.ClassGetProperty(Instance, Property.{0}).As<{1}?>();",
                            pascal_case(setget.name),
                            base_cls
                        ));
                        line(
                            fmt("set => ClassDB.ClassSetProperty(Instance, Property.{0}, "
                                "Variant.From(value));",
                                pascal_case(setget.name))
                        );
                    }
                } else {
                    line(
                        fmt("get => ClassDB.ClassGetProperty(Instance, Property.{0}).As<{1}>();",
                            pascal_case(setget.name),
                            setget.type_name)
                    );
                    line(
                        fmt("set => ClassDB.ClassSetProperty(Instance, Property.{0}, "
                            "value);",
                            pascal_case(setget.name))
                    );
                }
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

        else if (generate_for_classes.has(class_name))
            return ArgumentData(name, var_type, fmt("{0}?", class_name), hint, hint_string);

        return ArgumentData(name, var_type, class_name, hint, hint_string);
    } else if (var_type) {
        // Typed Array
        if (var_type == Variant::Type::ARRAY && hint == PROPERTY_HINT_ARRAY_TYPE) {
            return ArgumentData(
                name,
                var_type,
                fmt("{0}<{1}>", "List", hint_string),
                hint,
                hint_string
            );
        } else {
            return ArgumentData(name, var_type, types_map[var_type], hint, hint_string);
        }
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
        res.push_back(dd);
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
            arg_data.hint,
            arg_data.hint_string,
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
                        arg_data.hint,
                        arg_data.hint_string,
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
                        fmt("new {0} { {1} }",
                            types_map[def_val.get_type()],
                            String(", ").join(strs)),
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

    return DefaultData(
        "[no name]",
        "[no type]",
        PropertyHint::PROPERTY_HINT_NONE,
        "[no hint string]",
        false,
        "\"Error\""
    );
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

        if (generate_for_classes.has(arg_data.hint_string)
            && arg_data.type == Variant::Type::ARRAY) {
            arg_strs.append(
                fmt("new Godot.Collections.Array<Resource>({0}.Convert<{1}, {2}>())",
                    name,
                    arg_data.hint_string,
                    get_native_base_class(arg_data.hint_string))
            );
        } else if (arg_data.is_enum) {
            arg_strs.append(fmt("(long){0}", name));
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
    print(log(str, indent));
}

String GenerateCSharpBindingsPlugin::log(const String &str, const int &indent) {
    String val = String("  ").repeat(indent) + str;
    opened_log_file->store_string(val);
    opened_log_file->store_8('\n');
    return val;
}

String GenerateCSharpBindingsPlugin::log_warning(const String &str, const int &indent) {
    return log("WARNING: " + str);
}

GenerateCSharpBindingsPlugin::IndentGuard GenerateCSharpBindingsPlugin::tab() {
    return IndentGuard(this);
}