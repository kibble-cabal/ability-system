#ifndef AS_ATTRIBUTEEFFECT_HPP
#define AS_ATTRIBUTEEFFECT_HPP

#include "../attribute.hpp"
#include "effect.h"

class AttributeEffect: public Effect {
    GDCLASS(AttributeEffect, Effect);

private:
    Ref<Attribute> attribute;
    float min_effect = 0.0;
    float max_effect = 1.0;

protected:
    static void _bind_methods();

public:
    GETSET_RESOURCE(Ref<Attribute>, attribute)
    GETSET_RESOURCE(float, min_effect)
    GETSET_RESOURCE(float, max_effect)

    virtual int _tick(AbilitySystem *owner, float delta) override;
};

#endif