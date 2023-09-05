#version 450 core

layout (location=1) in vec3 vColor;
layout (location=2) in vec2 vTex;

layout (location=0) out vec4 fFragColor;

uniform vec3 uColor;

void main () {
fFragColor = vec4(vColor, 1.0) * vec4(uColor,1.0);
}
