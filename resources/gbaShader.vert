#version 430 core

layout(location = 0) in vec2 size;

uniform uint layer;
uniform mat4 viewMat;
uniform mat4 modelMat;

void main(void) {
    gl_Position = viewMat * modelMat * vec4(size, 0, 1);
}