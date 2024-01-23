# GDAbilitySystem
### An Ability System extension for Godot 4

This extension provides a framework for implementing game mechanics. It's inspired by Unreal Engine's Gameplay Ability System.

## Table of Contents
1. [Installation](#installation)
    1. [If you want to include the source files](#if-you-want-to-include-the-source-files)
    2. [If you just want the build](#if-you-just-want-the-build)
2. [Class overview](#class-overview)
    1. [`AbilitySystem`](#abilitysystem)
    2. [`Ability`](#ability)
    3. [`AbilityEvent`](#abilityevent)
    4. [`Attribute`](#attribute)
    5. [`Effect`](#effect)
    6. [`Tag`](#tag)
3. Tutorial (coming soon)

## Installation

### If you want to include the source files
1. Clone this repository somewhere into your project files
2. Update `ability_system.gdextension` with the path to the `bin` folder in your project, if necessary
3. Open or restart Godot

### If you just want the build
1. Copy the `bin` folder and the `ability_system.gdextension` file somewhere into your project
2. Update `ability_system.gdextension` with the path to the `bin` folder in your project, if necessary
3. Open or restart Godot

## Class overview

### `AbilitySystem`

This node can be added as a child to give its parent access to abilities, tags, attributes, etc.

### `Ability`

An ability defines an action or skill that can be granted to any node that has a child `AbilitySystem`.

#### Examples
* Restore health
* Do an attack
* Find an object

### `AbilityEvent`

An ability event defines the state of an `Ability` after activation. This class controls the ability's `Effect`s.

### `Attribute`

Attributes are a numerical value representing some in-game property.

#### Examples
* Health
* Hunger
* Intelligence

### `Effect`

An effect defines what happens in-game when an `Ability` is activated.

This is where the bulk of your custom game logic will go. It has access to the `AbilitySystem` so an effect can read tags, modify the scene tree, or really anything else.

#### Examples
* For an attack spell ability, the effects could be...
    * Decrease the mana attribute
    * Play a cool spell animation
    * Decrease health of any enemies in the surrounding area
* For an object finding ability, the effects could be...
    * Locate the object within the world
    * Move character to the object
    * Play pick up animation


#### Subclasses
* `AttributeEffect` - modifies an `Attribute`
* `TagEffect` - adds or removes `Tag`s from `AbilitySystem`
* `WaitEffect` - arbitrarily delays `AbilityEvent`
* `TryActivateAbilityEffect` - attempts to activate an `Ability` on the `AbilitySystem`

### `Tag`

Tags are names that classify and describe the state of an `AbilitySystem` node. They can be used together with abilities to create super dynamic states.

#### Examples
* `spells/is_casting`
* `spells/has_cast_recently`
* `is_sleepy`

### Viewer classes

These classes are mainly used for debugging purposes. They are `Control` nodes that can be added to your scene. Each of them export an `AbilitySystem` property and will be reactive to changes on that `AbilitySystem`. They work both in-editor and in-game.

#### `TagViewer`
Displays `Tag`s when provided an `AbilitySystem`.

#### `AttributeViewer`
Displays `Attribute`s and their values when provided an `AbilitySystem`.

#### `EventViewer`
Displays `AbilityEvent`s when provided an `AbilitySystem`.

#### `AbilityViewer`
Displays `Ability`s when provided an `AbilitySystem`.