export module drawable;

import <glad/glad.h>;

import glutils;
import shader;

/// <summary>
/// A base class that represents drawable objects. This class manages OpenGL resources such as Vertex Array Object (VAO),
/// Vertex Buffer Object (VBO), and Element Buffer Object (EBO). It provides functionality to clean up these resources
/// when the object is destroyed and a pure virtual function <see cref="draw"/> that should be implemented in derived classes 
/// to handle drawing the object.
/// </summary>
export class Drawable
{
public:
	/// <summary>
	/// Virtual destructor that cleans up OpenGL resources when a Drawable object is destroyed.
	/// It checks if the OpenGL resources (VAO, VBO, EBO) are allocated and deletes them properly.
	/// </summary>
	virtual ~Drawable()
	{
		if (VAO == 0)
			return;

		ScopedBindArray ba(VAO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glDeleteBuffers(1, &VBO);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glDeleteBuffers(1, &EBO);

		glDeleteVertexArrays(1, &VAO);
	}

	/// <summary>
	/// A pure virtual function that should be implemented by derived classes to draw the object using the provided shader.
	/// </summary>
	/// <param name="shader">The shader to be used for drawing the object.</param>
	virtual void draw(const Shader* shader) const = 0;

	/// <summary>
	/// Default constructor for the Drawable class. It initializes the OpenGL resource identifiers (VAO, VBO, EBO) to zero.
	/// </summary>
	Drawable() = default;

	/// <summary>
	/// Deleted copy constructor. This class cannot be copied to ensure proper management of OpenGL resources.
	/// </summary>
	Drawable(const Drawable& other) = delete;

	/// <summary>
	/// Deleted copy assignment operator. This class cannot be assigned to ensure proper management of OpenGL resources.
	/// </summary>
	Drawable& operator=(const Drawable& other) = delete;

protected:
	/// <summary>
	/// The identifier for the OpenGL Vertex Array Object (VAO).
	/// </summary>
	unsigned int VAO = 0;

	/// <summary>
	/// The identifier for the OpenGL Vertex Buffer Object (VBO).
	/// </summary>
	unsigned int VBO = 0;

	/// <summary>
	/// The identifier for the OpenGL Element Buffer Object (EBO).
	/// </summary>
	unsigned int EBO = 0;
};