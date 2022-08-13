#version 460



layout (location = 0) in vec3 vertex_position;



layout (push_constant) uniform pushConstant {
	mat4 projection;
	vec2 window_size;//the application window
} pconst;



#define TEXT_SCALE_MULTIPLIER 1.0f


layout (std140, set = 0, binding = 0) uniform textUniform {// dynamic uniform
	vec2 position;
	vec2 scale;
	vec4 priority;
} utext;



void main() {
	gl_PointSize = utext.scale.x / 10.0f * TEXT_SCALE_MULTIPLIER;
	gl_Position = pconst.projection * vec4(
		((vertex_position.xy * utext.scale.x) + utext.position * 2.0f) / pconst.window_size, 
		0.0f, 1.0f
		);
	//gl_Position = pconst.projection * vec4(
	//	vertex_position.xyz, 1.0f
	//);
}