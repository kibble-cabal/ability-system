#ifndef AS_ATTRIBUTEMAP_HPP
#define AS_ATTRIBUTEMAP_HPP

#include "attribute.hpp"
#include "core/object/ref_counted.h"
#include "map"

struct AttributeHasher {
	static _FORCE_INLINE_ uint32_t hash(const Ref<Attribute> &attribute) {
		if (attribute.is_valid())
			return hash_one_uint64((uint64_t)attribute->get_identifier().hash());
		return hash_one_uint64((uint64_t)attribute.operator->());
	}
};

struct AttributeComparator {
	static bool compare(const Ref<Attribute> &lhs, const Ref<Attribute> &rhs) {
		if (lhs.is_valid() && rhs.is_valid())
			return lhs->get_identifier() == rhs->get_identifier();
		if (lhs.is_valid() || rhs.is_valid())
			return false;
		return true;
	}
};

struct AttributeMap {
public:
	HashMap<Ref<Attribute>, float, AttributeHasher, AttributeComparator> attributes;

	Dictionary get_attribute_dict() const {
		Dictionary dict = {};
		for (auto element : attributes)
			dict[element.key] = element.value;
		return dict;
	}

	void set_attribute_dict(Dictionary value) {
		for (int i = 0; i < value.size(); i++) {
			Ref<Attribute> key = value.get_key_at_index(i);
			attributes[key] = value[key];
		}
	}

	void add(Ref<Attribute> attribute) {
		if (!has(attribute)) {
			attributes[attribute] = attribute->default_value;
		}
	}

	void remove(Ref<Attribute> attribute) {
		if (has(attribute))
			attributes.erase(attribute);
	}

	bool has(Ref<Attribute> attribute) const {
		return attributes.has(attribute);
	}

	float try_get_value(Ref<Attribute> attribute) const {
		if (has(attribute))
			return attributes[attribute];
		return attribute->default_value;
	}

	float get_value(Ref<Attribute> attribute) const {
		ERR_FAIL_COND_V_MSG(!has(attribute), attribute->default_value, "Cannot get; map is missing attribute.");
		return try_get_value(attribute);
	}

	void set_value(Ref<Attribute> attribute, float value) {
		ERR_FAIL_COND_MSG(!has(attribute), "Cannot set; map is missing attribute.");
		return try_set_value(attribute, value);
	}

	void try_set_value(Ref<Attribute> attribute, float value) {
		if (has(attribute))
			attributes[attribute] = CLAMP(value, attribute->min_value, attribute->max_value);
	}
};

#endif