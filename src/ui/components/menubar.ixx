export module gui:menubar;

import config;
import gui.controller;
import scene;

import <iostream>;
import <imguiFileDialog/ImGuiFileDialog.h>;
import <Serializer/Serializer.h>;

enum class IoOperation
{
	None,
	Open,
	Save
};

export void renderDialogWindow(Scene& scene, IoOperation& operation)
{
	// display
	if (ImGuiFileDialog::Instance()->Display("ChooseFileDlgKey"))
	{
		// action if OK
		if (ImGuiFileDialog::Instance()->IsOk())
		{
			std::string filePath = ImGuiFileDialog::Instance()->GetFilePathName();
			try
			{
				switch (operation)
				{
				case IoOperation::Open:
				{
					MG1::SceneSerializer serializer;
					serializer.LoadScene(filePath);
					auto& mgscene = MG1::Scene::Get();
					scene.deserialize(mgscene);
				}
					break;

				case IoOperation::Save:
				{
					MG1::SceneSerializer serializer;
					auto& mgscene = MG1::Scene::Get();
					mgscene.Clear();
					scene.serialize(mgscene);
					serializer.SaveScene(filePath);
				}
					break;
				}
			}
			catch (...)
			{
				std::cerr << "Error in json operation\n";
			}

			operation = IoOperation::None;
		}

		// close
		ImGuiFileDialog::Instance()->Close();
	}
}

export void GuiController::renderMenuBar()
{
	static IoOperation selectedOperation = IoOperation::None;

    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
			IGFD::FileDialogConfig config;
			config.path = ".";
            if (ImGui::MenuItem("Open"))
            {
				selectedOperation = IoOperation::Open;
				
				ImGuiFileDialog::Instance()->OpenDialog("ChooseFileDlgKey", "Choose file", ".json", config);
            }
            if (ImGui::MenuItem("Save"))
            {
				selectedOperation = IoOperation::Save;
				ImGuiFileDialog::Instance()->OpenDialog("ChooseFileDlgKey", "Save as", ".json", config);
            }
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }
    renderDialogWindow(scene, selectedOperation);
}