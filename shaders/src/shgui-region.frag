#version 460

layout (location = 0) out vec4 fragment_color;

layout (std140, set = 1, binding = 0) uniform uniformColor {
    vec4 color;
} ucolor;

void main() {
    fragment_color = vec4(ucolor.color.xyz, 1.0f);
}