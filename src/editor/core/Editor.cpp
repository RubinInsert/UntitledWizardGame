#include "editor/core/Editor.hpp"
#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_sdlgpu3.h"
Editor::Editor(Engine& engine)
    : engine(engine) {}

void Editor::OnInit() {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

    // Setup Platform/Renderer backends
    ImGui_ImplSDL3_InitForSDLGPU(engine.getWindowManager().getWindow());
    ImGui_ImplSDLGPU3_InitInfo init_info = {};
    init_info.Device = engine.getGPUDevice();
    init_info.ColorTargetFormat = SDL_GetGPUSwapchainTextureFormat(
        engine.getGPUDevice(), engine.getWindowManager().getWindow());
    init_info.MSAASamples = SDL_GPU_SAMPLECOUNT_1;
    init_info.SwapchainComposition = SDL_GPU_SWAPCHAINCOMPOSITION_SDR;
    init_info.PresentMode = SDL_GPU_PRESENTMODE_VSYNC;
    ImGui_ImplSDLGPU3_Init(&init_info);

    // Create offscreen target for the 3D viewport
    int w = engine.getScreenWidth();
    int h = engine.getScreenHeight();
    viewportTarget = std::make_unique<RenderTarget>(engine.getGPUDevice(), w, h);
}

void Editor::OnUpdate(double deltaTime) {
    // Free-fly editor camera control instead of player tracking camera
    ImGui_ImplSDLGPU3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();

    ImGui::ShowDemoWindow();
    // // 2. Build editor UI panels
    // renderMenuBar();
    // renderHierarchyPanel();
    // renderInspectorPanel();
    // renderViewportPanel();

    // 3. Editor camera control
    RenderSystem& renderSys = engine.getRenderSystem();
    cameraSystem.update(renderSys.getCamera(), engine.getInputManager());

    // 4. Render 3D scene to offscreen viewport target
    //    (This uses its own command buffer internally)
    engine.getRenderSystem().renderToTarget(*viewportTarget);
}

void Editor::OnRender() {
    // At this point, ImGui draw data is ready from OnUpdate
    ImGui::Render();

    SDL_GPUDevice* device = engine.getGPUDevice();
    SDL_Window* window = engine.getWindowManager().getWindow();

    // Acquire command buffer and swapchain texture
    SDL_GPUCommandBuffer* cmd = SDL_AcquireGPUCommandBuffer(device);
    if (!cmd) return;

    SDL_GPUTexture* swapchainTex;
    Uint32 w, h;
    if (!SDL_WaitAndAcquireGPUSwapchainTexture(cmd, window, &swapchainTex, &w, &h)) {
        SDL_CancelGPUCommandBuffer(cmd);
        return;
    }

    // Prepare ImGui vertex/index buffers before the render pass
    ImGui_ImplSDLGPU3_PrepareDrawData(ImGui::GetDrawData(), cmd);

    // Begin swapchain render pass (no depth needed for ImGui overlay)
    SDL_GPUColorTargetInfo colorTarget{};
    colorTarget.texture = swapchainTex;
    colorTarget.clear_color = {0.0f, 0.0f, 0.0f, 0.0f};  // clear to transparent
    colorTarget.load_op = SDL_GPU_LOADOP_CLEAR;
    colorTarget.store_op = SDL_GPU_STOREOP_STORE;

    SDL_GPURenderPass* pass = SDL_BeginGPURenderPass(cmd, &colorTarget, 1, nullptr);
    if (!pass) {
        SDL_CancelGPUCommandBuffer(cmd);
        return;
    }

    // Render ImGui (the 3D scene is already in the viewport target texture)
    ImGui_ImplSDLGPU3_RenderDrawData(ImGui::GetDrawData(), cmd, pass);

    SDL_EndGPURenderPass(pass);
    SDL_SubmitGPUCommandBuffer(cmd);
}
void Editor::OnEvent(const SDL_Event& e) {
    ImGui_ImplSDL3_ProcessEvent(&e); 
}

void Editor::OnShutdown() {
    viewportTarget.reset();
    ImGui_ImplSDLGPU3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();
}

const Camera& Editor::getCamera() const {
    return camera;
}