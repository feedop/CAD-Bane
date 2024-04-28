#version 420 core

layout(isolines) in;

uniform mat4 view;
uniform mat4 projection;
uniform float segmentCount;

uniform vec3 b;
uniform vec3 c;
uniform vec3 d;
uniform float dist;

void main()
{
    float segmentIndex = gl_TessCoord.y * segmentCount + gl_TessCoord.x;
	float t = segmentIndex / segmentCount;
    float tau = t * dist;

    vec3 a = gl_in[0].gl_Position.xyz;

    // Horner
    vec3 val = a + tau * (b + tau * (c + tau * d)); 

    gl_Position = projection * view * vec4(val, 1.0);
}