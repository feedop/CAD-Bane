export module gui:stereocomp;

import :controller;

export void GuiController::renderStereoConfig()
{
	ImGui::Checkbox("Stereoscopic rendering", &renderer.stereo);

	if (renderer.stereo)
	{
		if (ImGui::SliderFloat("Left eye position", &camera.leftEye, 0.0f, 0.1f))
		{
			camera.calculateRedProjection();
		}
		if (ImGui::SliderFloat("Right eye position", &camera.rightEye, 0.0f, 0.1f))
		{
			camera.calculateBlueProjection();
		}
		if (ImGui::SliderFloat("Projection plane distance", &camera.projectionPlaneDist, 0.5f, 20.0f))
		{
			camera.calculateRedProjection();
			camera.calculateBlueProjection();
		}
	}
}