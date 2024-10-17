#version 420 core

layout(isolines) in;

out vec2 texCoord;

uniform mat4 view;
uniform mat4 projection;
uniform int reverse;
uniform float segmentCount;
uniform int knotCountX;
uniform int knotIndexX;
uniform int knotCountZ;
uniform int knotIndexZ;
uniform float texModifierX;
uniform float texModifierZ;

float[5] basisFunctions(int i, float t, int knotCount)
{
    float knotDist = 1.0f/knotCount;

    float N[5];
    float A[4];
    float B[4];

    N[1] = 1;
    for (int j = 1; j <= 3; j++)
    {
        A[j] = knotDist * (i + j) - t;
		B[j] = t - knotDist * (i + 1 - j);
		float saved = 0;
        for (int k = 1; k <= j; k++)
        {
            float term = N[k]/(A[k] + B[j + 1 - k]);
			N[k] = saved + A[k] * term; 
			saved = B[j + 1 - k] * term;
        }
        N[j + 1] = saved;
    }

    return N;
}

void main()
{
    float x0 = float(knotIndexX) / knotCountX;
    float x1 = float(knotIndexX + 1) / knotCountX;
    float z0 = float(knotIndexZ) / knotCountZ;
    float z1 = float(knotIndexZ + 1) / knotCountZ;
    float x = x0 + (x1 - x0) * gl_TessCoord.x;
    float z = z0 + (z1 - z0) * gl_TessCoord.y * segmentCount/(segmentCount - 1.0f);

    vec3 deBoors[4];

    float[5] Nz = basisFunctions(knotIndexZ, z, knotCountZ);
    float[5] Nx = basisFunctions(knotIndexX, x, knotCountX);

    if (reverse == 0)
    {
        for (int i = 0; i < 4; i++)
        {
            int offset = i * 4;

            deBoors[i] = 
                gl_in[offset].gl_Position.xyz * Nz[1] +
                gl_in[offset + 1].gl_Position.xyz * Nz[2] +
                gl_in[offset + 2].gl_Position.xyz * Nz[3] +
                gl_in[offset + 3].gl_Position.xyz * Nz[4];
        }
    }
    else
    {
        for (int i = 0; i < 4; i++)
        {
            deBoors[i] = 
                gl_in[i].gl_Position.xyz * Nz[1] +
                gl_in[4 + i].gl_Position.xyz * Nz[2] +
                gl_in[8 + i].gl_Position.xyz * Nz[3] +
                gl_in[12 + i].gl_Position.xyz * Nz[4];
        }
    }
    vec3 final = 
        deBoors[0] * Nx[1] +
        deBoors[1] * Nx[2] +
        deBoors[2] * Nx[3] +
        deBoors[3] * Nx[4];

    gl_Position = projection * view * vec4(final, 1.0f);
    if (reverse == 0)
    {
        x = float(knotIndexX - 3) / (knotCountZ - 6) + float(gl_TessCoord.y) / (knotCountZ - 6);
        z = float(knotIndexZ - 3) / (knotCountX - 6) + float(gl_TessCoord.x) / (knotCountX - 6);
    }
    else
    {
        x = float(knotIndexX - 3) / (knotCountZ - 6) + float(gl_TessCoord.x) / (knotCountZ - 6);
        z = float(knotIndexZ - 3) / (knotCountX - 6) + float(gl_TessCoord.y) / (knotCountX - 6);
    }
    
    texCoord = vec2(x, z);
}