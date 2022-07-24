#version 460

vec4 canvas[6] = vec4[6](
	vec4(-1.0f,-1.0f, 0.0f, 1.0f),
	vec4( 1.0f, 1.0f, 0.0f, 1.0f),
	vec4(-1.0f, 1.0f, 0.0f, 1.0f),

	vec4( 1.0f, 1.0f, 0.0f, 1.0f),
	vec4(-1.0f,-1.0f, 0.0f, 1.0f),
	vec4( 1.0f,-1.0f, 0.0f, 1.0f)
);

struct GuiItem {
    vec4 size_position;
};

layout (std140, set = 0, binding = 0) buffer guiItemsBuffer {
    GuiItem items[];
} storageItems;

layout (push_constant) uniform pushConstant {
	mat4 projection;
	vec2 window_size;
} pconst;

void main() {

    uint item_index = gl_VertexIndex / 6;

    vec4 vertex = canvas[gl_VertexIndex % 6];
	vertex.x *= storageItems.items[item_index].size_position.x / pconst.window_size.x;
	vertex.y *= storageItems.items[item_index].size_position.y / pconst.window_size.y;

	vertex.x += storageItems.items[item_index].size_position.z / pconst.window_size.x * 2.0f;
	vertex.y += storageItems.items[item_index].size_position.w / pconst.window_size.y * 2.0f;

	gl_Position = pconst.projection * vertex;
}