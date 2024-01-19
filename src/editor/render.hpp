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

struct Drawable {
public:
	CanvasItem *canvas;
	Rect2 rect;

	Drawable() {}
	virtual ~Drawable() {}
	virtual void draw() {}
};

enum LabelStyle {
	filled,
	outlined,
	text
};

struct RenderLabel : Drawable {
public:
	RenderLabel(String string, Color color) {
		this->string = string;
		this->color = color;
		this->rect.size = string_size(string);
	}

	String string;
	Color color = Color(1.0, 1.0, 1.0);
	Vector2 padding = Vector2(2, 1);
	LabelStyle style = LabelStyle::text;

	Rect2 bbox() const {
		return Rect2(rect.position - padding, string_size(string) + padding * 2);
	}

	Vector2 string_pos() const {
		return rect.position + Vector2(0, font_size());
	}

	void draw_filled() const {
		canvas->draw_rect(bbox(), color);
		canvas->draw_string(font(), string_pos(), string, HORIZONTAL_ALIGNMENT_CENTER, -1, font_size(), contrast(color));
	}

	void draw_text() const {
		canvas->draw_string(font(), string_pos(), string, HORIZONTAL_ALIGNMENT_CENTER, -1, font_size(), color);
	}

	void draw_outlined() const {
		canvas->draw_rect(bbox(), Color(color, 0.1));
		canvas->draw_rect(bbox(), Color(color, 0.5), false, 1);
		canvas->draw_string(font(), string_pos(), string, HORIZONTAL_ALIGNMENT_CENTER, -1, font_size(), color);
	}

	virtual void draw() override {
		switch (style) {
			case LabelStyle::filled:
				draw_filled();
				break;
			case LabelStyle::outlined:
				draw_outlined();
				break;
			case LabelStyle::text:
				draw_text();
				break;
		}
	}
};

class RenderProgress : public Drawable {
public:
	Color color;
	float value;

	void draw_progress() {
		canvas->draw_rect(rect, Color(color, 0.15));
		canvas->draw_rect(Rect2(rect.position, rect.size * Vector2(value, 1)), color);
	}

	virtual void draw() override {
		draw_progress();
	}
};

// Gets positions of items. Works similar to `HFlowContainer`.
struct RenderContainer : Drawable {
protected:
	Vector2 offset;
	std::vector<RenderLabel> labels;
	std::vector<RenderProgress> progresses;

	template <typename T = Drawable>
	T add_item_size(T item) {
		if (item.rect.size.x > remaining_width())
			new_line();
		item.rect.position = rect.position + offset;
		offset.x += item.rect.size.x + margin.x;
		return item;
	}

public:
	Vector2 margin = Vector2(3, 3);
	Vector2 max_size;
	LabelStyle style = LabelStyle::outlined;

	RenderProgress *add(RenderProgress progress) {
		progresses.push_back(add_item_size(progress));
		return &progresses[progresses.size() - 1];
	}

	RenderLabel *add(RenderLabel label) {
		labels.push_back(add_item_size(label));
		return &labels[labels.size() - 1];
	}

	void new_line() {
		offset.x = 0;
		offset.y = total_rect().size.y + margin.y;
	}

	float remaining_width() {
		return max_size.x - offset.x - margin.x;
	}

	Rect2 total_rect() const {
		Rect2 result;
		for (auto label : labels)
			result = result.merge(label.rect);
		for (auto progress : progresses)
			result = result.merge(progress.rect);
		return result;
	}

	Vector2 total_size() {
		return total_rect().size;
	}

	virtual void draw() override {
		for (auto label : labels)
			label.draw();
		for (auto progress : progresses)
			progress.draw();
	}

	void draw_background(Color color) {
		canvas->draw_rect(total_rect(), color);
	}

	void draw_outline(Color color) {
		canvas->draw_rect(total_rect().grow(2), color, false, 1);
	}

	RenderLabel *add_label(String string, Color color = Color(1, 1, 1)) {
		RenderLabel label(string, color);
		label.canvas = canvas;
		label.style = style;
		return add(label);
	}

	std::pair<RenderLabel *, RenderProgress *> add_progress(String name, float value = 0.0, Color color = Color(1, 1, 1)) {
		String string = name + " " + stringify_variants(value * 100).left(5) + "%";
		RenderLabel *label = add_label(string, color);
		RenderProgress progress;
		progress.rect.size = Vector2(remaining_width(), font_size());
		progress.color = color;
		progress.value = value;
		progress.canvas = canvas;
		return std::pair(label, add(progress));
	}
};

#endif