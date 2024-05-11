export module imguiext;

import <imgui/imgui/imgui.h>;

void checkBounds(auto* v, auto minValue, auto maxValue)
{
	if (*v < minValue)
		*v = minValue;

	if (*v > maxValue)
		*v = maxValue;
}

export namespace ext
{
	bool InputClampedInt(const char* label, int* v, int minValue, int maxValue)
	{
		bool ret = ImGui::InputInt(label, v);
		checkBounds(v, minValue, maxValue);

		return ret;
	}

	bool InputClampedFloat(const char* label, float* v, float minValue, float maxValue)
	{
		bool ret = ImGui::InputFloat(label, v);
		checkBounds(v, minValue, maxValue);

		return ret;
	}
}