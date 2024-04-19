export module scene;

import <cmath>;
import <memory>;
import <vector>;

import <glm/vec3.hpp>;
import <glm/gtc/constants.hpp>;

import c0bezier;
import c2bezier;
import camera;
import cube;
import cursor;
import curve;
import grid;
import math;
import middlepoint;
import solidobject;
import point;
import pointrenderer;
import torus;

export class Scene
{
public:
	Scene(const Camera& camera, PointRenderer& pointRenderer) : camera(camera), pointRenderer(pointRenderer)
	{
		// Cursor
		cursor = std::make_unique<Cursor>();

		// Initial objects
		points.emplace_back(new Point(glm::vec3{ 0.5f, 0, 0 }));
		points.emplace_back(new Point(glm::vec3{ 1, 0, 0 }));
		points.emplace_back(new Point(glm::vec3{ 1, 1, 0 }));
		points.emplace_back(new Point(glm::vec3{ -1, 1, 0 }));
		points.emplace_back(new Point(glm::vec3{ -1, 0, 0 }));
		points.emplace_back(new Point(glm::vec3{ -0.5f, 0, 0 }));

		curves.emplace_back(new C2Bezier{ points[0].get(), points[1].get(), points[2].get(), points[3].get(), points[4].get(), points[5].get() });

		pointRenderer.update(points);
	}

	inline const SolidObject* getGrid() const
	{
		return grid.get();
	}

	inline const SolidObject* getCursor() const
	{
		return cursor.get();
	}

	inline const std::vector<std::unique_ptr<Torus>>& getTori() const
	{
		return tori;
	}

	inline const std::vector<Torus*>& getSelectedTori() const
	{
		return selectedTori;
	}

	inline const std::vector<std::unique_ptr<Point>>& getPoints() const
	{
		return points;
	}

	inline const std::vector<std::unique_ptr<Curve>>& getCurves() const
	{
		return curves;
	}

	void moveCursor(float xDiff, float yDiff)
	{
		cursor->translate(getTranslationFromMouse(xDiff, yDiff));
	}

	void moveObjects(float xDiff, float yDiff)
	{
		for (auto&& torus : selectedTori)
		{
			torus->translate(getTranslationFromMouse(xDiff, yDiff));
		}

		if (selectedPoints.size() > 0)
		{
			for (auto&& point : selectedPoints)
			{
				point->translate(getTranslationFromMouse(xDiff, yDiff));
			}
			pointRenderer.update(points);
		}

		if (selectedVirtualPoint)
		{
			selectedVirtualPoint->translate(getTranslationFromMouse(xDiff, yDiff));
		}

		MiddlePoint::getInstance().calculateMiddlePoint(selectedTori, selectedPoints);
		pointRenderer.update(points);
	}

	void rotateObjects(float xDiff, float yDiff)
	{
		float angleRadians = std::sqrtf(xDiff * xDiff + yDiff * yDiff);
		auto axis = glm::normalize(camera.getUp() * xDiff + camera.getRight() * yDiff);
		auto count = selectedTori.size() + selectedPoints.size();
		if (count == 1) // local
		{
			if (selectedTori.size() > 0)
				selectedTori[0]->rotateLocal(angleRadians, axis);
			else
				selectedPoints[0]->rotateLocal(angleRadians, axis);
		}
		else // global
		{
			for (auto&& torus : selectedTori)
			{
				torus->rotateGlobal(angleRadians, axis, MiddlePoint::getInstance().getPosition());
			}
			if (selectedPoints.size() > 0)
			{
				for (auto&& point : selectedPoints)
				{
					point->rotateGlobal(angleRadians, axis, MiddlePoint::getInstance().getPosition());
				}
				pointRenderer.update(points);
			}
		}

		MiddlePoint::getInstance().calculateMiddlePoint(selectedTori, selectedPoints);
		pointRenderer.update(points);
	}

	void scaleObjects(float xDiff, float yDiff)
	{
		float coeff = 0.5f * std::sqrtf(xDiff * xDiff + yDiff * yDiff);
		auto direction = coeff * glm::normalize(camera.getRight() * xDiff + camera.getUp() * yDiff);
		auto count = selectedTori.size() + selectedPoints.size();
		if (count == 1) // local
		{
			if (selectedTori.size() > 0)
				selectedTori[0]->scaleLocal(direction);
			else
				selectedPoints[0]->scaleLocal(direction);
		}
		else // global
		{
			for (auto&& torus : selectedTori)
			{
				torus->scaleGlobal(direction, MiddlePoint::getInstance().getPosition());
			}
			if (selectedPoints.size() > 0)
			{
				for (auto&& point : selectedPoints)
				{
					point->scaleGlobal(direction, MiddlePoint::getInstance().getPosition());
				}
				pointRenderer.update(points);
			}
		}

		MiddlePoint::getInstance().calculateMiddlePoint(selectedTori, selectedPoints);
		pointRenderer.update(points);
	}

	void addTorus()
	{
		tori.emplace_back(new Torus(cursor->getPosition()));
	}

	void removeObjects()
	{
		// Curves need to be removed before points so that every object is deleted in the same frame
		for (auto&& curve : selectedCurves)
		{
			if (curve == selectedVirtualPointOwner)
			{
				// Clear selections attached to the removed curve
				selectedVirtualPointOwner = nullptr;
				selectedVirtualPoint = nullptr;
				break;
			}
		}
		selectedCurves.clear();
		std::erase_if(curves, [](auto&& curve) { return curve->isSelected; });

		// Tori
		selectedTori.clear();
		std::erase_if(tori, [](auto&& torus) { return torus->isSelected; });

		// Points
		std::erase_if(selectedPoints, [](auto&& point) { return !point->isInCurve(); });
		std::erase_if(points, [](auto&& point) { return point->isSelected && !point->isInCurve(); });
		pointRenderer.update(points);
	}

	void selectTorus(Torus* torus)
	{
		selectedTori.push_back(torus);
		MiddlePoint::getInstance().calculateMiddlePoint(selectedTori, selectedPoints);
		pointRenderer.update(points);
	}

	void deselectTorus(const Torus* torus)
	{
		selectedTori.erase(std::find(selectedTori.begin(), selectedTori.end(), torus));
		MiddlePoint::getInstance().calculateMiddlePoint(selectedTori, selectedPoints);
		pointRenderer.update(points);
	}

	void addPoint()
	{
		points.emplace_back(new Point(cursor->getPosition()));
		for (auto& curve : selectedCurves)
		{
			curve->addPoints((points.end() - 1)->get());
		}

		pointRenderer.update(points);
	}

	void selectPoint(Point* point)
	{
		selectedPoints.push_back(point);
		MiddlePoint::getInstance().calculateMiddlePoint(selectedTori, selectedPoints);
		pointRenderer.update(points);
	}

	void deselectPoint(const Point* point)
	{
		selectedPoints.erase(std::find(selectedPoints.begin(), selectedPoints.end(), point));
		MiddlePoint::getInstance().calculateMiddlePoint(selectedTori, selectedPoints);
		pointRenderer.update(points);
	}

	template <typename T>
	requires std::is_base_of<Curve, T>::value
	void addCurve()
	{
		curves.emplace_back(new T(selectedPoints));
	}

	void selectCurve(Curve* torus)
	{
		selectedCurves.push_back(torus);
	}

	void deselectCurve(const Curve* torus)
	{
		selectedCurves.erase(std::find(selectedCurves.begin(), selectedCurves.end(), torus));
	}

	void addToCurves()
	{
		for (auto&& curve : selectedCurves)
		{
			curve->addPoints(selectedPoints);
		}
	}

	void removeFromCurves()
	{
		for (auto&& curve : selectedCurves)
		{
			curve->removePoints();
		}
	}

	void selectObjectFromScreen(const glm::vec3& objcoord, bool selectMultiple)
	{
		// Check points
		for (auto&& point : points)
		{
			if (point->isCoordInObject(objcoord))
			{
				if (selectMultiple)
				{
					point->isSelected = !point->isSelected;
					if (point->isSelected)
						selectPoint(point.get());
					else
						deselectPoint(point.get());
				}
				else
				{
					// Deselect all
					deselectAll();

					point->isSelected = true;
					selectPoint(point.get());
				}
				MiddlePoint::getInstance().calculateMiddlePoint(selectedTori, selectedPoints);
				pointRenderer.update(points);
				return;
			}
		}
		// Check virtual points for curves
		for (auto&& curve : curves)
		{
			auto&& test = curve->getVirtualPoints();
			for (auto&& point : curve->getVirtualPoints())
			{	
				if (point->isCoordInObject(objcoord))
				{
					deselectAll();
					point->isSelected = true;
					selectedVirtualPoint = point.get();
					selectedVirtualPointOwner = curve.get();

					MiddlePoint::getInstance().calculateMiddlePoint(selectedTori, selectedPoints);
					pointRenderer.update(points);

					curve->updateRenderer();
					return;
				}
			}
		}

		// Check tori
		for (auto&& torus : tori)
		{
			if (torus->isCoordInObject(objcoord))
			{
				if (selectMultiple)
				{
					torus->isSelected = !torus->isSelected;
					if (torus->isSelected)
						selectTorus(torus.get());
					else
						deselectTorus(torus.get());
				}
				else
				{
					// Deselect all
					deselectAll();

					torus->isSelected = true;
					selectTorus(torus.get());
				}
				MiddlePoint::getInstance().calculateMiddlePoint(selectedTori, selectedPoints);
				pointRenderer.update(points);
				return;
			}
		}

		if (selectMultiple)
			return;
		// No point clicked => deselect all
		deselectAll();
		MiddlePoint::getInstance().calculateMiddlePoint(selectedTori, selectedPoints);
		pointRenderer.update(points);
	}

	void updateCurves()
	{
		for (auto&& curve : curves)
		{
			curve->update(); // returns immediately unless the curve needs to be recalculated;
		}
	}

private:
	const Camera& camera;
	PointRenderer& pointRenderer;
	const std::unique_ptr<SolidObject> grid = std::make_unique<Grid>();

	std::vector<std::unique_ptr<Torus>> tori;
	std::vector<std::unique_ptr<Point>> points;
	std::vector<std::unique_ptr<Curve>> curves;
	std::unique_ptr<SolidObject> cursor;

	std::vector<Torus*> selectedTori;
	std::vector<Point*> selectedPoints;
	std::vector<Curve*> selectedCurves;
	Curve* selectedVirtualPointOwner = nullptr;
	Point* selectedVirtualPoint = nullptr;

	inline glm::vec3 getTranslationFromMouse(float xDiff, float yDiff) const
	{
		return 0.25f / camera.getZoomScale().y * (xDiff * camera.getRight() - yDiff * camera.getUp());
	}

	void deselectAll()
	{
		selectedTori.clear();
		for (auto&& torus : tori)
		{
			torus->isSelected = false;
		}

		selectedPoints.clear();
		for (auto&& point : points)
		{
			point->isSelected = false;
		}

		selectedVirtualPointOwner = nullptr;
		selectedVirtualPoint = nullptr;
		for (auto&& curve : curves)
		{
			for (auto&& point : curve->getVirtualPoints())
			{
				point->isSelected = false;
			}
			curve->updateRenderer();
		}
	}
};