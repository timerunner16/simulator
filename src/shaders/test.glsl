@module testmod

@vs vs
layout(binding=0) uniform vs_params {
	mat4 mvp;
};
layout(location=0) in vec3 position;
layout(location=1) in vec3 normal;
layout(location=2) in vec4 color0;
layout(location=3) in vec2 texcoord0;

out vec4 color;

void main() {
    gl_Position = mvp * vec4(position, 1.0);
	color = color0;
}
@end

@fs fs
layout(binding = 1) uniform fs_params {
	vec4 color_mod;
};
in vec4 color;
out vec4 frag_color;

void main() {
    frag_color = mix(color, 1-color, color_mod);
}
@end

@program test vs fs
