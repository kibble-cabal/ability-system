# config.py


def can_build(env, platform):
    return True


def configure(env):
    pass


def get_doc_path():
    return "docs"


def get_doc_classes():
    return [
        "AbilityEvent",
        "Ability",
        "AbilitySystem",
        "AttributeMap",
        "Attribute",
        "Effect",
        "AttributeEffect",
        "WaitEffect",
        "Tag",
    ]
