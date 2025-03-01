export module paths;

import std;
import glm;

import :rough;
import :flat;
import :detailed;
import scene;
import renderer;
import config;
import pathutils;

/// <summary>
/// Serializes a vector of 3D points into a formatted G-code style string stream.
/// </summary>
/// <param name="path">The vector of glm::vec3 points representing a path.</param>
/// <returns>A stringstream containing the formatted path data.</returns>
std::stringstream serialize(const std::vector<glm::vec3>& path)
{
    std::stringstream ss;
    ss << std::fixed << std::setprecision(3);
    for (int i = 0; i < path.size(); i++)
    {
        auto&& point = path[i];
        ss << "N" << i << "G01X" << point.x * 10 << "Y" << point.y * 10 << "Z" << point.z * 10 << "\n"; // TODO: make sure it's \r\n
    }

    return ss;
}

/// <summary>
/// Saves a vector of 3D points to a specified file in a serialized format.
/// </summary>
/// <param name="path">The vector of glm::vec3 points representing a path.</param>
/// <param name="file">The file path where the serialized data should be saved.</param>
void saveToFile(const std::vector<glm::vec3>& path, const char* file)
{
    std::ofstream fs;
    // ensure ifstream objects can throw exceptions:
    fs.exceptions(std::ofstream::failbit | std::ofstream::badbit);
    try
    {
        // open files
        fs.open(file);
        std::stringstream ss = serialize(path);
        fs << ss.rdbuf();
        // close file handlers
        fs.flush();
        fs.close();
    }
    catch (std::ifstream::failure const&)
    {
        std::cerr << "ERROR::PATHS::FILE_NOT_SUCCESFULLY_OPENED, path: " << file << std::endl;
    }
}

/// <summary>
/// Computes and prints the length of a given path.
/// </summary>
/// <param name="path">The vector of glm::vec3 points representing the path.</param>
void printPathLen(const std::vector<glm::vec3>& path)
{
    static int pathIndex = 0;
    float len = pathLength(path);
    std::cout << "Path " << pathIndex++ << " length = " << len << "\n";
}

/// <summary>
/// Functions related to generating tool paths.
/// </summary>
export namespace paths
{
    /// <summary>
    /// Generates different types of paths from a scene and saves them to files.
    /// </summary>
    /// <param name="scene">The reference to the Scene object.</param>
    /// <param name="renderer">The reference to the Renderer object.</param>
	void generatePaths(Scene& scene, const Renderer& renderer)
	{
		static constexpr const char* path1File = "paths/1.k16";
        static constexpr const char* path2File = "paths/2.f10";
        static constexpr const char* path3File = "paths/3.k08";
        static constexpr int heightSize = cfg::initialWidth;

        auto heightMap = renderer.getPathHeightMap(heightSize);

		auto path1 = generateRoughPath(scene, heightMap, heightSize);
        printPathLen(path1);
        auto path2 = generateFlatPath(scene);
        printPathLen(path2);
        auto path3 = generateDetailedPath(scene);
        printPathLen(path3);

        saveToFile(path1, path1File);
        saveToFile(path2, path2File);
        saveToFile(path3, path3File);
	}
}