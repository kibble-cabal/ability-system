#ifndef AS_MACROS_HPP
#define AS_MACROS_HPP

/***********************************
 *** _bind_methods helper macros ***
 ***********************************/

#define BIND_GETSET(cls, prop_name)                                           \
	ClassDB::bind_method(D_METHOD("get_" #prop_name), &cls::get_##prop_name); \
	ClassDB::bind_method(D_METHOD("set_" #prop_name, #prop_name), &cls::set_##prop_name);

#define GROUP(name) ADD_GROUP(name, "");

#define PROP(variant_ty, prop_name) \
	ClassDB::add_property(get_class_static(), PropertyInfo(variant_ty, #prop_name), "set_" #prop_name, "get_" #prop_name);

#define OBJECT_PROP(obj_cls, prop_name) \
	ClassDB::add_property(get_class_static(), PropertyInfo(Variant::OBJECT, #prop_name, PROPERTY_HINT_RESOURCE_TYPE, #obj_cls), "set_" #prop_name, "get_" #prop_name);

#define ARRAY_PROP(prop_name, type_hint) \
	ADD_PROPERTY(PropertyInfo(Variant::ARRAY, #prop_name, PROPERTY_HINT_ARRAY_TYPE, type_hint), "set_" #prop_name, "get_" #prop_name);

#define NO_EDITOR_PROP(variant_ty, prop_name) \
	ClassDB::add_property(get_class_static(), PropertyInfo(variant_ty, #prop_name, PROPERTY_HINT_NONE, "", PROPERTY_USAGE_NO_EDITOR), "set_" #prop_name, "get_" #prop_name);

#define OBJECT_PROP_INFO(obj_cls, prop_name) \
	PropertyInfo(Variant::OBJECT, #prop_name, PROPERTY_HINT_RESOURCE_TYPE, #obj_cls)

#define RESOURCE_TYPE_HINT(cls_name_string) vformat("%s/%s:%s", Variant::OBJECT, PROPERTY_HINT_RESOURCE_TYPE, cls_name_string)

/***********************************
 *** setter/getter helper macros ***
 ***********************************/

#define GETTER(ty, prop_name)    \
	ty get_##prop_name() const { \
		return prop_name;        \
	}

#define SETTER(ty, prop_name)            \
	void set_##prop_name(ty prop_name) { \
		this->prop_name = prop_name;     \
	}

#define SETTER_RESOURCE(ty, prop_name)   \
	void set_##prop_name(ty prop_name) { \
		this->prop_name = prop_name;     \
		emit_changed();                  \
	}

#define GETSET(ty, prop_name) \
	GETTER(ty, prop_name)     \
	SETTER(ty, prop_name)

#define GETSET_RESOURCE(ty, prop_name) \
	GETTER(ty, prop_name)              \
	SETTER_RESOURCE(ty, prop_name)

#define stringify(...) UtilityFunctions::str(__VA_ARGS__)
#define print_error(arg) UtilityFunctions::printerr(arg)
#define RANDF_RANGE(min, max) UtilityFunctions::randf_range(min, max)

#define fmt(string, args...) String(string).format(variant_array(args))

#endif