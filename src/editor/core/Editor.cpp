#include "editor/core/Editor.hpp"

Editor::Editor(Engine& engine)
    : engine(engine) {}

void Editor::OnInit() {
    // 1. Initialize ImGui context and SDL3 backend bindings
    // 2. Setup editor camera position
}

void Editor::OnUpdate(double deltaTime) {
    // Free-fly editor camera control instead of player tracking camera
    RenderSystem& renderSys = engine.getRenderSystem();
    cameraSystem.update(renderSys.getCamera(), engine.getInputManager());

    if (currentMode == EditorMode::Play) {
        // Run game systems only when testing in-editor
    }
}

void Editor::OnRender() {
    engine.getRenderSystem().render();
    // 2. Draw ImGui Editor Overlay
    // renderMenuBar();
    // renderHierarchyPanel();
    // renderInspectorPanel();
}

void Editor::OnShutdown() {
    // Cleanup ImGui contexts
}

const Camera& Editor::getCamera() const {
    return camera;
}