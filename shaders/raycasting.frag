#version 420 core

uniform int lightM;
uniform int width;
uniform int height;
uniform vec3 ellipsoidColor;
uniform mat4 Dm;

out vec4 FragColor;

bool closer(float x, float y, float z1, float z2)
{
	if (z1 == z2)
		return true;

	return length(vec3(x , y, z1)) < length(vec3(x, y, z2));
}

float specular(vec3 pos)
{
	vec3 toCamera = normalize(vec3(0, 0, 10) - pos);

	float dx = Dm[0][3] + Dm[3][0] + 2 * Dm[0][0] * pos.x + Dm[0][1] * pos.y + Dm[1][0] * pos.y + Dm[0][2] * pos.z + Dm[2][0] * pos.z;
	float dy = Dm[1][3] + Dm[3][1] + Dm[0][1] * pos.x + Dm[1][0] * pos.x + 2 * Dm[1][1] * pos.y + Dm[1][2] * pos.z + Dm[2][1] * pos.z;
	float dz = Dm[2][3] + Dm[3][2] + Dm[0][2] * pos.x + Dm[2][0] * pos.x + Dm[1][2] * pos.y + Dm[2][1] * pos.y + 2 * Dm[2][2] * pos.z;

	vec3 normal = normalize(vec3(dx, dy, dz));
	return pow(max(dot(toCamera, normal), 0.0f), lightM);
}

void main()
{
	float y = -(gl_FragCoord.x / width * 2 - 1.0f);
	float x = (gl_FragCoord.y / height * 2 - 1.0f);

	// Raycasting
	float a = Dm[2][2];
	float b = Dm[3][2] + Dm[2][3] + x * (Dm[0][2] + Dm[2][0]) + y * (Dm[1][2] + Dm[2][1]);
	float c = Dm[3][3] + x * (Dm[3][0] + Dm[0][3] + x * Dm[0][0] + y * Dm[0][1]) + y * (Dm[3][1] + Dm[1][3] + x * Dm[1][0] + y * Dm[1][1]);

	// Check quadratic equation solutions
	float delta = b * b - 4 * a * c;
	if (delta > 0)
	{
		float deltaRoot = sqrt(delta);
		float z1 = (-b + deltaRoot) / (2 * a);
		float z2 = (-b - deltaRoot) / (2 * a);
		float z = closer(x, y, z1, z2) ? z1 : z2;
		vec3 pixelPosition = vec3(x, y, z);

		FragColor =vec4(ellipsoidColor * (specular(pixelPosition)), 1.0f);
	}
	else
		FragColor = vec4( 0.0f, 0.0f, 0.0f, 0.0f );
}