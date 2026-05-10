#ifndef PLAYERANIMS_H
#define PLAYERANIMS_H
#include "engine/ecs/components/AnimationSequence.hpp"
namespace PlayerAnims {
    const AnimationSequence IDLE_NORTH  = {"IDLE_NORTH", 24, 1, 0.15f, true };
    const AnimationSequence IDLE_NORTHEAST  = {"IDLE_NORTHEAST", 32, 1, 0.15f, true };
    const AnimationSequence IDLE_EAST  = {"IDLE_EAST", 40, 1, 0.15f, true };
    const AnimationSequence IDLE_SOUTHEAST  = {"IDLE_SOUTHEAST", 48, 1, 0.15f, true };
    const AnimationSequence IDLE_SOUTH  = {"IDLE_SOUTH", 56, 1, 0.15f, true };
    const AnimationSequence IDLE_SOUTHWEST  = {"IDLE_SOUTHWEST", 0, 1, 0.15f, true };
    const AnimationSequence IDLE_WEST  = {"IDLE_WEST", 8, 1, 0.15f, true };
    const AnimationSequence IDLE_NORTHWEST  = {"IDLE_NORTHWEST", 16, 1, 0.15f, true };
    const AnimationSequence WALK_NORTH    = {"WALK_NORTH",24, 4, 0.1f, true };
    const AnimationSequence WALK_NORTHEAST    = {"WALK_NORTHEAST",32, 4, 0.1f, true };
    const AnimationSequence WALK_EAST = {"WALK_EAST", 40, 4, 0.1f, true };
    const AnimationSequence WALK_SOUTHEAST = {"WALK_SOUTHEAST", 48, 4, 0.1f, true };
    const AnimationSequence WALK_SOUTH  = {"WALK_SOUTH", 56, 4, 0.1f, true };
    const AnimationSequence WALK_SOUTHWEST  = {"WALK_SOUTHWEST", 0, 4, 0.1f, true };
    const AnimationSequence WALK_WEST  = {"WALK_WEST", 8, 4, 0.1f, true };
    const AnimationSequence WALK_NORTHWEST  = {"WALK_NORTHWEST", 16, 4, 0.1f, true };
};

#endif