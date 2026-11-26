@module test

@vs vs
layout(binding=0) uniform vs_params {
	mat4 mvp;
};

layout(location=0) in vec3 position;
layout(location=1) in vec3 normal;
layout(location=2) in vec4 color0;
layout(location=3) in vec2 texcoord0;

out vec4 color;
out vec2 uv;

void main() {
    gl_Position = mvp * vec4(position, 1.0);
	color = color0;
	uv = texcoord0;
}
@end

@fs fs
layout(binding=0) uniform texture2D tex0;
layout(binding=0) uniform sampler smp0;

in vec4 color;
in vec2 uv;

out vec4 frag_color;

void main() {
    frag_color = texture(sampler2D(tex0, smp0), uv) * color;
}
@end

@program test vs fs
