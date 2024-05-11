#version 420 core

layout (vertices = 16) out;

uniform float segmentCount;

void main()
{
	
	gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;

	gl_TessLevelOuter[0] = segmentCount;
	gl_TessLevelOuter[1] = 64;
}