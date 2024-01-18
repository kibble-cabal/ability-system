#ifndef AS_ATTRIBUTEMAP_HPP
#define AS_ATTRIBUTEMAP_HPP

#include "attribute.hpp"
#include "core/io/resource.h"
#include "map"

class AttributeMap : public Resource {
	GDCLASS(AttributeMap, Resource);

private:
	std::map<Ref<Attribute>, float> attributes;

protected:
	static void _bind_methods() {
		ClassDB::bind_method(D_METHOD("add", "attribute"), &AttributeMap::add);
		ClassDB::bind_method(D_METHOD("remove", "attribute"), &AttributeMap::remove);
		ClassDB::bind_method(D_METHOD("has", "attribute"), &AttributeMap::has);
		ClassDB::bind_method(D_METHOD("try_get_value", "attribute"), &AttributeMap::try_get_value);
		ClassDB::bind_method(D_METHOD("get_value", "attribute"), &AttributeMap::get_value);
		ClassDB::bind_method(D_METHOD("try_set_value", "attribute", "value"), &AttributeMap::try_set_value);
		ClassDB::bind_method(D_METHOD("set_value", "attribute", "value"), &AttributeMap::set_value);

		ClassDB::bind_method(D_METHOD("_get_attribute_dict"), &AttributeMap::get_attribute_dict);
		ClassDB::bind_method(D_METHOD("_set_attribute_dict", "value"), &AttributeMap::set_attribute_dict);

		ADD_GROUP("Attribute map", "");
		ADD_PROPERTY(
				PropertyInfo(Variant::DICTIONARY, "attributes", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_NO_EDITOR),
				"_set_attribute_dict",
				"_get_attribute_dict");
	}

public:
	Dictionary get_attribute_dict() const {
		Dictionary dict = {};
		for (auto element : attributes)
			dict[element.first] = element.second;
		return dict;
	}

	void set_attribute_dict(Dictionary value) {
		for (int i = 0; i < value.size(); i++) {
			Ref<Attribute> key = value.get_key_at_index(i);
			attributes.insert(std::pair(key, value[key]));
		}
	}

	void add(Ref<Attribute> attribute) {
		if (!has(attribute)) {
			attributes.insert(std::pair(attribute, attribute->default_value));
		}
		emit_changed();
	}

	void remove(Ref<Attribute> attribute) {
		for (auto pair : attributes) {
			if (pair.first->get_attribute_name() == attribute->get_attribute_name()) {
				attributes.erase(pair.first);
				emit_changed();
				return;
			}
		}
	}

	bool has(Ref<Attribute> attribute) const {
		for (auto pair : attributes) {
			if (pair.first->get_attribute_name() == attribute->get_attribute_name())
				return true;
		}
		return false;
	}

	float try_get_value(Ref<Attribute> attribute) const {
		for (auto pair : attributes) {
			if (pair.first->get_attribute_name() == attribute->get_attribute_name()) {
				return pair.second;
			}
		}
		return attribute->default_value;
	}

	float get_value(Ref<Attribute> attribute) const {
		ERR_FAIL_COND_V_MSG(!has(attribute), 0.0, "Cannot get; map is missing attribute.");
		try_get_value(attribute);
	}

	void set_value(Ref<Attribute> attribute, float value) {
		ERR_FAIL_COND_MSG(!has(attribute), "Cannot set; map is missing attribute.");
		try_set_value(attribute, value);
	}

	void try_set_value(Ref<Attribute> attribute, float value) {
		for (auto pair : attributes) {
			if (pair.first->get_attribute_name() == attribute->get_attribute_name()) {
				attributes[pair.first] = value;
				emit_changed();
				break;
			}
		}
	}
};

#endif