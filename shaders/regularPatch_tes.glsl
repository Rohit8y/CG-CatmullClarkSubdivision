#version 410
layout(quads, equal_spacing, ccw) in;

layout(location = 0) in vec3[] vertcoords_tc;
layout(location = 1) in vec3[] vertnormals_tc;

layout(location = 0) out vec3 vertcoords_te;
layout(location = 1) out vec3 vertnormals_te;

//Uniforms
uniform mat4 modelviewmatrix;
uniform mat4 projectionmatrix;
uniform mat3 normalmatrix;

// Takes sum of product of two 4*4 matrix
float dotMat4(mat4 G, mat4 B){
    float pos = G[0][0]*B[0][0] + G[0][1]*B[0][1] + G[0][2]*B[0][2] + G[0][3]*B[0][3] +
                G[1][0]*B[1][0] + G[1][1]*B[1][1] + G[1][2]*B[1][2] + G[1][3]*B[1][3] +
                G[2][0]*B[2][0] + G[2][1]*B[2][1] + G[2][2]*B[2][2] + G[2][3]*B[2][3] +
                G[3][0]*B[3][0] + G[3][1]*B[3][1] + G[3][2]*B[3][2] + G[3][3]*B[3][3];

    return pos;
}
// Basis0
float getB0(float x){
    float B0 = ((1-x)*(1-x)*(1-x))/6;
    return B0;
}
// Basis1
float getB1(float x){
    float B1 = (4 - 6*x*x + 3*x*x*x)/6;
    return B1;
}
// Basis2
float getB2(float x){
    float B2 = (1 + 3*x + 3*x*x - 3*x*x*x)/6;
    return B2;
}
// Basis3
float getB3(float x){
    return (x*x*x)/6;
}

// Generate 4*4 Basis Matrix using u,v
mat4 generateBasisFunction(float u, float v){
    mat4 B = mat4( getB0(u)*getB3(v), getB1(u)*getB3(v), getB2(u)*getB3(v), getB3(u)*getB3(v),
                   getB0(u)*getB2(v), getB1(u)*getB2(v), getB2(u)*getB2(v), getB3(u)*getB2(v),
                   getB0(u)*getB1(v), getB1(u)*getB1(v), getB2(u)*getB1(v), getB3(u)*getB1(v),
                   getB0(u)*getB0(v), getB1(u)*getB0(v), getB2(u)*getB0(v), getB3(u)*getB0(v) );
    return B;
}
float getPosition(mat4 G, float u, float v){
    mat4 B = generateBasisFunction(u,v);
    float pos = dotMat4(G, B);
    return pos;
}
void main() {
    float u = gl_TessCoord.x;
    float v = gl_TessCoord.y;
    // 4*4 matrix of x coordinate of vertices
    mat4 Gx  = mat4( (gl_in[0].gl_Position).x, (gl_in[1].gl_Position).x, (gl_in[2].gl_Position).x,(gl_in[3].gl_Position).x,
                    (gl_in[4].gl_Position).x, (gl_in[5].gl_Position).x, (gl_in[6].gl_Position).x, (gl_in[7].gl_Position).x,
                    (gl_in[8].gl_Position).x, (gl_in[9].gl_Position).x, (gl_in[10].gl_Position).x, (gl_in[11].gl_Position).x,
                    (gl_in[12].gl_Position).x, (gl_in[13].gl_Position).x, (gl_in[14].gl_Position).x,(gl_in[15].gl_Position).x );
    // 4*4 matrix of y coordinate of vertices
    mat4 Gy  = mat4( (gl_in[0].gl_Position).y, (gl_in[1].gl_Position).y, (gl_in[2].gl_Position).y,(gl_in[3].gl_Position).y,
                    (gl_in[4].gl_Position).y, (gl_in[5].gl_Position).y, (gl_in[6].gl_Position).y, (gl_in[7].gl_Position).y,
                    (gl_in[8].gl_Position).y, (gl_in[9].gl_Position).y, (gl_in[10].gl_Position).y, (gl_in[11].gl_Position).y,
                    (gl_in[12].gl_Position).y, (gl_in[13].gl_Position).y, (gl_in[14].gl_Position).y,(gl_in[15].gl_Position).y );
    // 4*4 matrix of z coordinate of vertices
    mat4 Gz  = mat4( (gl_in[0].gl_Position).z, (gl_in[1].gl_Position).z, (gl_in[2].gl_Position).z,(gl_in[3].gl_Position).z,
                    (gl_in[4].gl_Position).z, (gl_in[5].gl_Position).z, (gl_in[6].gl_Position).z, (gl_in[7].gl_Position).z,
                    (gl_in[8].gl_Position).z, (gl_in[9].gl_Position).z, (gl_in[10].gl_Position).z, (gl_in[11].gl_Position).z,
                    (gl_in[12].gl_Position).z, (gl_in[13].gl_Position).z, (gl_in[14].gl_Position).z,(gl_in[15].gl_Position).z );

    float x = getPosition(Gx, u, v);
    float y = getPosition(Gy, u, v);
    float z = getPosition(Gz, u, v);

    // vec3 normal = mix(mix(vertnormals_tc[0], vertnormals_tc[1], u),
    //                mix(vertnormals_tc[3], vertnormals_tc[2], u), v);

    gl_Position = projectionmatrix * modelviewmatrix * vec4(x, y, z, 1.0);
    vertcoords_te = vec3(modelviewmatrix * vec4(x, y, z, 1.0));
    float magVector = sqrt(x*x + y*y +z*z);
    vec3 normal = vec3(x,y,z) / magVector;
    vertnormals_te = normalize(normalmatrix * normal);
    //vertnormals_te = normal;
}
