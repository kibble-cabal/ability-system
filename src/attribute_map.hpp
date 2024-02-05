#ifndef AS_ATTRIBUTEMAP_HPP
#define AS_ATTRIBUTEMAP_HPP

#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/templates/hash_map.hpp>
#include <godot_cpp/templates/hashfuncs.hpp>

#include "attribute.hpp"
#include "map"

using namespace godot;

struct AttributeHasher {
    static _FORCE_INLINE_ uint32_t hash(const Ref<Attribute> &attribute) {
        if (attribute.is_valid())
            return hash_one_uint64((uint64_t)attribute->get_identifier().hash()
            );
        return hash_one_uint64((uint64_t)attribute.operator->());
    }
};

struct AttributeComparator {
    static bool compare(const Ref<Attribute> &lhs, const Ref<Attribute> &rhs) {
        if (lhs.is_valid() && rhs.is_valid())
            return lhs->get_identifier() == rhs->get_identifier();
        if (lhs.is_valid() || rhs.is_valid()) return false;
        return true;
    }
};

template <typename T>
using AttributeHashMap
    = HashMap<Ref<Attribute>, T, AttributeHasher, AttributeComparator>;

class AttributeMap {
public:
    AttributeHashMap<float> attributes;

    Dictionary get_attribute_dict() const {
        Dictionary dict = {};
        for (auto element : attributes) dict[element.key] = element.value;
        return dict;
    }

    void set_attribute_dict(Dictionary value) {
        attributes.clear();
        Array keys = value.keys();
        for (int i = 0; i < keys.size(); i++) {
            Ref<Attribute> key = keys[i];
            attributes[key] = value[key];
        }
    }

    void add(Ref<Attribute> attribute) {
        if (!has(attribute)) {
            attributes[attribute] = attribute->default_value;
        }
    }

    void remove(Ref<Attribute> attribute) {
        if (has(attribute)) attributes.erase(attribute);
    }

    bool has(Ref<Attribute> attribute) const {
        return attributes.has(attribute);
    }

    float try_get_value(Ref<Attribute> attribute) const {
        if (has(attribute)) return attributes[attribute];
        return attribute->default_value;
    }

    float get_value(Ref<Attribute> attribute) const {
        ERR_FAIL_COND_V_MSG(
            !has(attribute),
            attribute->default_value,
            "Cannot get; map is missing attribute."
        );
        return try_get_value(attribute);
    }

    /* Returns attribute's value normalized between 0 and 1. */
    float get_percent(Ref<Attribute> attribute) const {
        float value = get_value(attribute);
        return (value - attribute->min_value)
               / (attribute->max_value - attribute->min_value);
    }

    void set_value(Ref<Attribute> attribute, float value) {
        ERR_FAIL_COND_MSG(
            !has(attribute),
            "Cannot set; map is missing attribute."
        );
        return try_set_value(attribute, value);
    }

    void try_set_value(Ref<Attribute> attribute, float value) {
        if (has(attribute))
            attributes[attribute]
                = CLAMP(value, attribute->min_value, attribute->max_value);
    }
};

#endif