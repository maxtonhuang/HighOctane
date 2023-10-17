#version 450 core

layout (location=0) in vec4 vInterpColor;
layout (location=0) out vec4 fFragColor;

void main () {
fFragColor = vInterpColor;
}
