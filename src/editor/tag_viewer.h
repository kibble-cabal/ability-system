#ifndef AS_TAGVIEWER_HPP
#define AS_TAGVIEWER_HPP

#include "viewer_base.h"

class TagViewer : public AbilitySystemViewerBase {
	GDCLASS(TagViewer, AbilitySystemViewerBase);

public:
	/* Override other event listeners to avoid unnecessary processing */
	void _attributes_changed();
	void _abilities_changed();
	void _events_changed();
	void _effects_changed();
	void _event_blocked();

	virtual void _draw() override;

	#ifdef ABILITY_SYSTEM_MODULE
	Vector2 get_minimum_size() const override;
	#endif
};

#endif