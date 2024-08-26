export module scene;

import <cmath>;
import <memory>;
import <ranges>;
import <unordered_map>;
import <vector>;

import <glm/vec3.hpp>;
import <glm/gtc/constants.hpp>;
import <Serializer/Serializer.h>;

import c0bezier;
import c2bezier;
import c0surface;
import c2surface;
import gregorysurface;
import camera;
import cube;
import cursor;
import curve;
import grid;
import holefinder;
import interpolatingspline;
import math;
import middlepoint;
import shader;
import shape;
import solidobject;
import surface;
import point;
import pointrenderer;
import torus;
import mg1utils;


export class Scene
{
public:
	void init()
	{
		addSurface<C0Surface>(1, 0.5f);
	}

	Scene(const Camera& camera, PointRenderer& pointRenderer) : camera(camera), pointRenderer(pointRenderer)
	{
		// Cursor
		cursor = std::make_unique<Cursor>();

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

	inline const std::vector<Surface*>& getSelectedSurfaces() const
	{
		return selectedSurfaces;
	}

	inline const std::vector<Point*>& getSelectedPoints() const
	{
		return selectedPoints;
	}

	inline const std::vector<std::unique_ptr<Point>>& getPoints() const
	{
		return points;
	}

	inline const auto& getCurves() const
	{
		return curves;
	}

	inline auto getApproximatingCurves() const
	{
		return std::ranges::views::filter(curves, [](auto& curve) { return !(curve->isInterpolating()); });
	}

	inline auto getInterpolatingCurves() const
	{
		return std::ranges::views::filter(curves, [](auto& curve) { return curve->isInterpolating(); });
	}

	inline const auto& getSurfaces() const
	{
		return surfaces;
	}

	inline const auto& getSurfaceTypes() const
	{
		return surfaceTypes;
	}

	void moveCursor(float xDiff, float yDiff)
	{
		cursor->translate(getTranslationFromMouse(xDiff, yDiff));
	}

	void moveObjects(float xDiff, float yDiff)
	{
		auto translation = getTranslationFromMouse(xDiff, yDiff);

		for (auto&& surface : selectedSurfaces)
		{
			surface->translate(translation);
		}

		for (auto&& torus : selectedTori)
		{
			torus->translate(translation);
		}

		if (selectedPoints.size() > 0)
		{
			for (auto&& point : selectedPoints)
			{
				point->translate(translation);
			}
			pointRenderer.update(points);
		}

		if (selectedVirtualPoint)
		{
			selectedVirtualPoint->translate(translation);
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

		// Surfaces
		for (auto&& surfaces : selectedSurfaces)
		{
			if (surfaces == selectedVirtualPointOwner)
			{
				// Clear selections attached to the removed curve
				selectedVirtualPointOwner = nullptr;
				selectedVirtualPoint = nullptr;
				break;
			}
		}
		selectedSurfaces.clear();
		std::erase_if(surfaces, [](auto&& surface) { return surface->isSelected; });
		updateSurfaceTypes();

		// Tori
		selectedTori.clear();
		std::erase_if(tori, [](auto&& torus) { return torus->isSelected; });

		// Points
		std::erase_if(selectedPoints, [](auto&& point) { return !point->isAttached(); });
		std::erase_if(points, [](auto&& point) { return point->isSelected && !point->isAttached(); });
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

	template <class T>
	requires std::is_base_of<Curve, T>::value
	void addCurve()
	{
		curves.emplace_back(new T(selectedPoints));
	}

	template <class T, class... Us>
	requires std::is_base_of<Surface, T>::value
	void addSurface(Us... args)
	{
		surfaces.emplace_back(new T(cursor->getPosition(), args...));
		for (auto&& point : surfaces[surfaces.size() - 1]->getPoints())
		{
			points.emplace_back(point);
		}
		pointRenderer.update(points);
		updateSurfaceTypes();
	}

	void addGregoryPatch(bool backGregory = false)
	{
		auto&& holes = math::findHole(selectedSurfaces);
		if (selectedSurfaces.size() == 1 && selectedSurfaces[0]->isCylinder() && holes.size() == 2)
		{
			if (!backGregory)
				surfaces.emplace_back(new GregorySurface(holes[0]));
			else
				surfaces.emplace_back(new GregorySurface(holes[1]));
			updateSurfaceTypes();
			return;
		}

		for (auto&& hole : holes)
		{
			surfaces.emplace_back(new GregorySurface(hole));
		}
		updateSurfaceTypes();
	}

	void selectCurve(Curve* curve)
	{
		selectedCurves.push_back(curve);
	}

	void selectSurface(Surface* surface)
	{
		selectedSurfaces.push_back(surface);
	}

	void deselectCurve(const Curve* curve)
	{
		selectedCurves.erase(std::find(selectedCurves.begin(), selectedCurves.end(), curve));
	}

	void deselectSurface(const Surface* surface)
	{
		selectedSurfaces.erase(std::find(selectedSurfaces.begin(), selectedSurfaces.end(), surface));
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

	void collapsePoints()
	{
		if (selectedPoints.size() != 2)
			return;

		auto ptr1 = selectedPoints[0];
		auto ptr2 = selectedPoints[1];

		//for (auto&& attachedTo1 : ptr1->getAttachmentList())
		//{
		//	if (ptr1->getAttachmentList().contains(attachedTo1))
		//	{
		//		// TODO - handle this case
		//		return;
		//	}
		//}

		points.emplace_back(new Point(0.5f * (ptr1->getPosition() + ptr2->getPosition())));

		for (auto&& attachment : ptr1->getAttachmentList())
		{
			attachment->collapsePoints(ptr1, ptr2, points[points.size() - 1].get());
			attachment->scheduleToUpdate();
		}

		for (auto&& attachment : ptr2->getAttachmentList())
		{
			attachment->collapsePoints(ptr2, ptr1, points[points.size() - 1].get());
			attachment->scheduleToUpdate();
		}

		std::erase_if(points, [](auto&& point) { return point->isSelected; });
		selectedPoints.clear();

		MiddlePoint::getInstance().calculateMiddlePoint(selectedTori, selectedPoints);
		pointRenderer.update(points);
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

	void updateObjects()
	{
		// returns immediately unless the curve needs to be recalculated;
		for (auto&& curve : curves)
		{
			curve->update();
		}

		for (auto&& surface : surfaces)
		{
			surface->update();
		}
	}

	void clear()
	{
		curves.clear();
		interpolatingCurves.clear();
		surfaces.clear();
		surfaceTypes.clear();
		tori.clear();
		points.clear();
		
		selectedCurves.clear();
		selectedSurfaces.clear();
		selectedTori.clear();
		selectedPoints.clear();

		selectedVirtualPointOwner = nullptr;
		selectedVirtualPoint = nullptr;

		pointRenderer.update(points);
	}

	void deserialize(MG1::Scene& mgscene)
	{
		clear();

		for (auto&& point : mgscene.points)
		{
			points.emplace_back(new Point(point));
		}

		for (auto&& torus : mgscene.tori)
		{
			tori.emplace_back(new Torus(torus));
		}

		auto transformPoints = [&](const auto& controlPoints)
		{
			std::vector<Point*> newPoints;
			for (auto&& point : controlPoints)
			{
				newPoints.push_back(findPoint(points, point.GetId()));
			}

			return newPoints;
		};

		for (auto&& curve : mgscene.bezierC0)
		{
			curves.emplace_back(new C0Bezier(transformPoints(curve.controlPoints)));
			curves[curves.size() - 1]->setName(curve.name);
		}

		for (auto&& curve : mgscene.bezierC2)
		{
			curves.emplace_back(new C2Bezier(transformPoints(curve.controlPoints)));
			curves[curves.size() - 1]->setName(curve.name);
		}

		for (auto&& curve : mgscene.interpolatedC2)
		{
			curves.emplace_back(new InterpolatingSpline(transformPoints(curve.controlPoints)));
			curves[curves.size() - 1]->setName(curve.name);
		}

		for (auto&& surface : mgscene.surfacesC0)
		{
			transposeC0(surface);
			surfaces.emplace_back(new C0Surface(surface, points));
		}
		for (auto& surface : mgscene.surfacesC2)
		{
			if (surface.uWrapped)
				transposeSurface(surface);
			surfaces.emplace_back(new C2Surface(surface, points));
		}

		updateSurfaceTypes();
		pointRenderer.update(points);
	}

	void serialize(MG1::Scene& mgscene)
	{
		auto addCurveToMGScene = [&](const auto& curve)
		{
			std::vector<unsigned int> indices;
			for (auto&& point : curve->getPoints())
			{
				unsigned int found = 0;
				for (int i = 0; i < points.size(); i++)
				{
					if (points[i].get() == point)
					{
						found = i;
						break;
					}
				}
				indices.push_back(found);
			}

			curve->addToMGScene(mgscene, indices);
		};

		for (auto&& point : points)
		{
			mgscene.points.emplace_back(*point);
		}

		for (auto&& torus : tori)
		{
			mgscene.tori.emplace_back(*torus);
		}

		for (auto&& curve : curves)
		{
			addCurveToMGScene(curve);
		}
		for (auto&& curve : interpolatingCurves)
		{
			addCurveToMGScene(curve);
		}

		for (auto&& surface : surfaces)
		{
			surface->addToMGScene(mgscene, points);
		}
	}

private:
	const Camera& camera;
	PointRenderer& pointRenderer;
	const std::unique_ptr<SolidObject> grid = std::make_unique<Grid>();

	std::vector<std::unique_ptr<Torus>> tori;
	std::vector<std::unique_ptr<Point>> points;
	std::vector<std::unique_ptr<Curve>> curves;
	std::vector<std::unique_ptr<Curve>> interpolatingCurves;

	std::vector<std::unique_ptr<Surface>> surfaces;
	std::unordered_map<Shader*, std::vector<const Surface*>> surfaceTypes;

	std::unique_ptr<SolidObject> cursor;

	std::vector<Torus*> selectedTori;
	std::vector<Point*> selectedPoints;
	std::vector<Curve*> selectedCurves;
	std::vector<Surface*> selectedSurfaces;
	Shape* selectedVirtualPointOwner = nullptr;
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

	void updateSurfaceTypes()
	{
		surfaceTypes.clear();
		for (auto&& surface : surfaces)
		{
			auto shader = surface->getPreferredShader();
			if (surfaceTypes.contains(shader))
			{
				surfaceTypes[shader].push_back(surface.get());
			}
			else
			{
				surfaceTypes.insert({ shader, std::vector<const Surface*>{surface.get()} });
			}
		}
	}
};