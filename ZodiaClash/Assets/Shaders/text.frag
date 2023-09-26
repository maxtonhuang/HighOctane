#version 450 core
in vec2 TexCoords;
out vec4 fFragColor;

uniform sampler2D text;

void main()
{    
    fFragColor = vec4(1.0, 1.0, 1.0, texture(text, TexCoords).r);
}