#ifndef AS_RENDER_HPP
#define AS_RENDER_HPP

#include "map"
#include "scene/main/canvas_item.h"
#include "scene/theme/theme_db.h"

Ref<Font> font() {
	return ThemeDB::get_singleton()->get_fallback_font();
}

int font_size() {
	return ThemeDB::get_singleton()->get_fallback_font_size();
}

Vector2 string_size(String text) {
	return font()->get_string_size(text, HORIZONTAL_ALIGNMENT_CENTER, -1, font_size());
}

Vector2 string_pos() {
	return Vector2(0, font_size());
}

Color contrast(Color other) {
	if (other.get_luminance() > 0.5)
		return Color(0, 0, 0);
	return Color(1, 1, 1);
}

struct Render {
	CanvasItem *canvas;
};

struct RenderLabel : Render {
	String string;
	Color color = Color(1.0, 1.0, 1.0);
	Vector2 padding = Vector2(2, 1);
	Vector2 pos;

	Rect2 rect() const {
		return Rect2(pos - padding, string_size(string) + padding * 2);
	}

	Vector2 string_pos() const {
		return pos + Vector2(0, font_size());
	}

	void draw_filled() const {
		canvas->draw_rect(rect(), color);
		canvas->draw_string(font(), string_pos(), string, HORIZONTAL_ALIGNMENT_CENTER, -1, font_size(), contrast(color));
	}

	void draw_text() const {
		canvas->draw_string(font(), string_pos(), string, HORIZONTAL_ALIGNMENT_CENTER, -1, font_size(), color);
	}

	void draw_outlined() const {
		canvas->draw_rect(rect(), Color(color, 0.1));
		canvas->draw_rect(rect(), Color(color, 0.5), false, 1);
		canvas->draw_string(font(), string_pos(), string, HORIZONTAL_ALIGNMENT_CENTER, -1, font_size(), color);
	}
};

// Gets positions of items. Works similar to `HFlowContainer`.
struct RenderContainer : Render {
protected:
	Vector2 offset;
	Rect2 total_rect;

public:
	Vector2 margin = Vector2(3, 3);
	Vector2 max_size;

	Vector2 next(Vector2 item_size) {
		if (item_size.x > remaining_width()) {
			offset.x = 0;
			offset.y += item_size.y + margin.y;
		}
		Vector2 current_offset = offset;
		offset.x += item_size.x + margin.x;
		total_rect = total_rect.merge(Rect2(current_offset, item_size));
		return current_offset;
	}

	float remaining_width() {
		return max_size.x - offset.x - margin.x;
	}

	Vector2 total_size() {
		return total_rect.size;
	}
};

struct RenderLabelContainer : RenderContainer {
public:
	RenderLabel add(String string, Color color = Color(1, 1, 1)) {
		RenderLabel label;
		label.canvas = canvas;
		label.string = string;
		label.color = color;
		label.pos = next(label.rect().size);
		return label;
	}

	void draw_label_filled(String string, Color color) {
		add(string, color).draw_filled();
	}

	void draw_label_outlined(String string, Color color) {
		add(string, color).draw_outlined();
	}

	void draw_label_text(String string, Color color) {
		add(string, color).draw_text();
	}
};

struct RenderProgress : Render {
	Color color;
	Vector2 pos;
	Vector2 size;
	float value;

	void draw() {
		canvas->draw_rect(Rect2(pos, size), Color(color, 0.15));
		canvas->draw_rect(Rect2(pos, size * Vector2(value, 1)), color);
	}
};

struct RenderProgressContainer : RenderLabelContainer {
	std::pair<RenderLabel, RenderProgress> add_progress(String name, float value = 0.0, Color color = Color(1, 1, 1)) {
		String string = name + " " + stringify_variants(value * 100).left(5) + "%";
		RenderLabel label = add(string, color);
		RenderProgress progress;
		progress.size = Vector2(remaining_width(), font_size());
		progress.pos = next(progress.size);
		progress.color = color;
		progress.value = value;
		progress.canvas = canvas;
		return std::pair(label, progress);
	}

	void draw_progress(String name, float value, Color color) {
		auto elements = add_progress(name, value, color);
		elements.first.draw_text();
		elements.second.draw();
	}
};

#endif