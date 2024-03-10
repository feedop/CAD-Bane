export module gui:torusconfigcomp;

import <algorithm>;
import <format>;
import <imgui/imgui/imgui.h>;

import gui.controller;
import torus;

export void GuiController::renderTorusConfig()
{
	const auto& tori = repository.getTori();
	for (auto&& torus : tori)
	{
		if (torus->isSelected)
		{
			ImGui::Text(torus->getName().c_str());

			// Using bitwise OR to disable short-circuiting 
			if (ImGui::SliderFloat(std::format("{} R", torus->getName()).c_str(), &torus->R, 0.1f, 5.0f) |
				ImGui::SliderFloat(std::format("{} r ", torus->getName()).c_str(), &torus->r, 0.1f, 5.0f) |
				ImGui::SliderInt(std::format("{} major points", torus->getName()).c_str(), &torus->majorPoints, 3, 50) |
				ImGui::SliderInt(std::format("{} minor points", torus->getName()).c_str(), &torus->minorPoints, 3, 50)
				)
			{
				torus->calculateTorus();
			}

			ImGui::NewLine();
			ImGui::Separator();
			ImGui::NewLine();
		}
	}
}