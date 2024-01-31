#ifndef AS_ABILITYSYSTEMVIEWER_HPP
#define AS_ABILITYSYSTEMVIEWER_HPP

#include <godot_cpp/classes/label.hpp>
#include <godot_cpp/classes/v_box_container.hpp>
#include "../ability_system.h"
#include "ability_viewer.h"
#include "attribute_viewer.h"
#include "event_viewer.h"
#include "tag_viewer.h"

using namespace godot;

class AbilitySystemViewer : public VBoxContainer {
	GDCLASS(AbilitySystemViewer, VBoxContainer);

private:
	NodePath ability_system_path;

    Label *ability_label = memnew(Label);
    Label *attribute_label = memnew(Label);
    Label *event_label = memnew(Label);
    Label *tag_label = memnew(Label);

    AbilityViewer *ability_viewer = memnew(AbilityViewer);
    AttributeViewer *attribute_viewer = memnew(AttributeViewer);
    EventViewer *event_viewer = memnew(EventViewer);
    TagViewer *tag_viewer = memnew(TagViewer);

protected:
    static void _bind_methods() {
        BIND_GETSET(AbilitySystemViewer, ability_system_path);
        PROP(Variant::NODE_PATH, ability_system_path);
    }

public:
    AbilitySystemViewer() {
        // Abilities
        ability_label->set_text("Abilities");
        add_child(ability_label);
        add_child(ability_viewer);
        // Tags
        tag_label->set_text("Tags");
        add_child(tag_label);
        add_child(tag_viewer);
        // Attributes
        attribute_label->set_text("Attributes");
        add_child(attribute_label);
        add_child(attribute_viewer);
        // Events
        event_label->set_text("Events");
        add_child(event_label);
        add_child(event_viewer);
    }

	void _notification(int what) {
        switch (what) {
            case NOTIFICATION_READY:
                update_children_ability_system_paths();
                break;
        }
    }

    GETTER(NodePath, ability_system_path)

    void set_ability_system_path(NodePath value) {
        ability_system_path = value;
        update_children_ability_system_paths();
    }

    void update_children_ability_system_paths() {
        AbilitySystem *ability_system = get_ability_system();
        if (ability_system) {
            ability_viewer->set_ability_system_path(ability_viewer->get_path_to(ability_system));
            tag_viewer->set_ability_system_path(tag_viewer->get_path_to(ability_system));
            attribute_viewer->set_ability_system_path(attribute_viewer->get_path_to(ability_system));
            event_viewer->set_ability_system_path(event_viewer->get_path_to(ability_system));
        }
    }

    AbilitySystem *get_ability_system() const {
        if (is_inside_tree() && is_node_ready() && has_node(ability_system_path))
            return get_node<AbilitySystem>(ability_system_path);
        return nullptr;
    }
};

#endif