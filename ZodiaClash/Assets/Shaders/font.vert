#version 450 core

layout (location=0) in vec2 aVertexPosition;
layout (location=1) in vec3 aVertexColor;
layout (location=2) in vec2 aTexturePosition;
layout (location=3) in float aIndex;

layout (location=0) out vec3 vColor;
layout (location=1) out vec2 vTex;
layout (location=2) out float vIndex;

void main() {
gl_Position = vec4(vec2(aVertexPosition),0.0,1.0);
vColor = aVertexColor;
vTex = aTexturePosition;
vIndex = aIndex;
}
