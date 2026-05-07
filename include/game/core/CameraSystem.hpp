#ifndef CAMERASYSTEM_H
#define CAMERASYSTEM_H
#include "engine/render/Camera.hpp"
#include "engine/core/InputManager.hpp"
#include "engine/ecs/components/Transform.hpp"
#include "engine/core/TimeManager.hpp"
class CameraSystem {

    public:
    CameraSystem();
    void update(Camera& camera, const InputManager& input);
    void followPlayer(Camera& camera, const Transform&, const TimeManager& time);
};

#endif