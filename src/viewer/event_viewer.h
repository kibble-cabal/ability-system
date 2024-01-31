#ifndef AS_EVENTVIEWER_H
#define AS_EVENTVIEWER_H

#include "viewer_base.h"

struct RenderContainer;

class EventViewer : public AbilitySystemViewerBase {
	GDCLASS(EventViewer, AbilitySystemViewerBase);

private:
	RenderContainer make_container_const(float y_offset = 0) const;
	RenderContainer make_container(float y_offset = 0);
	void add_header(RenderContainer *container, Ref<AbilityEvent> event) const;
	void add_effects(RenderContainer *container, Ref<AbilityEvent> event) const;

public:
	/* Override other event listeners to avoid unnecessary processing */
	void _attributes_changed();
	void _tags_changed();
	void _abilities_changed();

	void _draw() override;

	Vector2 _get_minimum_size() const override;
};

#endif