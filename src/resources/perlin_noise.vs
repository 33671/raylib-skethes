#version 330 core

layout (location = 0) in vec2 position;
layout (location = 1) in vec2 texcoord;
uniform vec2 iResolution;

out vec2 TexCoord;

void main()
{
    vec2 ndc = (position / iResolution) * 2.0 - 1.0;    // Map x,y from [0, width/height] to [-1, 1]
    ndc.y = -ndc.y;                                     // Flip y-axis (screen y-down to NDC y-up)
    gl_Position = vec4(ndc, 0.0, 1.0);                  // Set position in clip space
    TexCoord = texcoord;                                // Pass texture coordinates (0 to 1)
}
