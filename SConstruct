#!/usr/bin/env python

env = SConscript("godot-cpp/SConstruct")

env.Append(CPPPATH=["src/"])
sources = (
    Glob("src/*.cpp")
    + Glob("src/effect/*.cpp")
    + Glob("src/editor/*.cpp")
    + Glob("src/editor/attribute_inspector/*.cpp")
)

if env["platform"] == "macos":
    library = env.SharedLibrary(
        "bin/libability_system.{}.{}.framework/libability_system.{}.{}".format(
            env["platform"], env["target"], env["platform"], env["target"]
        ),
        source=sources,
    )
else:
    library = env.SharedLibrary(
        "bin/libability_system{}{}".format(env["suffix"], env["SHLIBSUFFIX"]),
        source=sources,
    )

Default(library)
