#version 460

layout (location = 0) in vec4 pixel_position;
layout (location = 1) in vec2 region_position;
layout (location = 2) in vec2 region_scale;
layout (location = 3) in vec4 region_color;
layout (location = 4) in vec4 region_edge_color;
layout (location = 5) in vec2 window_size;

layout (location = 0) out vec4 fragment_color;



void main() {

    fragment_color = vec4(region_color.xyz, 1.0f);

    float dist_x = distance(pixel_position.x, region_position.x);
    float dist_y = distance(pixel_position.y, region_position.y);

    if (
            dist_x >= region_scale.x - 3.0f && dist_x <= region_scale.x ||
            dist_y >= region_scale.y - 3.0f && dist_y <= region_scale.y
        ) {

        fragment_color = vec4(region_edge_color.xyz, 1.0f);
    }

}