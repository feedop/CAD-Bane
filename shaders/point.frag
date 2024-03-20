#version 420 core

layout (location = 0) in vec3 color;

out vec4 FragColor;

void main()
{
    vec2 circCoord = 2.0 * gl_PointCoord - 1.0;
    if (dot(circCoord, circCoord) > 1.0) {
        discard;
    }
    FragColor = vec4(color, 1.0f);
}