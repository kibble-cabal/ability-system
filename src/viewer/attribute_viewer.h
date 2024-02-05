#ifndef AS_ATTRIBUTEVIEWER_HPP
#define AS_ATTRIBUTEVIEWER_HPP

#include "viewer_base.h"

class AttributeViewer: public AbilitySystemViewerBase {
    GDCLASS(AttributeViewer, AbilitySystemViewerBase);

public:
    /* Override other event listeners to avoid unnecessary processing */
    void _tags_changed();
    void _abilities_changed();
    void _events_changed();
    void _effects_changed();
    void _event_blocked();

    void _draw() override;

    Vector2 _get_minimum_size() const override;
};

#endif