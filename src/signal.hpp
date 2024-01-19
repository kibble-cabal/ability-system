#ifndef AS_SIGNAL_HPP
#define AS_SIGNAL_HPP

namespace as_signal {
static const auto EventBlocked = "ability_event_blocked";
static const auto EventFinished = "ability_event_finished";
static const auto EventStarted = "ability_event_started";
static const auto EventsChanged = "events_changed";
static const auto TagGranted = "tag_granted";
static const auto TagRevoked = "tag_revoked";
static const auto TagsChanged = "tags_changed";
static const auto AbilityGranted = "ability_granted";
static const auto AbilityRevoked = "ability_revoked";
static const auto AbilitiesChanged = "abilities_changed";
static const auto AttributeGranted = "attribute_granted";
static const auto AttributeRevoked = "attribute_revoked";
static const auto AttributeValueChanged = "attribute_value_changed";
static const auto AttributesChanged = "attributes_changed";
static const auto EffectFinished = "effect_finished";
static const auto EffectsChanged = "effects_changed";
}; //namespace as_signal

#endif