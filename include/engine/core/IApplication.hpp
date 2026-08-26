#ifndef IAPPLICATION_HPP
#define IAPPLICATION_HPP
#include <sdl3/SDL.h>
/// @brief A class that defines a generic application to be ran by the engine
class IApplication {
    public:
        virtual ~IApplication() = default;
        virtual void OnInit() {}
        /// @brief A callback allowing the application to hook onto raw SDL events
        virtual void OnEvent(const SDL_Event& e) {}
        virtual void OnUpdate(double deltaTime) = 0;
        virtual void OnRender() = 0;
        virtual void OnShutdown() {}
};
#endif