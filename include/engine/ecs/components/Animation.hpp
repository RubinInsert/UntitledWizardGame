#ifndef ANIMATION_H
#define ANIMATION_H

#include "engine/ecs/components/AnimationSequence.hpp"
struct Animation {
    AnimationSequence sequence;
    float timer = 0.0f;
    int currentFrameOffset = 0; // 0 to (frameCount - 1)
};

#endif