#version 460

layout (location = 0) in vec4 pixel_position;
flat layout (location = 1) in vec4 region_position_size;
layout (location = 2) in vec4 window_size;

layout (location = 0) out vec4 fragment_color;

layout (std140, set = 1, binding = 0) uniform uniformColor {
    vec4 color;
} ucolor;

void main() {


    if (ucolor.color.x == 0.05f) {//dark
        fragment_color = vec4((0.065f + ucolor.color.xyz) * (gl_FragCoord.x / window_size.x + 1.0f), 1.0f);
    }
    else {//light
        fragment_color = vec4((0.05 + ucolor.color.xyz) / (gl_FragCoord.x / window_size.x + 1.0f), 1.0f);
    }


    float dist_x = distance(pixel_position.x, region_position_size.x);
    float dist_y = distance(pixel_position.y, region_position_size.y);

    if (
            dist_x >= region_position_size.z - 3.0f && dist_x <= region_position_size.z ||
            dist_y >= region_position_size.w - 3.0f && dist_y <= region_position_size.w
        ) {
        
        if (ucolor.color.x == 0.05f) {//dark
            fragment_color = vec4(vec3(0.7f), 1.0f);
        }
        else {//light
            fragment_color = vec4(vec3(0.4f), 1.0f);
        }

    }

}