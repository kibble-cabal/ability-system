@tool
extends EditorProperty

const Scene := preload("editor_scene.tscn")

var editor: Control

var current_value: Dictionary:
	get: return get_edited_object()[get_edited_property()]


func _init():
	editor = Scene.instantiate()
	add_child(editor)
	add_focusable(editor)
	set_bottom_editor(editor)


func _ready() -> void:
	editor.add.connect(add_attributes)
	editor.remove.connect(remove_attribute)
	editor.change_value.connect(change_value)


func add_attributes(attributes: Array[Attribute]) -> void:
	var new_value := current_value.duplicate(true)
	for attr in attributes:
		new_value[attr] = attr.default_value
	emit_changed(get_edited_property(), new_value)


func remove_attribute(attribute: Attribute) -> void:
	var new_value := current_value.duplicate(true)
	new_value.erase(attribute)
	emit_changed(get_edited_property(), new_value)


func change_value(attribute: Attribute, value: float) -> void:
	var new_value := current_value.duplicate(true)
	new_value[attribute] = value
	emit_changed(get_edited_property(), new_value)


func _update_property():
	if editor: editor.update(current_value)
