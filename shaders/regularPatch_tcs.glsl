#version 410
// Tesselation Control Shader (TCS)
layout(vertices = 16) out;

layout(location = 0) in vec3[] vertcoords_vs;
layout(location = 1) in vec3[] vertnormals_vs;

layout(location = 0) out vec3[] vertcoords_tc;
layout(location = 1) out vec3[] vertnormals_tc;

// Outer tessellation level
uniform float outerTessLevel0;
uniform float outerTessLevel1;
uniform float outerTessLevel2;
uniform float outerTessLevel3;

// Inner tessellation level
uniform float innerTessLevel0;
uniform float innerTessLevel1;



void main() {
  if (gl_InvocationID == 0) {
    float tessLevel = 4.0;
    gl_TessLevelOuter[0] = outerTessLevel0;
    gl_TessLevelOuter[1] = outerTessLevel1;
    gl_TessLevelOuter[2] = outerTessLevel2;
    gl_TessLevelOuter[3] = outerTessLevel3;

    gl_TessLevelInner[0] = innerTessLevel0;
    gl_TessLevelInner[1] = innerTessLevel1;
  }

  gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;

  vertcoords_tc[gl_InvocationID] = vertcoords_vs[gl_InvocationID];
  vertnormals_tc[gl_InvocationID] = vertnormals_vs[gl_InvocationID];
}
