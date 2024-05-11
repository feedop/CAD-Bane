export module gui:surfaceconfigcomp;

import <algorithm>;
import <format>;
import <imgui/imgui/imgui.h>;

import gui.controller;
import torus;

export void GuiController::renderSurfaceConfig()
{
	const auto& selectedSurfaces = scene.getSelectedSurfaces();
	for (auto&& surface : selectedSurfaces)
	{
		ImGui::Text(surface->getName().c_str());

		ImGui::SliderFloat(std::format("{} density X ", surface->getName()).c_str(), &surface->densityX, 4.0f, 64.0f);
		ImGui::SliderFloat(std::format("{} density Z ", surface->getName()).c_str(), &surface->densityZ, 4.0f, 64.0f);

		ImGui::NewLine();
		ImGui::Separator();
		ImGui::NewLine();
	}
}