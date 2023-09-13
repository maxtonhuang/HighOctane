#version 450 core

layout (location=0) in vec2 aVertexPosition;
layout (location=1) in vec3 aVertexColor;
layout (location=2) in vec2 aTexturePosition;

layout (location=1) out vec3 vColor;
layout (location=2) out vec2 vTex;

uniform mat3 uModelToNDC;
uniform mat3 uTexCoord;

void main() {
gl_Position = vec4(vec2(uModelToNDC * vec3(aVertexPosition, 1.f)),0.0,1.0);
vColor = aVertexColor;
vTex = vec2(uTexCoord * vec3(aTexturePosition, 1.f));
}
