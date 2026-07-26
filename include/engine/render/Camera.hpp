#ifndef CAMERA_H
#define CAMERA_H
#include <SDL3/SDL.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
struct Camera {
    glm::vec3 position{0.f, 0.f, 5.f};  // World position of camera
    glm::vec3 target{0.f, 0.f, 0.f}; // What the camera is looking at
    glm::vec3 up{0.f, 1.f, 0.f};
    float fovY = 45.0f;
    float aspectRatio = 16.0f / 9.0f;
    float nearPlane = 0.1f;
    float farPlane = 1000.0f;
    
    glm::mat4 getViewMatrix() const {
        return glm::lookAt(position, target, up);
    }

    glm::mat4 getProjectionMatrix() const {
        return glm::perspective(glm::radians(fovY), aspectRatio, nearPlane, farPlane);
    }

    // Helpers
    void orbit(float yawDelta, float pitchDelta) {
        // Current offset from target
        glm::vec3 offset = position - target;
        float dist = glm::length(offset);

        // Derive current spherical angles from the offset
        float yaw   = glm::degrees(atan2(offset.z, offset.x));
        float pitch = glm::degrees(asinf(glm::clamp(offset.y / dist, -1.0f, 1.0f)));

        // Apply deltas
        yaw   += yawDelta;
        pitch  = glm::clamp(pitch + pitchDelta, -89.0f, 89.0f);

        // Write back to position
        float cosPitch = cos(glm::radians(pitch));
        position = target + glm::vec3{
            dist * cos(glm::radians(yaw)) * cosPitch,
            dist * sin(glm::radians(pitch)),
            dist * sin(glm::radians(yaw)) * cosPitch
        };
    }

    void zoom(float factor) {
        glm::vec3 direction = glm::normalize(target - position);
        float newDist = glm::clamp(
            glm::distance(position, target) * factor, 0.5f, 50.0f
        );
        position = target - direction * newDist;
    }
};

#endif