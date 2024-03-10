export module renderer;

import <glad/glad.h>;
import <vector>;
import <memory>;

import camera;
import objectrepository;
import raycaster;
import shader;


/// <summary>
/// Responsible for rendering the scene using OpenGL
/// </summary>
export class Renderer
{
public:
	friend class GuiController;

	Renderer(int windowWidth, int windowHeight, const Camera& camera, const ObjectRepository& repository, Raycaster& raycaster) :
		windowWidth(windowWidth), windowHeight(windowHeight),
		camera(camera), repository(repository), raycaster(raycaster)
	{
		glViewport(0, 0, windowWidth, windowHeight);

		glDisable(GL_DEPTH_TEST);
		//glEnable(GL_CULL_FACE);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		// First raycasting
		raycaster.setTextureSize(windowWidth, windowHeight);
	}

	/// <summary>
	/// Render the scene
	/// </summary>
	void draw() const
	{
		// Clear
		glClearColor(0.2f, 0.0f, 0.2f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		

		// Tori
		solidColorShader->use();
		solidColorShader->setMatrix("view", camera.getView());
		solidColorShader->setMatrix("projection", camera.getProjection());
		for (auto&& torus: repository.getTori())
		{
			torus->draw(solidColorShader.get());
		}

		// Grid
		if (drawGrid)
		{
			gridShader->use();
			gridShader->setMatrix("view", camera.getView());
			gridShader->setMatrix("projection", camera.getProjection());
			repository.getGrid()->draw(gridShader.get());
		}

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

private:
	const Camera& camera;
	const ObjectRepository& repository;
	Raycaster& raycaster;
	std::unique_ptr<Shader> raycastingShader = std::make_unique<FlatTextureShader>();
	std::unique_ptr<Shader> flatTextureShader = std::make_unique<FlatTextureShader>();
	std::unique_ptr<Shader> gridShader = std::make_unique<GridShader>();
	std::unique_ptr<Shader> solidColorShader = std::make_unique<SolidColorShader>();

	int windowWidth;
	int windowHeight;

	bool drawGrid = true;
};