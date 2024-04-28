export module gui:objectlistcomp;

import <vector>;
import <imgui/imgui/imgui.h>;

import c0bezier;
import c2bezier;
import gui.controller;
import interpolatingspline;
import scene;
import point;
import torus;

static char name[31]{ 0 };

void renderRenamePopup(auto& object)
{
    if (ImGui::BeginPopupContextItem())
    {

        ImGui::Text("Rename ");
        ImGui::InputText("##Rename", name, 30);
        if (ImGui::Button("Accept"))
        {
            if (name[0] != 0)
                object->setName(name);
            ImGui::CloseCurrentPopup();
        }

        if (ImGui::Button("Cancel"))
            ImGui::CloseCurrentPopup();
        ImGui::EndPopup();
    }
}

export void GuiController::renderObjectList()
{
    if (ImGui::Begin("Object list", nullptr, 0))
    {
        // Tori
        ImGui::Text("Tori");

        const auto& tori = scene.getTori();

        
        if (ImGui::BeginListBox("##Tori"))
        {
            for (auto& torus : tori)
            {
                if (ImGui::Selectable(torus->getName().c_str(), &torus->isSelected))
                {
                    if (torus->isSelected)
                        scene.selectTorus(torus.get());
                    else
                        scene.deselectTorus(torus.get());
                }

                renderRenamePopup(torus);
            }

            if (ImGui::Button("New##newtorus"))
            {
                scene.addTorus();
            }

            ImGui::EndListBox();
        }

        // Points
        const auto& points = scene.getPoints();

        ImGui::Text("Points");
        if (ImGui::BeginListBox("##Points"))
        {
            for (auto& point : points)
            {
                if (ImGui::Selectable(point->getName().c_str(), &point->isSelected))
                {
                    if (point->isSelected)
                        scene.selectPoint(point.get());
                    else
                        scene.deselectPoint(point.get());
                }

                renderRenamePopup(point);
            }

            if (ImGui::Button("New##newpoint"))
            {
                scene.addPoint();
            }

            ImGui::EndListBox();
        }

        // Curves
        const auto& curves = scene.getCurves();

        ImGui::Text("Curves");
        if (ImGui::BeginListBox("##Curves"))
        {
            for (auto& curve : curves)
            {
                if (ImGui::Selectable(curve->getName().c_str(), &curve->isSelected))
                {
                    if (curve->isSelected)
                        scene.selectCurve(curve.get());
                    else
                        scene.deselectCurve(curve.get());
                }

                renderRenamePopup(curve);
            }

            if (ImGui::Button("New C0 Bezier curve ##newc0bezier"))
            {
                scene.addCurve<C0Bezier>();
            }
            if (ImGui::Button("New C2 Bezier curve ##newc2bezier"))
            {
                scene.addCurve<C2Bezier>();
            }
            if (ImGui::Button("New C2 interpolating spline ##newinterpolatingspline"))
            {
                scene.addCurve<InterpolatingSpline>();
            }

            ImGui::EndListBox();
        }

        ImGui::End(); 
    }    
}