#version 460

vec4 canvas[6] = vec4[6](
	vec4(-1.0f,-1.0f, 0.0f, 1.0f),
	vec4( 1.0f, 1.0f, 0.0f, 1.0f),
	vec4(-1.0f, 1.0f, 0.0f, 1.0f),

	vec4( 1.0f, 1.0f, 0.0f, 1.0f),
	vec4(-1.0f,-1.0f, 0.0f, 1.0f),
	vec4( 1.0f,-1.0f, 0.0f, 1.0f)
);

struct GuiRegion {
    vec2 position;
    vec2 size;
	vec4 priority;
};

layout (std140, set = 0, binding = 0) buffer GuiRegionsBuffer {
    GuiRegion regions[];
} storageregions;

layout (push_constant) uniform pushConstant {
	mat4 projection;
	vec2 window_size;//the application window
} pconst;

layout (location = 0) out vec4 pixel_position;
layout (location = 1) out vec2 region_position;
layout (location = 2) out vec2 region_size;
layout (location = 3) out vec4 window_size;

void main() {

    uint region_index = gl_VertexIndex / 6;

    vec4 vertex = canvas[gl_VertexIndex % 6];
	vertex.x *= storageregions.regions[region_index].size.x / pconst.window_size.x;
	vertex.y *= storageregions.regions[region_index].size.y / pconst.window_size.y;

	vertex.x += storageregions.regions[region_index].position.x / pconst.window_size.x * 2.0f;
	vertex.y += storageregions.regions[region_index].position.y / pconst.window_size.y * 2.0f;

	//pixel_coords = vec4(storageregions.regions[region_index].position.xw, 0.0f, 1.0f);
	//region_size = vec4(storageregions.regions[region_index].size.xy, 0.0f, 1.0f);

	pixel_position 			= vec4((storageregions.regions[region_index].position) + canvas[gl_VertexIndex % 6].xy * storageregions.regions[region_index].size.xy, 0.0f, 1.0f);
	region_position 		= vec2(storageregions.regions[region_index].position);
	region_size		 		= vec2(storageregions.regions[region_index].size);
	window_size 			= vec4(pconst.window_size, 0.0f, 1.0f);
	
	gl_Position = pconst.projection * vec4(vertex.xy, storageregions.regions[region_index].priority.x, 1.0f);
}