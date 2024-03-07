#version 420 core

out vec4 FragColor;

in vec3 pos;

vec3 checker(in float u, in float v)
{
	float checkSize = 20;
	float fmodResult = mod(floor(checkSize * u) + floor(checkSize * v), 2.0);
	float fin = max(sign(fmodResult), 0.0);
	return vec3(fin, fin, fin);
}

void main(void)
{
	vec3 check = checker(pos.x, pos.y);
	FragColor = vec4(check, 0.3f);
}