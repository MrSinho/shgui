#version 460



layout (location = 0) in vec3 vertex_position;



layout (push_constant) uniform pushConstant {
	mat4 projection;
	vec2 window_size;//the application window
} pconst;



#define TEXT_SCALE_FACTOR 1.0f / 10.0f
#define TEXT_PIXEL_COUNT (7 * 7)

struct ShGuiCharInfo {
	vec2 position;
	vec2 scale;
	vec4 priority;
};

layout (std140, set = 0, binding = 0) buffer _char_infos {// storage buffer
	ShGuiCharInfo char_infos[];
} char_infos_buffer;



void main() {

	ShGuiCharInfo char_info = char_infos_buffer.char_infos[gl_VertexIndex / TEXT_PIXEL_COUNT];

	gl_PointSize = char_info.scale.x * TEXT_SCALE_FACTOR;

	gl_Position = vec4(
		((vertex_position.xy * char_info.scale.x) + char_info.position * 2.0f) / pconst.window_size, 
		vertex_position.z + char_info.priority.x, 
		1.0f
	);
}