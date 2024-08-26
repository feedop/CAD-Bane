#version 420 core

layout(isolines) in;

uniform mat4 view;
uniform mat4 projection;
uniform int reverse;
uniform float segmentCount;

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

    vec3 p0 = gl_in[0].gl_Position.xyz;
    vec3 p1 = gl_in[1].gl_Position.xyz;
    vec3 p2 = gl_in[2].gl_Position.xyz;
    vec3 p3 = gl_in[3].gl_Position.xyz;

    vec3 e0 = gl_in[4].gl_Position.xyz;
    vec3 e1 = gl_in[5].gl_Position.xyz;
    vec3 e2 = gl_in[6].gl_Position.xyz;
    vec3 e3 = gl_in[7].gl_Position.xyz;
    vec3 e4 = gl_in[8].gl_Position.xyz;
    vec3 e5 = gl_in[9].gl_Position.xyz;
    vec3 e6 = gl_in[10].gl_Position.xyz;
    vec3 e7 = gl_in[11].gl_Position.xyz;

    vec3 f0 = gl_in[12].gl_Position.xyz;
    vec3 f1 = gl_in[13].gl_Position.xyz;
    vec3 f2 = gl_in[14].gl_Position.xyz;
    vec3 f3 = gl_in[15].gl_Position.xyz;
    vec3 f4 = gl_in[16].gl_Position.xyz;
    vec3 f5 = gl_in[17].gl_Position.xyz;
    vec3 f6 = gl_in[18].gl_Position.xyz;
    vec3 f7 = gl_in[19].gl_Position.xyz;

    float v = (reverse == 0) ? z : x;
    float u = (reverse == 0) ? x : z;

    //vec3 F0 = (u * f0 + v * f7) / (0.0001 + u + v);//og
    vec3 F0 = (u * f0 + v * f7) / (0.0001 + u + v);

    vec3 F1 = ((1 - u) * f1 + v * f2) / (1.0001 - u + v);//og

    vec3 F2 = ((1 - u) * f4 + (1 - v) * f3) / (2.0001 - u - v);//og
    //vec3 F2 = (u * f4 + v * f3) / (0.0001 + u + v);

    vec3 F3 = (u * f5 + (1 - v) * f6) / (1.0001 + u - v);//og
    //vec3 F3 = ((1 - u) * f6 + v * f5) / (1.0001 - u + v);

    vec3 Patch[16] = vec3[](
    p0, e7, e6, p3,
    e0, F0, F3, e5,
    e1, F1, F2, e4,
    p1, e2, e3, p2
    );

    if (reverse == 0)
    {
        for (int i = 0; i < 4; i++)
        {
            int offset = i * 4;

            bernsteins[i] = deCasteljau(
                Patch[offset],
                Patch[offset + 1],
                Patch[offset + 2],
                Patch[offset + 3],
                z
            );
        }
    }
    else
    {
        for (int i = 0; i < 4; i++)
        {
            bernsteins[i] = deCasteljau(
                Patch[i],
                Patch[4 + i],
                Patch[8 + i],
                Patch[12 + i],
                z
            );
        }
    }
    vec3 final = deCasteljau(bernsteins[0], bernsteins[1], bernsteins[2], bernsteins[3], x);

    gl_Position = projection * view * vec4(final, 1.0f);
}