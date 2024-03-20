export module renderer;

import <glad/glad.h>;
import <vector>;
import <memory>;

import <glm/vec3.hpp>;
import <glm/vec4.hpp>;
import <glm/gtc/matrix_transform.hpp>;

import camera;
import objectrepository;
import pointrenderer;
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
			ObjectRepository& repository, Raycaster& raycaster) :
		windowWidth(windowWidth), windowHeight(windowHeight),
		camera(camera), pointRenderer(pointRenderer), repository(repository), raycaster(raycaster)
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

		// Tori
		uniformColorShader->use();
		uniformColorShader->setMatrix("view", camera.getView());
		uniformColorShader->setMatrix("projection", camera.getProjection());

		for (auto&& torus: repository.getTori())
		{
			torus->draw(uniformColorShader.get());
		}

		// Points
		pointShader->use();
		pointShader->setMatrix("view", camera.getView());
		pointShader->setMatrix("projection", camera.getProjection());
		pointShader->setVector("cameraPosition", camera.getPosition());
		pointRenderer.draw();
		
		// Grid
		if (drawGrid)
		{
			gridShader->use();
			gridShader->setMatrix("view", camera.getView());
			gridShader->setMatrix("projection", camera.getProjection());
			repository.getGrid()->draw(gridShader.get());
		}

		glDisable(GL_DEPTH_TEST);

		// Cursor
		multiColorShader->use();
		multiColorShader->setMatrix("view", camera.getView());
		multiColorShader->setMatrix("projection", camera.getProjection());
		repository.getCursor()->draw(multiColorShader.get());

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
		repository.selectObjectFromScreen(objcoord, selectMultiple);
	}

private:
	const Camera& camera;
	ObjectRepository& repository;
	Raycaster& raycaster;
	PointRenderer& pointRenderer;

	std::unique_ptr<Shader> raycastingShader = std::make_unique<FlatTextureShader>();
	std::unique_ptr<Shader> flatTextureShader = std::make_unique<FlatTextureShader>();
	std::unique_ptr<Shader> gridShader = std::make_unique<InfiniteGridShader>();
	std::unique_ptr<Shader> uniformColorShader = std::make_unique<UniformColorShader>();
	std::unique_ptr<Shader> multiColorShader = std::make_unique<MultiColorShader>();
	std::unique_ptr<Shader> pointShader = std::make_unique<PointShader>();

	int windowWidth;
	int windowHeight;

	bool drawGrid = true;

	// Fill depth buffer with only torus an point data without actually rendering
	void fillDepthBuffer()
	{
		glEnable(GL_DEPTH_TEST);
		glClear(GL_DEPTH_BUFFER_BIT);
		glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

		// Tori
		uniformColorShader->use();
		uniformColorShader->setMatrix("view", camera.getView());
		uniformColorShader->setMatrix("projection", camera.getProjection());

		for (auto&& torus : repository.getTori())
		{
			torus->draw(uniformColorShader.get());
		}

		// Points
		pointShader->use();
		pointShader->setMatrix("view", camera.getView());
		pointShader->setMatrix("projection", camera.getProjection());
		pointShader->setVector("cameraPosition", camera.getPosition());
		pointRenderer.draw();

		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	}
};