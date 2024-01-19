# config.py


def can_build(env, platform):
    return True


def configure(env):
    pass


def get_doc_path():
    return "docs"


def get_doc_classes():
    core_classes = [
        "AbilityEvent",
        "Ability",
        "AbilitySystem",
        "Attribute",
        "Tag",
    ]
    effect_classes = [
        "Effect",
        "AttributeEffect",
        "WaitEffect",
        "TagEffect",
    ]
    editor_classes = [
        "AbilityViewer",
        "TagViewer",
        "AttributeViewer",
        "EventViewer",
    ]
    return core_classes + effect_classes + editor_classes
