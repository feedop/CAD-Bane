#version 450 core

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

layout(location = 0) out vec3 nearPoint;
layout(location = 1) out vec3 farPoint;

vec3 unprojectPoint(float x, float y, float z, mat4 mView, mat4 mProjection) {
    mat4 viewInv = inverse(mView);
    mat4 projInv = inverse(mProjection);
    vec4 unprojectedPoint =  viewInv * projInv * vec4(x, y, z, 1.0);
    return unprojectedPoint.xyz / unprojectedPoint.w;
}

void main() {
    nearPoint = unprojectPoint(aPos.x, aPos.y, 0.0, view, projection).xyz; // unprojecting on the near plane
    farPoint = unprojectPoint(aPos.x, aPos.y, 1.0, view, projection).xyz; // unprojecting on the far plane
    gl_Position = vec4(aPos, 1.0); // using directly the clipped coordinates
}
