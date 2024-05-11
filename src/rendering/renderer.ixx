export module renderer;

import <glad/glad.h>;
import <vector>;
import <memory>;

import <glm/vec3.hpp>;
import <glm/vec4.hpp>;
import <glm/gtc/matrix_transform.hpp>;

import camera;
import solidobject;
import scene;
import pointrenderer;
import surface;
import raycaster;
import shader;


/// <summary>
/// Responsible for rendering the scene using OpenGL
/// </summary>
export class Renderer
{
public:
	friend class GuiController;

	Renderer(int windowWidth, int windowHeight, const Camera& camera, PointRenderer& pointRenderer,
			Scene& scene, Raycaster& raycaster) :
		windowWidth(windowWidth), windowHeight(windowHeight),
		camera(camera), pointRenderer(pointRenderer), scene(scene), raycaster(raycaster)
	{
		glViewport(0, 0, windowWidth, windowHeight);

		glEnable(GL_DEPTH_TEST);
		//glEnable(GL_CULL_FACE);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_PROGRAM_POINT_SIZE);

		// First raycasting
		raycaster.setTextureSize(windowWidth, windowHeight);
	}

	/// <summary>
	/// Render the scene
	/// </summary>
	void draw() const
	{
		// Clear
		glClearColor(0.15f, 0.0f, 0.17f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glEnable(GL_DEPTH_TEST);

		// Surfaces
		surfaceShader->use();
		surfaceShader->setMatrix("view", camera.getView());
		surfaceShader->setMatrix("projection", camera.getProjection());

		surfaceShader->setInt("reverse", false);
		for (auto&& surface : scene.getSurfaces())
		{
			surfaceShader->setFloat("segmentCount", surface->getDensityZ());
			surface->draw(surfaceShader.get());
		}

		surfaceShader->setInt("reverse", true);
		for (auto&& surface : scene.getSurfaces())
		{
			surfaceShader->setFloat("segmentCount", surface->getDensityX());
			surface->draw(surfaceShader.get());
		}

		// Curves
		static constexpr float baseSegmentCount = 128.0f;
		// Interpolating curves

		interpolatingSplineShader->use();
		interpolatingSplineShader->setMatrix("view", camera.getView());
		interpolatingSplineShader->setMatrix("projection", camera.getProjection());

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
		bezierCubicShader->setMatrix("projection", camera.getProjection());
		for (auto&& curve : approximatingCurves)
		{
			auto segmentCount = std::min(64.0f, baseSegmentCount / glm::length(camera.getPosition() - curve->getCenter()));
			bezierCubicShader->setFloat("segmentCount", segmentCount);
			curve->draw(bezierCubicShader.get());
		}

		bezierQuadraticShader->use();
		bezierQuadraticShader->setMatrix("view", camera.getView());
		bezierQuadraticShader->setMatrix("projection", camera.getProjection());
		
		for (auto&& curve : approximatingCurves)
		{
			auto segmentCount = std::min(64.0f, baseSegmentCount / glm::length(camera.getPosition() - curve->getCenter()));
			bezierQuadraticShader->setFloat("segmentCount", segmentCount);
			curve->drawQuadratic(bezierQuadraticShader.get());
		}

		uniformColorShader->use();
		uniformColorShader->setMatrix("model", glm::mat4{1.0f});
		uniformColorShader->setMatrix("view", camera.getView());
		uniformColorShader->setMatrix("projection", camera.getProjection());

		for (auto&& curve : approximatingCurves)
		{
			curve->drawLines(uniformColorShader.get());
		}

		// Tori

		for (auto&& torus: scene.getTori())
		{
			torus->draw(uniformColorShader.get());
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

		// Points
		pointShader->use();
		pointShader->setMatrix("view", camera.getView());
		pointShader->setMatrix("projection", camera.getProjection());
		pointShader->setVector("cameraPosition", camera.getPosition());
		pointRenderer.draw(pointShader.get());

		// Additional points from curves
		for (auto&& curve : scene.getCurves())
		{
			curve->drawAdditionalPoints(pointShader.get());
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
		fillDepthBuffer();

		float depth;
		glReadPixels(x, windowHeight - y - 1, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &depth);
		glm::vec4 viewport = glm::vec4(0, 0, windowWidth, windowHeight);
		glm::vec3 wincoord = glm::vec3(x, windowHeight - y - 1, depth);
		glm::vec3 objcoord = glm::unProject(wincoord, camera.getView(), camera.getProjection(), viewport);
		scene.selectObjectFromScreen(objcoord, selectMultiple);
	}

private:
	const Camera& camera;
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
	std::unique_ptr<Shader> surfaceShader = std::make_unique<SurfaceShader>();

	int windowWidth;
	int windowHeight;

	bool drawGrid = true;
	bool drawPolygons = false;

	// Fill depth buffer with only torus and point data without actually rendering
	void fillDepthBuffer()
	{
		glEnable(GL_DEPTH_TEST);
		glClear(GL_DEPTH_BUFFER_BIT);
		glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

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
		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	}
};