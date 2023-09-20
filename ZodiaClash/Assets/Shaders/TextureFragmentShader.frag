#version 450 core

layout (location=1) in vec3 vColor;
layout (location=2) in vec2 vTex;

layout (location=0) out vec4 fFragColor;

uniform sampler2D uTex2d;
uniform vec4 uColor;

void main () {
fFragColor = texture(uTex2d,vTex) * vec4(vColor, 1.0) * uColor;
}