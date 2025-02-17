export module config;

/// <summary>
/// Various compile-time app settings.
/// </summary>
export namespace cfg
{
	inline constexpr int initialWidth = 800;
	inline constexpr int initialHeight = 800;

	inline constexpr int maxAdaptationLevel = 5;
	inline constexpr bool enablePathGeneration = false;
	inline constexpr bool savePathTextures = false;
}