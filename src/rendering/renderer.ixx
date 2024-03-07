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

		// Ground
		if (drawGrid)
		{
			groundShader->use();
			groundShader->setMatrix("view", camera.getView());
			groundShader->setMatrix("projection", camera.getProjection());
			repository.getGround()->draw(groundShader.get());
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
	std::unique_ptr<Shader> groundShader = std::make_unique<GroundShader>();

	int windowWidth;
	int windowHeight;

	bool drawGrid = true;
};