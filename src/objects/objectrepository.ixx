export module objectrepository;

import <cmath>;
import <memory>;
import <vector>;

import <glm/vec3.hpp>;
import <glm/gtc/constants.hpp>;

import camera;
import cube;
import cursor;
import grid;
import math;
import middlepoint;
import object;
import point;
import pointrenderer;
import torus;

export class ObjectRepository
{
public:
	ObjectRepository(const Camera& camera, PointRenderer& pointRenderer) : camera(camera), pointRenderer(pointRenderer)
	{
		// Cursor
		cursor = std::make_unique<Cursor>();
		// Initial objects
		tori.emplace_back(new Torus);
	}

	inline const Object* getGrid() const
	{
		return grid.get();
	}

	inline const Object* getCursor() const
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
		auto axis = coeff * glm::normalize(camera.getRight() * xDiff + camera.getUp() * yDiff);
		auto count = selectedTori.size() + selectedPoints.size();
		if (count == 1) // local
		{
			if (selectedTori.size() > 0)
				selectedTori[0]->scaleLocal(axis);
			else
				selectedPoints[0]->scaleLocal(axis);
		}
		else // global
		{
			axis += glm::vec3{ 1.0f, 1.0f, 1.0f };
			for (auto&& torus : selectedTori)
			{
				torus->scaleGlobal(axis, MiddlePoint::getInstance().getPosition());
			}
			if (selectedPoints.size() > 0)
			{
				for (auto&& point : selectedPoints)
				{
					point->scaleGlobal(axis, MiddlePoint::getInstance().getPosition());
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

	void removeTori()
	{
		selectedTori.clear();
		std::erase_if(tori, [](auto&& torus) { return torus->isSelected; });
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
		pointRenderer.update(points);
	}

	void removePoints()
	{
		selectedPoints.clear();
		std::erase_if(points, [](auto&& point) { return point->isSelected; });
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

	void selectObjectFromScreen(const glm::vec3& objcoord, bool selectMultiple)
	{
		// Check points
		for (auto& point : points)
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
		// Check tori
		for (auto& torus : tori)
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

private:
	const Camera& camera;
	PointRenderer& pointRenderer;
	const std::unique_ptr<Object> grid = std::make_unique<Grid>();

	std::vector<std::unique_ptr<Torus>> tori;
	std::vector<std::unique_ptr<Point>> points;
	std::unique_ptr<Object> cursor;

	std::vector<Torus*> selectedTori;
	std::vector<Point*> selectedPoints;

	inline glm::vec3 getTranslationFromMouse(float xDiff, float yDiff) const
	{
		return 0.25f / camera.getZoomScale().y * (xDiff * camera.getRight() - yDiff * camera.getUp());
	}

	void deselectAll()
	{
		selectedTori.clear();
		for (auto& torus : tori)
		{
			torus->isSelected = false;
		}

		selectedPoints.clear();
		for (auto& point : points)
		{
			point->isSelected = false;
		}
	}
};