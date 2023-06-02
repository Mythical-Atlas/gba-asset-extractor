#version 430 core

out vec4 color;

uniform vec4 palette[16];
uniform uint tile[16];

void main(void) {
    color = texture(image, texCoords + frame) * colorMult;
}