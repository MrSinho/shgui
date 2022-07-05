#version 460

vec4 canvas[6] = vec4[6](
	vec4(-1.0f,-1.0f, 0.0f, 1.0f),
	vec4( 1.0f,-1.0f, 0.0f, 1.0f),
	vec4( 1.0f, 1.0f, 0.0f, 1.0f),
	vec4(-1.0f,-1.0f, 0.0f, 1.0f),
	vec4(-1.0f, 1.0f, 0.0f, 1.0f),
	vec4( 1.0f,-1.0f, 0.0f, 1.0f)
);

struct GuiItem {
    vec4 size_position;
};

layout (std140, set = 0, binding = 0) buffer guiItemsBuffer {
    GuiItem items[];
} storageItems;

void main() {

    uint item_index = gl_VertexIndex / 6;

    gl_Position = canvas[gl_VertexIndex % 6];

}