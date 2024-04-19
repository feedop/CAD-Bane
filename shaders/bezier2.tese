#version 420 core

layout(isolines) in;

uniform mat4 view;
uniform mat4 projection;
uniform float segmentCount;

void main()
{
	float segmentIndex = gl_TessCoord.y * segmentCount + gl_TessCoord.x;
	float t = segmentIndex / segmentCount;

    vec3 b00 = gl_in[0].gl_Position.xyz;
    vec3 b10 = gl_in[1].gl_Position.xyz;
    vec3 b20 = gl_in[2].gl_Position.xyz;

    // De Casteljau
    float u = (1.0 - t);

    vec3 b01 = b00 * u + b10 * t;
    vec3 b11 = b10 * u + b20 * t;

    vec3 b02 = b01 * u + b11 * t;

    gl_Position = projection * view * vec4(b02, 1.0);
}