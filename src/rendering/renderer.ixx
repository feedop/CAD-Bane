export module renderer;

import std;
import glm;

import <glad/glad.h>;

import camera;
import canvas;
import colors;
import config;
import intersectioncurve;
import solidobject;
import scene;
import pointrenderer;
import surface;
import c0surface;
import c2surface;
import gregorysurface;
import raycaster;
import shader;
import glutils;
import math;
import depthbmp;


/// <summary>
/// Responsible for rendering the scene using OpenGL
/// </summary>
export class Renderer
{
public:
	friend class GuiController;

	Renderer(int windowWidth, int windowHeight, Camera& camera, PointRenderer& pointRenderer,
			Scene& scene, Raycaster& raycaster) :
		windowWidth(windowWidth), windowHeight(windowHeight),
		camera(camera), pointRenderer(pointRenderer), scene(scene), raycaster(raycaster)
	{
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_PROGRAM_POINT_SIZE);

		// Set surface shaders
		C0Surface::setPreferredShader(c0SurfaceShader.get());
		C2Surface::setPreferredShader(c2SurfaceShader.get());
		GregorySurface::setPreferredShader(gregoryShader.get());

		// First raycasting
		raycaster.setTextureSize(windowWidth, windowHeight);
	}

	/// <summary>
	/// Render the scene
	/// </summary>
	void draw() const
	{
		// Clear
		glViewport(0, 0, windowWidth, windowHeight);
		glClearColor(0.15f, 0.0f, 0.17f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Full screen canvas - intersection curve parametric view
		if (parametricViewCanvas)
		{
			flatTextureShader->use();
			parametricViewCanvas->draw(flatTextureShader.get());
			return;
		}

		// Stereographic rendering
		if (stereo)
		{
			camera.setForLeftEye();
			glColorMask(true, false, false, false);
			drawScene(camera.getRedProjection());
			glClear(GL_DEPTH_BUFFER_BIT);

			camera.setForRightEye();
			glColorMask(false, true, true, false);
			drawScene(camera.getBlueProjection());
			glColorMask(true, true, true, true);
			camera.setForCenter();
		}
		else
		{
			drawScene(camera.getProjection());
		}

		// Grid
		if (drawGrid)
		{
			gridShader->use();
			gridShader->setMatrix("view", camera.getView());
			gridShader->setMatrix("projection", camera.getProjection());
			scene.getGrid()->draw(gridShader.get());
		}

		glDisable(GL_DEPTH_TEST);

		// Cursor
		multiColorShader->use();
		multiColorShader->setMatrix("view", camera.getView());
		multiColorShader->setMatrix("projection", camera.getProjection());
		scene.getCursor()->draw(multiColorShader.get());

		// Raycasting
		raycaster.draw(flatTextureShader.get());
	}

	void setWindowSize(int width, int height)
	{
		windowWidth = width;
		windowHeight = height;
		glViewport(0, 0, windowWidth, windowHeight);
		raycaster.setTextureSize(windowWidth, windowHeight);
	}

	void selectObjectFromScreen(double x, double y, bool selectMultiple)
	{
		fillClickDepthBuffer();

		float depth;
		glReadPixels(static_cast<int>(x), static_cast<int>(windowHeight - y - 1), 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &depth);
		glm::vec4 viewport = glm::vec4(0, 0, windowWidth, windowHeight);
		glm::vec3 wincoord = glm::vec3(x, windowHeight - y - 1, depth);
		glm::vec3 objcoord = glm::unProject(wincoord, camera.getView(), camera.getProjection(), viewport);
		scene.selectObjectFromScreen(objcoord, selectMultiple);
	}

	const Canvas* getParametricViewCanvas() const
	{
		return parametricViewCanvas.get();
	}

	void setParametricViewCurve(IntersectionCurve* curve, bool firstSurface)
	{
		parametricViewCanvas = curve->getParametricViewCanvas(firstSurface);
	}

	void resetParametricViewCurve()
	{
		parametricViewCanvas.reset();
	}

	std::vector<float> getPathHeightMap(int size) const
	{
		static constexpr float scaleFactor = 1.0f / 7.5f;

		glEnable(GL_DEPTH_TEST);
		glClear(GL_DEPTH_BUFFER_BIT);
		ScopedDisableColorWrite disable;

		static constexpr float zNear = 0.00001f;
		static constexpr float zFar = 1.0f;
		auto linearizeDepth = [](float d)
		{
			float z_n = 2.0 * d - 1.0;
			return 2.0 * zNear * zFar / (zFar + zNear - z_n * (zFar - zNear));
		};

		// Surfaces	
		for (auto&& [shader, surfaces] : scene.getSurfaceTypes())
		{
			shader->use();
			shader->setMatrix("view", math::translate({ 0, 0, -1.0f }) * math::scale({ scaleFactor, scaleFactor, scaleFactor }) * math::translate({ 0, 0, -1.5f }));
			shader->setMatrix("projection", /*glm::perspective(200.0f,1.0f,zNear, zFar)*/ glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, zNear, zFar));
			for (auto&& surface : surfaces)
			{
				auto densityX = surface->getDensityX();
				auto densityZ = surface->getDensityZ();
				surface->setDensityX(64);
				surface->setDensityZ(64);
				surface->draw(shader);
				surface->setDensityX(densityX);
				surface->setDensityZ(densityZ);
			}
		}

		// Download the depth buffer data
		std::vector<float> depthData(size * size);;
		glReadPixels(0, 0, size, size, GL_DEPTH_COMPONENT, GL_FLOAT, depthData.data());
		std::transform(std::execution::par, depthData.begin(), depthData.end(), depthData.begin(), [&](auto d)
		{
			float linearDepth = linearizeDepth(d);
			return (1.0f - d) * 7.5f + 1.5f;
		});
		float min = *std::min_element(depthData.begin(), depthData.end());
		float max = *std::max_element(depthData.begin(), depthData.end());

		if constexpr (cfg::savePathTextures)
			saveDepthBmp(depthData, size, size, "paths/depth.bmp");

		return depthData;
	}

private:
	Camera& camera;
	Scene& scene;
	Raycaster& raycaster;
	PointRenderer& pointRenderer;

	std::unique_ptr<Shader> raycastingShader = std::make_unique<FlatTextureShader>();
	std::unique_ptr<Shader> flatTextureShader = std::make_unique<FlatTextureShader>();
	std::unique_ptr<Shader> gridShader = std::make_unique<InfiniteGridShader>();
	std::unique_ptr<Shader> uniformColorShader = std::make_unique<UniformColorShader>();
	std::unique_ptr<Shader> multiColorShader = std::make_unique<MultiColorShader>();
	std::unique_ptr<Shader> pointShader = std::make_unique<PointShader>();
	std::unique_ptr<Shader> bezierCubicShader = std::make_unique<BezierCubicShader>();
	std::unique_ptr<Shader> bezierQuadraticShader = std::make_unique<BezierQuadraticShader>();
	std::unique_ptr<Shader> interpolatingSplineShader = std::make_unique<InterpolatingSplineShader>();
	std::unique_ptr<Shader> c0SurfaceShader = std::make_unique<C0SurfaceShader>();
	std::unique_ptr<Shader> c2SurfaceShader = std::make_unique<C2SurfaceShader>();
	std::unique_ptr<Shader> gregoryShader = std::make_unique<GregoryShader>();
	std::unique_ptr<Shader> torusShader = std::make_unique<TorusShader>();

	int windowWidth;
	int windowHeight;

	bool drawGrid = true;
	bool drawPolygons = false;

	bool stereo = false;

	std::shared_ptr<Canvas> parametricViewCanvas;

	void drawScene(const glm::mat4& projection) const
	{
		glEnable(GL_DEPTH_TEST);

		// Surfaces	
		for (auto&& [shader, surfaces] : scene.getSurfaceTypes())
		{
			shader->use();
			shader->setMatrix("view", camera.getView());
			shader->setMatrix("projection", projection);
			for (auto&& surface : surfaces)
			{
				surface->draw(shader);
			}
		}

		// Curves
		static constexpr float baseSegmentCount = 128.0f;
		// Interpolating curves

		interpolatingSplineShader->use();
		interpolatingSplineShader->setMatrix("view", camera.getView());
		interpolatingSplineShader->setMatrix("projection", projection);

		for (auto&& curve : scene.getInterpolatingCurves())
		{
			auto segmentCount = std::min(64.0f, baseSegmentCount / glm::length(camera.getPosition() - curve->getCenter()));
			interpolatingSplineShader->setFloat("segmentCount", segmentCount);
			curve->draw(interpolatingSplineShader.get());
		}

		// Approximating curves		
		auto approximatingCurves = scene.getApproximatingCurves();

		bezierCubicShader->use();
		bezierCubicShader->setMatrix("view", camera.getView());
		bezierCubicShader->setMatrix("projection", projection);
		for (auto&& curve : approximatingCurves)
		{
			auto segmentCount = std::min(64.0f, baseSegmentCount / glm::length(camera.getPosition() - curve->getCenter()));
			bezierCubicShader->setFloat("segmentCount", segmentCount);
			curve->draw(bezierCubicShader.get());
		}

		bezierQuadraticShader->use();
		bezierQuadraticShader->setMatrix("view", camera.getView());
		bezierQuadraticShader->setMatrix("projection", projection);

		for (auto&& curve : approximatingCurves)
		{
			auto segmentCount = std::min(64.0f, baseSegmentCount / glm::length(camera.getPosition() - curve->getCenter()));
			bezierQuadraticShader->setFloat("segmentCount", segmentCount);
			curve->drawQuadratic(bezierQuadraticShader.get());
		}

		uniformColorShader->use();
		uniformColorShader->setMatrix("model", glm::mat4{ 1.0f });
		uniformColorShader->setMatrix("view", camera.getView());
		uniformColorShader->setMatrix("projection", projection);

		for (auto&& curve : approximatingCurves)
		{
			curve->drawLines(uniformColorShader.get());
		}

		// Curve and surface polygons if enabled
		if (drawPolygons)
		{
			for (auto&& curve : scene.getCurves())
			{
				curve->drawPolygon(uniformColorShader.get());
			}

			for (auto&& surface : scene.getSurfaces())
			{
				surface->drawPolygon(uniformColorShader.get());
			}
		}

		// Tori
		torusShader->use();
		torusShader->setMatrix("view", camera.getView());
		torusShader->setMatrix("projection", projection);
		for (auto&& torus : scene.getTori())
		{
			torus->draw(torusShader.get());
		}

		// Points
		pointShader->use();
		pointShader->setMatrix("view", camera.getView());
		pointShader->setMatrix("projection", projection);
		pointShader->setVector("cameraPosition", camera.getPosition());
		pointRenderer.draw(pointShader.get());

		// Additional points from curves
		for (auto&& curve : scene.getCurves())
		{
			curve->drawAdditionalPoints(pointShader.get());
		}
	}

	// Fill depth buffer with only torus and point data without actually rendering
	void fillClickDepthBuffer()
	{
		glEnable(GL_DEPTH_TEST);
		glClear(GL_DEPTH_BUFFER_BIT);
		ScopedDisableColorWrite disable;

		// Tori
		uniformColorShader->use();
		uniformColorShader->setMatrix("view", camera.getView());
		uniformColorShader->setMatrix("projection", camera.getProjection());

		for (auto&& torus : scene.getTori())
		{
			torus->draw(uniformColorShader.get());
		}

		// Curves' virtual points
		pointShader->use();
		pointShader->setMatrix("view", camera.getView());
		pointShader->setMatrix("projection", camera.getProjection());
		for (auto&& curve : scene.getCurves())
		{
			curve->drawAdditionalPoints(pointShader.get());
		}

		// Points
		pointRenderer.draw(pointShader.get());
	}
};