#version 420 core

layout(isolines) in;

out vec2 texCoord;

uniform mat4 view;
uniform mat4 projection;
uniform int reverse;
uniform float segmentCount;
uniform float patchIndexU;
uniform float patchIndexV;
uniform float patchCountU;
uniform float patchCountV;

vec3 deCasteljau(vec3 b00, vec3 b10, vec3 b20, vec3 b30, float t)
{
        float u = (1.0f - t);

        vec3 b01 = b00 * u + b10 * t;
        vec3 b11 = b10 * u + b20 * t;
        vec3 b21 = b20 * u + b30 * t;

        vec3 b02 = b01 * u + b11 * t;
        vec3 b12 = b11 * u + b21 * t;

        return b02 * u + b12 * t;
}

void main()
{
    float x = gl_TessCoord.x;
    float z = gl_TessCoord.y * segmentCount/(segmentCount - 1.0f);
    vec3 bernsteins[4];

    if (reverse == 0)
    {
        for (int i = 0; i < 4; i++)
        {
            int offset = i * 4;

            bernsteins[i] = deCasteljau(
                gl_in[offset].gl_Position.xyz,
                gl_in[offset + 1].gl_Position.xyz,
                gl_in[offset + 2].gl_Position.xyz,
                gl_in[offset + 3].gl_Position.xyz,
                z
            );
        }
    }
    else
    {
        for (int i = 0; i < 4; i++)
        {
            bernsteins[i] = deCasteljau(
                gl_in[i].gl_Position.xyz,
                gl_in[4 + i].gl_Position.xyz,
                gl_in[8 + i].gl_Position.xyz,
                gl_in[12 + i].gl_Position.xyz,
                z
            );
        }
    }
    vec3 final = deCasteljau(bernsteins[0], bernsteins[1], bernsteins[2], bernsteins[3], x);

    gl_Position = projection * view * vec4(final, 1.0f);
    texCoord = reverse == 0 ? vec2(z, x) : vec2(x, z);
    texCoord.x = patchIndexU / patchCountU + texCoord.x / patchCountU;
    texCoord.y = patchIndexV / patchCountV + texCoord.y / patchCountV;
}