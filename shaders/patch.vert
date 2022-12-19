#version 410
// Vertex shader

layout(location = 0) in vec3 vertcoords;
layout(location = 1) in vec3 vertnormal;


layout(location = 0) out vec3 vertcoords_vs;
layout(location = 1) out vec3 vertnormals_vs;

void main() {

gl_Position = vec4(vertcoords_vs, 1.0);
 vertcoords_vs = vertcoords;
 vertnormals_vs = vertnormal;

}
