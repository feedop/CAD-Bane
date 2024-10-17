#version 420 core

out vec4 FragColor;
  
in vec2 texCoord;

uniform vec4 color;
uniform int trimMode;
uniform sampler2D tex;

void main()
{
    vec2 uv = vec2(texCoord.x, texCoord.y);
//    if (trimMode != 0)
//    {
//        FragColor = vec4(uv, 0.0, 1.0);
//        return;
//    }     
//
    if (trimMode == 1)
        discard;
    else if (trimMode == 2 && texture(tex, uv).x == 1.0f)
        discard;
    else if (trimMode == 3 && texture(tex, uv).x < 1.0f)
        discard;
    FragColor = color;
}