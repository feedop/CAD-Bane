export module gui:torusconfigcomp;

import std;

import <imgui/imgui/imgui.h>;

import gui.controller;
import torus;

export void GuiController::renderTorusConfig()
{
	const auto& selectedTori = scene.getSelectedTori();
	for (auto&& torus : selectedTori)
	{
		ImGui::Text(torus->getName().c_str());

		// Using bitwise OR to disable short-circuiting 
		if (ImGui::SliderFloat(std::format("{} R", torus->getName()).c_str(), &torus->R, 0.1f, 5.0f) |
			ImGui::SliderFloat(std::format("{} r ", torus->getName()).c_str(), &torus->r, 0.1f, 5.0f) |
			ImGui::SliderInt(std::format("{} major points", torus->getName()).c_str(), &torus->majorPoints, 3, 100) |
			ImGui::SliderInt(std::format("{} minor points", torus->getName()).c_str(), &torus->minorPoints, 3, 100)
			)
		{
			torus->calculateTorus();
		}

		ImGui::NewLine();
		ImGui::Separator();
		ImGui::NewLine();
	}
}