export module concavehull;

import std;
import glm;
import <concavehull/concavehull.hpp>;

using Point3D = glm::vec3;

/// <summary>
/// Flattens a collection of 2D or 3D points into a 1D vector of double values.
/// </summary>
/// <param name="points">The collection of points to be flattened.</param>
/// <returns>A vector of double values containing the x and y coordinates of the input points.</returns>
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

/// <summary>
/// Extracts a collection of 3D points from a flattened vector of double values.
/// </summary>
/// <param name="points">A vector of double values where each consecutive pair represents an (x, y) coordinate.</param>
/// <returns>A vector of glm::vec3 points, with a default z-value of 1.5.</returns>
std::vector<glm::vec3> extractPoints(const std::vector<double>& points)
{
    std::vector<glm::vec3> extractedPoints;
    for (int i =0; i < points.size(); i +=2)
    {
        extractedPoints.emplace_back(points[i], points[i + 1], 1.5f);
    }
    return extractedPoints;
}

/// <summary>
/// Checks if the z-component of a given 3D point is close to 1.5.
/// </summary>
/// <param name="point">The 3D point to check.</param>
/// <returns>True if the z-value is approximately 1.5, otherwise false.</returns>
inline bool isCloseTo15(const glm::vec3& point)
{
    static constexpr float eps = 0.01f;
    return glm::abs(point.z - 1.5f) < eps;
}

/// <summary>
/// Computes the concave hull of a filtered subset of points based on the z-component.
/// </summary>
/// <param name="points">The input collection of 3D points.</param>
/// <param name="alpha">The alpha parameter controlling the concavity of the hull.</param>
/// <returns>A vector of 3D points representing the computed concave hull.</returns>
export std::vector<glm::vec3> getConcaveHullOfFilteredPoints(const std::vector<glm::vec3>& points, double alpha = 0.015)
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

/// <summary>
/// Computes the concave hull of a given set of 3D points.
/// </summary>
/// <param name="points">The input collection of 3D points.</param>
/// <param name="alpha">The alpha parameter controlling the concavity of the hull.</param>
/// <returns>A vector of 3D points representing the computed concave hull.</returns>
export std::vector<glm::vec3> getConcaveHull(const std::vector<glm::vec3>& points, double alpha = 0.05) //backfin 0.05
{
    // Compute and return the convex hull of the filtered points
    auto flattenedPoints = flattenPoints(points);
    return extractPoints(concavehull(std::span<double>(flattenedPoints), alpha));
}