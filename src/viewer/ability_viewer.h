#ifndef AS_ABILITYVIEWER_HPP
#define AS_ABILITYVIEWER_HPP

#include "viewer_base.h"

class AbilityViewer : public AbilitySystemViewerBase {
	GDCLASS(AbilityViewer, AbilitySystemViewerBase);

public:
	/* Override other event listeners to avoid unnecessary processing */
	void _attributes_changed();
	void _tags_changed();
	void _events_changed();
	void _effects_changed();
	void _event_blocked();

	void _draw() override;

	Vector2 _get_minimum_size() const override;
};

#endif