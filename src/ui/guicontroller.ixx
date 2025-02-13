export module gui:controller;

import std;
import glm;

import <imgui.h>;
import <imgui/backend/imgui_impl_glfw.h>;
import <imgui/backend/imgui_impl_opengl3.h>;
import <GLFW/glfw3.h>;

import camera;
import config;
import ellipsoid;
import scene;
import paths;
import raycaster;
import renderer;
import theme;
import torus;

ImGuiIO& createImguiContext()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    return ImGui::GetIO();
}

/// <summary>
/// Responsible for managing the GUI
/// </summary>
export class GuiController
{
public:
    GuiController(GLFWwindow* window, Camera& camera, Scene& scene, Raycaster& raycaster, Renderer& renderer, Ellipsoid& ellipsoid) :
        io(createImguiContext()), camera(camera), scene(scene), raycaster(raycaster), renderer(renderer), ellipsoid(ellipsoid)
    {
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
        //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
        //io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
        //io.ConfigViewportsNoAutoMerge = true;
        //io.ConfigViewportsNoTaskBarIcon = true;

        // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
        ImGuiStyle& style = ImGui::GetStyle();
        style.WindowMinSize = ImVec2(500, 500);

        cinder::setTheme();

        // Setup Platform/Renderer backends
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init("#version 420");
    }

    void draw()
    {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // imgui debug
        bool showDemo = false;
        if (showDemo)
        {
            ImGui::ShowDemoWindow(&showDemo);
            render();
            return;
        }

        ImGui::Begin("Configuration window", nullptr, ImGuiWindowFlags_MenuBar); // Create a window and append into it.

        renderMenuBar();

        static bool fpsLimit = true;
        if (ImGui::Checkbox("Limit fps to screen frequency", &fpsLimit))
        {
            glfwSwapInterval(fpsLimit);
        }

        ImGui::Checkbox("Render grid", &renderer.drawGrid);

        if (ImGui::Checkbox("Render ellipsoid", &raycaster.enabled) && raycaster.enabled)
        {
            raycaster.enqueueUpdate();
        }
        if (raycaster.enabled)
        {
            renderEllipsoid();
        }

        ImGui::Checkbox("Render curve polygons", &renderer.drawPolygons);

        renderStereoConfig();

        if (renderer.getParametricViewCanvas() != nullptr)
        {
            if (ImGui::Button("Close parametric view window"))
                renderer.resetParametricViewCurve();
        }

        if (ImGui::Button("Generate paths"))
        {
            if constexpr (cfg::enablePathGeneration)
                paths::generatePaths(scene, renderer);
        }

        /*ImGui::Text("Cursor position: ");
        auto&& cursorPosition = scene.getCursor()->getPosition();
        auto text = std::format("glm::vec3({}, {}, {})", cursorPosition.x, cursorPosition.y, cursorPosition.z).c_str();
        ImGui::Text(text);
        if (ImGui::Button("Copy"))
        {
            ImGui::SetClipboardText(text);
        }*/

        renderTorusConfig();
        renderSurfaceConfig();

        ImGui::End();

        // Render other windows
        renderObjectList();

        render();
    }
private:
    ImGuiIO& io;
    Camera& camera;
    Scene& scene;
    Raycaster& raycaster;
    Renderer& renderer;
    Ellipsoid& ellipsoid;

    static std::vector<Torus*> selectedTori;

    void render()
    {
        ImGui::Render();

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Update and Render additional Platform Windows
        // (Platform functions may change the current OpenGL context, so we save/restore it to make it easier to paste this code elsewhere.
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            GLFWwindow* backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_current_context);
        }
    }

    // Components
    void renderMenuBar();
    void renderEllipsoid();
    void renderStereoConfig();
    void renderObjectList();
    void renderTorusConfig();
    void renderSurfaceConfig();
};