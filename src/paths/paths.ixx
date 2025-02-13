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

void printPathLen(const std::vector<glm::vec3>& path)
{
    static int pathIndex = 0;
    float len = pathLength(path);
    std::cout << "Path " << pathIndex++ << " length = " << len << "\n";
}

export namespace paths
{
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