export module concavehull;

import std;
import glm;
import <concavehull/concavehull.hpp>;

using Point3D = glm::vec3;

std::vector<double> flattenPoints(const auto& points)
{
    std::vector<double> flattenedPoints;
    for (auto&& point : points)
    {
        flattenedPoints.push_back(point.x);
        flattenedPoints.push_back(point.y);
    }
    return flattenedPoints;
}

std::vector<glm::vec3> extractPoints(const std::vector<double>& points)
{
    std::vector<glm::vec3> extractedPoints;
    for (int i =0; i < points.size(); i +=2)
    {
        extractedPoints.emplace_back(points[i], points[i + 1], 1.5f);
    }
    return extractedPoints;
}

// Helper function to check if the z-component is close to 1.5
inline bool isCloseTo15(const glm::vec3& point)
{
    static constexpr float eps = 0.01f;
    return glm::abs(point.z - 1.5f) < eps;
}

export // Function to filter points and compute the convex hull
std::vector<glm::vec3> getConcaveHullOfFilteredPoints(const std::vector<glm::vec3>& points, double alpha = 0.015)
{
    // Filter points based on z-component
    std::vector<glm::vec2> filteredPoints;
    for (auto&& point : points)
    {
        if (isCloseTo15(point))
        {
            filteredPoints.emplace_back(point.x, point.y);
        }
    }

    // Compute and return the convex hull of the filtered points
    auto flattenedPoints = flattenPoints(filteredPoints);
    return extractPoints(concavehull(std::span<double>(flattenedPoints), alpha));
}

export // Function to compute the convex hull
std::vector<glm::vec3> getConcaveHull(const std::vector<glm::vec3>& points, double alpha = 0.05) //backfin 0.05
{
    // Compute and return the convex hull of the filtered points
    auto flattenedPoints = flattenPoints(points);
    return extractPoints(concavehull(std::span<double>(flattenedPoints), alpha));
}