@tool
extends VBoxContainer

signal add(attributes: Array[Attribute])
signal remove(attribute: Attribute)
signal change_value(attribute: Attribute, value: float)

const Row := preload("editor_row_scene.tscn")

@onready var file_dialog := %FileDialog as FileDialog

var row_nodes := {}


func update(new_value: Dictionary):
	# Re-render all attributes
	for attribute in new_value.keys():
		_render_attribute(attribute, new_value[attribute])
	
	# Remove attributes that are not in the new value
	for attribute in row_nodes.keys():
		if not attribute in new_value:
			row_nodes[attribute].queue_free()
			row_nodes.erase(attribute)


func _on_add_button_pressed() -> void:	
	if file_dialog:
		file_dialog.show()


func _on_file_dialog_files_selected(paths: PackedStringArray) -> void:
	var add_attributes: Array[Attribute] = []
	for path in paths:
		var attribute := ResourceLoader.load(path)
		if attribute and attribute is Attribute:
			add_attributes.append(attribute)
	add.emit(add_attributes)


func _on_attribute_value_changed(value: float, attribute: Attribute) -> void:
	change_value.emit(attribute, value)


func _on_attribute_remove_pressed(attribute: Attribute) -> void:
	remove.emit(attribute)


func _render_attribute(attribute: Attribute, value: float) -> void:
	if attribute in row_nodes:
		var row = row_nodes[attribute]
		row.set_value(value)
	else:
		var new_row := Row.instantiate()
		new_row.attribute = attribute
		new_row.value_changed.connect(_on_attribute_value_changed.bind(attribute))
		new_row.remove_pressed.connect(_on_attribute_remove_pressed.bind(attribute))
		row_nodes[attribute] = new_row
		add_child(new_row)


func _on_file_dialog_file_selected(path: String) -> void:
	_on_file_dialog_files_selected(PackedStringArray([path]))
