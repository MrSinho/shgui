#version 460

layout (location = 0) in vec3 vertex_position;



layout (push_constant) uniform pushConstant {
	mat4 projection;
	vec2 window_size;
} pconst;



#define PIXEL_SCALE_FACTOR (1.0f / 8.0f)
#define CHAR_SCALE_FACTOR  (1.0f / 4.0f)
#define TEXT_PIXEL_COUNT  (7 * 7)

struct ShGuiCharRaw {
	vec2  position;
	float scale;
	float priority;
	vec4  color;
};

layout (std430, set = 0, binding = 0) buffer _char_infos {// storage buffer
	ShGuiCharRaw chars_raw[];
} char_infos;



layout (location = 0) out vec4 char_color;



void main() {

	ShGuiCharRaw char_raw = char_infos.chars_raw[gl_VertexIndex / TEXT_PIXEL_COUNT];

	char_color = char_raw.color;

	gl_PointSize = char_raw.scale.x * PIXEL_SCALE_FACTOR;

	gl_Position = vec4(
		(vec2(vertex_position.x, -vertex_position.y) * char_raw.scale.x * CHAR_SCALE_FACTOR + vec2(char_raw.position.x, -char_raw.position.y) * 1.0f) / pconst.window_size, 
		vertex_position.z + char_raw.priority, 
		1.0f
	);
}