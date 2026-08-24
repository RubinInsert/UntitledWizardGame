#ifndef EDITOR_APP_HPP
#define EDITOR_APP_HPP

#include <entt/entt.hpp>
#include <SDL3/SDL.h>
#include "engine/core/Engine.h"
#include "engine/render/Camera.hpp"
#include "game/core/CameraSystem.hpp"
#include "game/world/Map.hpp"

// Forward declaration for scene/gizmo picking
enum class EditorMode { Edit, Play, Pause };

class Editor : public IApplication {
public:
    Editor(Engine& engine);
    ~Editor() override = default;

    void OnInit() override;
    void OnUpdate(double deltaTime) override;
    void OnRender() override;
    void OnShutdown() override;

    const Camera& getCamera() const;

private:
    void setupEditorUI();
    void renderMenuBar();
    void renderHierarchyPanel();
    void renderInspectorPanel();
    void renderViewportPanel();

    void loadScene(const std::string& filepath);
    void saveScene(const std::string& filepath);

    // Systems
    Engine& engine;
    Camera camera{};
    CameraSystem cameraSystem{};
    Map worldMap;

    // Editor State
    EditorMode currentMode{EditorMode::Edit};
    entt::entity selectedEntity{entt::null};
};

#endif // EDITOR_APP_HPP