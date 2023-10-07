#version 450 core

layout (location=0) in vec3 vColor;
layout (location=1) in vec2 vTex;
layout (location=2) in float vIndex;

layout (location=0) out vec4 fFragColor;

uniform sampler2D uTex2d;

void main () {
fFragColor = vec4(1.0,1.0,1.0,texture2D(uTex2d,vTex).r);
}
