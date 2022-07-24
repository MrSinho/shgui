#version 460

layout (push_constant) uniform pushConstant {
	mat4 projection;
	vec2 window_size;//the application window
} pconst;

void main() {

	gl_Position = vec4(1.0f);
}