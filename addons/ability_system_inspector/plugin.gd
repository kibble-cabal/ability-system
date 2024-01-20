@tool
extends EditorPlugin

const AttributeInspectorPlugin := preload("attribute_inspector_plugin.gd")

var plugin: AttributeInspectorPlugin


func _enter_tree() -> void:
	plugin = AttributeInspectorPlugin.new()
	add_inspector_plugin(plugin)


func _exit_tree() -> void:
	remove_inspector_plugin(plugin)
