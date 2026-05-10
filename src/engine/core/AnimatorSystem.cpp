#include "engine/core/AnimatorSystem.hpp"
#include "engine/ecs/components/Animation.hpp"
#include "engine/ecs/components/Sprite.hpp"
void AnimatorSystem::update(entt::registry& registry, float deltaTime) {
    
    auto view = registry.view<Animation, Sprite>();
    for(auto entity : view) {
        auto& anim = view.get<Animation>(entity);
        auto& sprite = view.get<Sprite>(entity);

        anim.timer += deltaTime;
        if(anim.timer >= anim.sequence.frameDuration) {
            anim.timer = 0.0f;
            anim.currentFrameOffset++;

            if( anim.currentFrameOffset >= anim.sequence.frameCount) {
                anim.currentFrameOffset = anim.sequence.loop ? 0 : anim.sequence.frameCount - 1;

            }
        }
        sprite.frame = anim.sequence.startFrame + anim.currentFrameOffset;
    }
}