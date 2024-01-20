@tool
extends HBoxContainer

signal remove_pressed
signal value_changed(value: float)


@export var attribute: Attribute:
	set(value):
		attribute = value
		update_children()

@onready var label := %Label as Label
@onready var spin_box := %SpinBox as SpinBox
@onready var color_rect := %ColorRect as ColorRect

@onready var range_label := %RangeLabel as Label


func _ready() -> void:
	update_children()


func get_text_color() -> Color:
	if attribute:
		return Color.WHITE if attribute.ui_color.get_luminance() < 0.5 else Color.BLACK
	return get_theme_color("font_color", "Label")


func update_children() -> void:
	if attribute and is_inside_tree():
		if color_rect:
			color_rect.color = attribute.ui_color
		if label:
			label.text = attribute.identifier
			label.add_theme_color_override("font_color", get_text_color())
		if spin_box:
			spin_box.rounded = false
			spin_box.max_value = attribute.max_value
			spin_box.min_value = attribute.min_value
			spin_box.step = (attribute.max_value - attribute.min_value) / 1000
			spin_box.custom_arrow_step = spin_box.step * 10
		if range_label:
			range_label.add_theme_color_override("font_color", get_text_color())
			range_label.text = "({0} - {1})".format([attribute.min_value, attribute.max_value])


func set_value(value: float) -> void:
	if attribute and is_inside_tree() and spin_box:
		spin_box.value = value
		update_children()


func _on_remove_button_pressed() -> void:
	remove_pressed.emit()


func _on_spin_box_value_changed(value: float) -> void:
	value_changed.emit(value)
