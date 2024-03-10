export module gui:objectlistcomp;

import <vector>;
import <imgui/imgui/imgui.h>;

import gui.controller;
import torus;

export void GuiController::renderObjectList()
{
    if (ImGui::Begin("Object list", nullptr, 0))
    {
        // Tori
        ImGui::Text("Tori");

        const auto& tori = repository.getTori();

        if (ImGui::BeginListBox("##Tori"))
        {
            for (auto&& torus : tori)
            {
                ImGui::Selectable(torus->getName().c_str(), &torus->isSelected);
            }

            ImGui::EndListBox();
        }

        // Points

        ImGui::Text("Points");
        ImGui::Text("<empty>");

        ImGui::End(); 
    }    
}