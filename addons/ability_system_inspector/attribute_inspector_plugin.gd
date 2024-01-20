extends EditorInspectorPlugin

const AttributeEditor = preload("attribute_editor/property.gd")


func _can_handle(object):
	return object is AbilitySystem


func _parse_property(object: Object, type: Variant.Type, name: String, hint_type: PropertyHint, hint_string: String, usage_flags, wide):
	# We handle properties of type integer.
	if type == TYPE_DICTIONARY and name =="attributes":
		# Create an instance of the custom property editor and register
		# it to a specific property path.
		add_property_editor(name, AttributeEditor.new())
		# Inform the editor to remove the default property editor for
		# this property type.
		return true
	else:
		return false
