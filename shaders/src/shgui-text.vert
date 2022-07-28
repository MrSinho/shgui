#version 460



layout (location = 0) in vec3 vertex_position;



layout (push_constant) uniform pushConstant {
	mat4 projection;
	vec2 window_size;//the application window
} pconst;



#define TEXT_SCALE_MULTIPLIER 2.8


layout (set = 0, binding = 0) uniform textUniform {
	vec4 position_scale;
} utext;



void main() {
	gl_PointSize = utext.position_scale.z * TEXT_SCALE_MULTIPLIER;
	gl_Position = pconst.projection * vec4(
		utext.position_scale.xy + vertex_position.xy * utext.position_scale.z, 
		0.0f, 1.0f
		);
	//gl_Position = pconst.projection * vec4(
	//	vertex_position.xyz, 1.0f
	//);
}