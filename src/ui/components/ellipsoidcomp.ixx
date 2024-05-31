export module gui:ellipsoidcomp;

import config;
import gui.controller;

export void GuiController::renderEllipsoid()
{
    ImGui::Text("Ellipsoid parameters");

    // Bitwise OR to disable short-circuiting
    bool update = false;
    update |= ImGui::SliderFloat("a", &ellipsoid.a, 0.1f, 2.0f)
        | ImGui::SliderFloat("b", &ellipsoid.b, 0.1f, 2.0f)
        | ImGui::SliderFloat("c", &ellipsoid.c, 0.1f, 2.0f);

    ImGui::Text("Lighting");
    update |= ImGui::SliderInt("m", &raycaster.lightM, 1, 128);

    static bool gpuRender = true;
    if (ImGui::Checkbox("GPU raycasting", &gpuRender))
    {
        raycaster.setMode(gpuRender);
    }

    ImGui::Text("Adaptive rendering");
    update |= ImGui::SliderInt("Adaptation level", &raycaster.adaptationLevel, 0, cfg::maxAdaptationLevel);
    update |= ImGui::Checkbox("Selected level only", &raycaster.selectedAdaptationLevelOnly);

    if (update)
        raycaster.enqueueUpdate();

}