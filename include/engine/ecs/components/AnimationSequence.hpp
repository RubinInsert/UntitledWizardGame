#ifndef ANIMATIONSEQUENCE_H
#define ANIMATIONSEQUENCE_H
#include <string>
struct AnimationSequence {
    std::string name;
    int startFrame;
    int frameCount;
    float frameDuration;
    bool loop = true;
};

#endif