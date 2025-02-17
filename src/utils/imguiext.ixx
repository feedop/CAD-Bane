export module imguiext;

import <imgui/imgui/imgui.h>;

void checkBounds(auto* v, auto minValue, auto maxValue)
{
	if (*v < minValue)
		*v = minValue;

	if (*v > maxValue)
		*v = maxValue;
}

/// <summary>
/// Contains ImGui extensions.
/// </summary>
export namespace ext
{
	/// <summary>
	/// Displays an integer input field in the GUI, clamped to a specified range.
	/// </summary>
	/// <param name="label">The label for the input field.</param>
	/// <param name="v">Pointer to the integer value to modify.</param>
	/// <param name="minValue">The minimum allowable value for the integer input.</param>
	/// <param name="maxValue">The maximum allowable value for the integer input.</param>
	/// <returns>Returns true if the input value was changed, otherwise false.</returns>
	bool InputClampedInt(const char* label, int* v, int minValue, int maxValue)
	{
		bool ret = ImGui::InputInt(label, v);
		checkBounds(v, minValue, maxValue);

		return ret;
	}

	/// <summary>
	/// Displays a float input field in the GUI, clamped to a specified range.
	/// </summary>
	/// <param name="label">The label for the input field.</param>
	/// <param name="v">Pointer to the float value to modify.</param>
	/// <param name="minValue">The minimum allowable value for the float input.</param>
	/// <param name="maxValue">The maximum allowable value for the float input.</param>
	/// <returns>Returns true if the input value was changed, otherwise false.</returns>
	bool InputClampedFloat(const char* label, float* v, float minValue, float maxValue)
	{
		bool ret = ImGui::InputFloat(label, v);
		checkBounds(v, minValue, maxValue);

		return ret;
	}
}