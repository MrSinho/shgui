#version 460

vec4 canvas[6] = vec4[6](
	vec4(-1.0f,-1.0f, 0.0f, 1.0f),
	vec4( 1.0f, 1.0f, 0.0f, 1.0f),
	vec4(-1.0f, 1.0f, 0.0f, 1.0f),

	vec4( 1.0f, 1.0f, 0.0f, 1.0f),
	vec4(-1.0f,-1.0f, 0.0f, 1.0f),
	vec4( 1.0f,-1.0f, 0.0f, 1.0f)
);



layout (push_constant) uniform pushConstant {
	mat4 projection;
	vec2 window_size;
} pconst;



struct ShGuiRegionRaw {
    vec2  position;
    vec2  scale;
	vec4  color;
	vec3  edge_color;
	float priority;
};

layout (std430, set = 0, binding = 0) buffer _region_infos {
    ShGuiRegionRaw regions_raw[];
} region_infos;



layout (location = 0) out vec4 pixel_position;
layout (location = 1) out vec2 region_position;
layout (location = 2) out vec2 region_scale;
layout (location = 3) out vec4 region_color;
layout (location = 4) out vec4 region_edge_color;
layout (location = 5) out vec2 window_size;



void main() {

    uint region_index = gl_VertexIndex / 6;

	ShGuiRegionRaw region_raw = region_infos.regions_raw[region_index];


    vec4 vertex = canvas[gl_VertexIndex % 6];

	vertex.x *= region_raw.scale.x / pconst.window_size.x;
	vertex.y *= region_raw.scale.y / pconst.window_size.y;

	vertex.x += region_raw.position.x / pconst.window_size.x * 2.0f;
	vertex.y += region_raw.position.y / pconst.window_size.y * 2.0f;

	vertex.y *= -1;

	pixel_position    = vec4((region_raw.position) + canvas[gl_VertexIndex % 6].xy * region_raw.scale.xy, 0.0f, 1.0f);
	region_position   = vec2(region_raw.position);
	region_scale      = vec2(region_raw.scale);
	window_size       = vec2(pconst.window_size);
	
	region_color      = vec4(region_raw.color.xyz, 1.0f);
	region_edge_color = vec4(region_raw.edge_color.xyz, 1.0f);

	gl_Position = pconst.projection * vec4(vertex.xy, region_raw.priority, 1.0f);
}