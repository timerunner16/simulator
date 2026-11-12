@vs vs
layout(location=0) in vec3 position;
layout(location=1) in vec3 normal;
layout(location=2) in vec4 color0;
layout(location=3) in vec2 texcoord0;

out vec4 color;

void main() {
    gl_Position = vec4(position, 1.0);
	color = color0;
}
@end

@fs fs
in vec4 color;
out vec4 frag_color;

void main() {
    frag_color = color;
}
@end

@program test vs fs
