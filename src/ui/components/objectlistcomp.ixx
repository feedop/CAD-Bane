export module gui:objectlistcomp;

import <vector>;
import <imgui/imgui/imgui.h>;

import gui.controller;
import objectrepository;
import point;
import torus;

static char name[31]{ 0 };

export void GuiController::renderObjectList()
{
    if (ImGui::Begin("Object list", nullptr, 0))
    {
        // Tori
        ImGui::Text("Tori");

        const auto& tori = repository.getTori();

        
        if (ImGui::BeginListBox("##Tori"))
        {
            for (auto& torus : tori)
            {
                if (ImGui::Selectable(torus->getName().c_str(), &torus->isSelected))
                {
                    if (torus->isSelected)
                        repository.selectTorus(torus.get());
                    else
                        repository.deselectTorus(torus.get());
                }

                if (ImGui::BeginPopupContextItem())
                {
           
                    ImGui::Text("Rename ");
                    ImGui::InputText("##Rename", name, 30);
                    if (ImGui::Button("Accept"))
                    {
                        if (name[0] != 0)
                            torus->setName(name);
                        ImGui::CloseCurrentPopup();
                    }
                        
                    if (ImGui::Button("Cancel"))
                        ImGui::CloseCurrentPopup();
                    ImGui::EndPopup();
                }
            }

            if (ImGui::Button("New##newtorus"))
            {
                repository.addTorus();
            }

            ImGui::EndListBox();
        }

        // Points
        const auto& points = repository.getPoints();

        ImGui::Text("Points");
        if (ImGui::BeginListBox("##Points"))
        {
            for (auto& point : points)
            {
                if (ImGui::Selectable(point->getName().c_str(), &point->isSelected))
                {
                    if (point->isSelected)
                        repository.selectPoint(point.get());
                    else
                        repository.deselectPoint(point.get());
                }

                if (ImGui::BeginPopupContextItem())
                {

                    ImGui::Text("Rename ");
                    ImGui::InputText("##Rename", name, 30);
                    if (ImGui::Button("Accept"))
                    {
                        if (name[0] != 0)
                            point->setName(name);
                        ImGui::CloseCurrentPopup();
                    }

                    if (ImGui::Button("Cancel"))
                        ImGui::CloseCurrentPopup();
                    ImGui::EndPopup();
                }
            }

            if (ImGui::Button("New##newpoint"))
            {
                repository.addPoint();
            }

            ImGui::EndListBox();
        }

        ImGui::Text("Curves");
        ImGui::Text("<empty>");

        ImGui::End(); 
    }    
}