#version 420 core

out vec3 pos;

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    vec4 transformedPos = projection * view * model * vec4(aPos, 1.0);
    pos = aPos;
    gl_Position = transformedPos;
    
}