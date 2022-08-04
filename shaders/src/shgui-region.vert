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
    vec4 size_position;
};

layout (std140, set = 0, binding = 0) buffer GuiRegionsBuffer {
    GuiRegion regions[];
} storageregions;

layout (push_constant) uniform pushConstant {
	mat4 projection;
	vec2 window_size;//the application window
} pconst;

layout (location = 0) out vec4 pixel_position;
layout (location = 1) out vec4 region_position_size;
layout (location = 2) out vec4 window_size;

void main() {

    uint region_index = gl_VertexIndex / 6;

    vec4 vertex = canvas[gl_VertexIndex % 6];
	vertex.x *= storageregions.regions[region_index].size_position.x / pconst.window_size.x;
	vertex.y *= storageregions.regions[region_index].size_position.y / pconst.window_size.y;

	vertex.x += storageregions.regions[region_index].size_position.z / pconst.window_size.x * 2.0f;
	vertex.y += storageregions.regions[region_index].size_position.w / pconst.window_size.y * 2.0f;

	//pixel_coords = vec4(storageregions.regions[region_index].size_position.zw, 0.0f, 1.0f);
	//region_size = vec4(storageregions.regions[region_index].size_position.xy, 0.0f, 1.0f);

	pixel_position 			= vec4((storageregions.regions[region_index].size_position.zw) + canvas[gl_VertexIndex % 6].xy * storageregions.regions[region_index].size_position.xy, 0.0f, 1.0f);
	region_position_size 	= vec4( storageregions.regions[region_index].size_position.zw, storageregions.regions[region_index].size_position.xy);
	window_size 			= vec4(pconst.window_size, 0.0f, 1.0f);
	
	gl_Position = pconst.projection * vertex;
}