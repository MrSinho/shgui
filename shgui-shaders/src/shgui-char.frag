#version 460

layout (location = 0) in  vec4 char_color;
layout (location = 0) out vec4 fragment_color;



void main() {
    fragment_color = vec4(char_color.xyz, 1.0f);
}