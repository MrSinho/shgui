#version 460



layout (location = 0) in vec3 vertex_position;



layout (push_constant) uniform pushConstant {
	mat4 projection;
	vec2 window_size;//the application window
} pconst;



#define TEXT_SCALE_MULTIPLIER 80


layout (std140, set = 0, binding = 0) uniform textUniform {// dynamic uniform
	vec4 position_scale;
} utext;



void main() {
	gl_PointSize = utext.position_scale.z * TEXT_SCALE_MULTIPLIER / length(pconst.window_size * 3.5f) + 1.0f;
	gl_Position = pconst.projection * vec4(
		((vertex_position.xy * utext.position_scale.z) + utext.position_scale.xy * 2.0f) / pconst.window_size, 
		0.0f, 1.0f
		);
	//gl_Position = pconst.projection * vec4(
	//	vertex_position.xyz, 1.0f
	//);
}