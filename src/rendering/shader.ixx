export module shader;

import <glad/glad.h>;


import <fstream>;
import <iostream>;
import <sstream>;
import <string>;
import <string_view>;
import <vector>;

import <glm/glm.hpp>;
import <glm/gtc/type_ptr.hpp>;


export std::string getShaderCode(const char* filePath)
{
    std::string shaderCode;
    std::ifstream shaderFile;
    // ensure ifstream objects can throw exceptions:
    shaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try
    {
        // open files
        shaderFile.open(filePath);
        std::stringstream vShaderStream, fShaderStream;
        // read file's buffer contents into streams
        vShaderStream << shaderFile.rdbuf();
        // close file handlers
        shaderFile.close();
        // convert stream into string
        shaderCode = vShaderStream.str();
    }
    catch (std::ifstream::failure const&)
    {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ, path: " << filePath << std::endl;
        exit(1);
    }
    return shaderCode;
}

export GLuint compileShader(GLuint type, const char* shaderCode)
{
    int success;
    char infoLog[512];

    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &shaderCode, NULL);
    glCompileShader(shader);
    // print compile errors if any
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::COMPILATION_FAILED\n" << infoLog << std::endl;
        exit(1);
    };
    return shader;
}

/// <summary>
/// A wrapper class for GLSL shaders. heavily inspired by https://learnopengl.com/Getting-started/Shaders
/// </summary>
export class Shader
{
public:
    // the program ID
    unsigned int id;
    // use/activate the shader
    virtual ~Shader() = default;

    inline void use() const
    {
        glUseProgram(id);
    }

    // utility uniform functions
    void setFloat(const char* name, float value) const
    {
        int location = glGetUniformLocation(id, name);
        glUniform1f(location, value);
    }

    void setInt(const char* name, int value) const
    {
        int location = glGetUniformLocation(id, name);
        glUniform1i(location, value);
    }

    void setVector(const char* name, const glm::vec3& vector) const
    {
        int location = glGetUniformLocation(id, name);
        glUniform3f(location, vector.x, vector.y, vector.z);
    }

    void setVector(const char* name, const glm::vec4& vector) const
    {
        int location = glGetUniformLocation(id, name);
        glUniform4f(location, vector.x, vector.y, vector.z, vector.w);
    }

    void setMatrix(const char* name, const glm::mat4& matrix) const
    {
        int location = glGetUniformLocation(id, name);
        glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
    }

protected:
    // constructor reads and builds the shader
    Shader(const char* vertexPath, const char* fragmentPath)
    {
        // 1. retrieve the vertex/fragment source code from filePath
        std::string vertexCode = getShaderCode(vertexPath);
        std::string fragmentCode = getShaderCode(fragmentPath);

        // 2. compile shaders
        GLuint vertex = compileShader(GL_VERTEX_SHADER, vertexCode.c_str());
        GLuint fragment = compileShader(GL_FRAGMENT_SHADER, fragmentCode.c_str());

        // 3. shader Program
        id = glCreateProgram();
        glAttachShader(id, vertex);
        glAttachShader(id, fragment);
        glLinkProgram(id);
        // print linking errors if any
        int success;
        char infoLog[512];

        glGetProgramiv(id, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(id, 512, NULL, infoLog);
            std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
        }

        // 4. delete the shaders as they're linked into our program now and no longer necessary
        glDeleteShader(vertex);
        glDeleteShader(fragment);
    }
};

export class FlatTextureShader : public Shader
{
public:
    FlatTextureShader() : Shader("shaders/flattexture.vert", "shaders/flattexture.frag") {}
};

export class GridShader : public Shader
{
public:
    GridShader() : Shader("shaders/grid.vert", "shaders/grid.frag") {}
};

export class RaycastingShader : public Shader
{
public:
    RaycastingShader() : Shader("shaders/raycasting.vert", "shaders/raycasting.frag") {}
};

export class SolidColorShader : public Shader
{
public:
    SolidColorShader() : Shader("shaders/solidcolor.vert", "shaders/solidcolor.frag") {}
};