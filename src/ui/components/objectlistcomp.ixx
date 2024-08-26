export module gui:objectlistcomp;

import <vector>;
import <imgui/imgui/imgui.h>;

import c0bezier;
import c0surface;
import c2bezier;
import c2surface;
import gui.controller;
import imguiext;
import interpolatingspline;
import scene;
import surface;
import point;
import torus;

static char name[31]{ 0 };

template <class T>
void renderAddSurfacePopup(Scene& scene, const char* popupName, int minSize)
{
    static bool cylinder = false;
    static int sizeX = 1;
    static int sizeZ = 1;
    static float radius = 0.5f;

    if (ImGui::BeginPopupContextItem(popupName))
    {
        ImGui::Checkbox("Render as cylinder", &cylinder);

        ext::InputClampedInt("Length", &sizeX, minSize, 100);
        if (cylinder)
        {
            ext::InputClampedFloat("Radius", &radius, 0.1f, 100.0F);            
        }
        else
        {
            ext::InputClampedInt("Width", &sizeZ, minSize, 100);
        }

        if (ImGui::Button("Add to scene##addSurface"))
        {
            if (cylinder)
            {
                scene.addSurface<T>(sizeX, radius);
            }
            else
            {
                scene.addSurface<T>(sizeX, sizeZ);
            }
        }

        ImGui::EndPopup();
    }  
}

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
        if (ImGui::Button("Clear scene"))
        {
            scene.clear();
        }

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
            ImGui::EndListBox();
        }
        if (ImGui::Button("New##newtorus"))
        {
            scene.addTorus();
        }

        // Points
        const auto& points = scene.getPoints();
        const auto& selectedPoints = scene.getSelectedPoints();

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
            ImGui::EndListBox();
        }
        if (ImGui::Button("New##newpoint"))
        {
            scene.addPoint();
        }
        if (selectedPoints.size() == 2)
        {
            ImGui::SameLine();
            if (ImGui::Button("Collapse##collapsepoints"))
            {
                scene.collapsePoints();
            }
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
            ImGui::EndListBox();
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

        // Surfaces
        const auto& surfaces = scene.getSurfaces();

        ImGui::Text("Surfaces");
        if (ImGui::BeginListBox("##Surfaces"))
        {
            for (auto& surface : surfaces)
            {
                if (ImGui::Selectable(surface->getName().c_str(), &surface->isSelected))
                {
                    if (surface->isSelected)
                        scene.selectSurface(surface.get());
                    else
                        scene.deselectSurface(surface.get());
                }

                renderRenamePopup(surface);
            }
            ImGui::EndListBox();
        }
        if (ImGui::Button("New C0 Surface ##newc0surface"))
        {
            ImGui::OpenPopup("Add C0 Surface");
        }
        if (ImGui::Button("New C2 Surface ##newc2surface"))
        {
            ImGui::OpenPopup("Add C2 Surface");
        }

        static bool backGregory = false;
        ImGui::Checkbox("Flip cylinder Gregory patch", &backGregory);
        if (ImGui::Button("New Gregory patch ##newgregorypatch"))
        {
            scene.addGregoryPatch(backGregory);
        }

        renderAddSurfacePopup<C0Surface>(scene, "Add C0 Surface", 1);
        renderAddSurfacePopup<C2Surface>(scene, "Add C2 Surface", 4);

        ImGui::End(); 
    }    
}