#version 420 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;

layout (location = 0) out vec3 color;

uniform mat4 view;
uniform mat4 projection;
uniform vec3 cameraPosition;

void main()
{
    gl_Position = projection * view * vec4(aPos, 1.0);

    float scale = length(aPos - cameraPosition);
    gl_PointSize = 40 / scale;
    color = aColor;
}