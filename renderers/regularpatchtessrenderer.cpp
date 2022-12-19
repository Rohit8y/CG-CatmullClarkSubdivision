#include "regularpatchtessrenderer.h"

/**
 * @brief RegularPatchTessellationRenderer::TessellationRenderer Creates a new regular patch tessellation
 * renderer.
 */
RegularPatchTessellationRenderer::RegularPatchTessellationRenderer() : meshIBOSize(0) {}

/**
 * @brief RegularPatchTessellationRenderer::~RegularPatchTessellationRenderer Deconstructor.
 */
RegularPatchTessellationRenderer::~RegularPatchTessellationRenderer() {
    gl->glDeleteVertexArrays(1, &vao);
    gl->glDeleteBuffers(1, &meshCoordsBO);
    gl->glDeleteBuffers(1, &meshNormalsBO);
    gl->glDeleteBuffers(1, &meshIndexBO);
}

/**
 * @brief RegularPatchTessellationRenderer::initShaders Initializes the shaders used for the
 * Tessellation.
 */
void RegularPatchTessellationRenderer::initShaders() {
    // Construct Regular Patch Tesselation Shader
    regularPatchTessellationShader = constructRegularPatchTesselationShader("regularPatch");
}

/**
 * @brief RegularPatchTessellationRenderer::constructRegularPatchTesselationShader Constructs a shader
 * consisting of a vertex shader, tessellation control shader, tessellation
 * evaluation shader and a fragment shader. The shaders are assumed to follow
 * the naming convention: <name>.vert, <name.tesc>, <name.tese> and <name>.frag.
 * All of these files have to exist for this function to work successfully.
 * @param name Name of the shader.
 * @return The constructed shader.
 */
QOpenGLShaderProgram* RegularPatchTessellationRenderer::constructRegularPatchTesselationShader(
    const QString& name) const {
    QString pathVert = ":/shaders/patch.vert";
    QString pathTesC = ":/shaders/" + name + "_tcs.glsl";
    QString pathTesE = ":/shaders/" + name + "_tes.glsl";
    QString pathFrag = ":/shaders/patch.frag";
    QString pathShading = ":/shaders/shading.glsl";

    // we use the qt wrapper functions for shader objects
    QOpenGLShaderProgram* shader = new QOpenGLShaderProgram();
    shader->addShaderFromSourceFile(QOpenGLShader::Vertex, pathVert);
    shader->addShaderFromSourceFile(QOpenGLShader::TessellationControl, pathTesC);
    shader->addShaderFromSourceFile(QOpenGLShader::TessellationEvaluation,pathTesE);
    shader->addShaderFromSourceFile(QOpenGLShader::Fragment, pathFrag);
    shader->addShaderFromSourceFile(QOpenGLShader::Fragment, pathShading);
    shader->link();
    return shader;
}

/**
 * @brief RegularPatchTessellationRenderer::initBuffers Initializes the buffers. Uses
 * indexed rendering. The coordinates and normals are passed into the shaders.
 */
void RegularPatchTessellationRenderer::initBuffers() {
    gl->glGenVertexArrays(1, &vao);
    gl->glBindVertexArray(vao);

    gl->glGenBuffers(1, &meshCoordsBO);
    gl->glBindBuffer(GL_ARRAY_BUFFER, meshCoordsBO);
    gl->glEnableVertexAttribArray(0);
    gl->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

    gl->glGenBuffers(1, &meshNormalsBO);
    gl->glBindBuffer(GL_ARRAY_BUFFER, meshNormalsBO);
    gl->glEnableVertexAttribArray(1);
    gl->glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

    gl->glGenBuffers(1, &meshIndexBO);
    gl->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshIndexBO);

    gl->glBindVertexArray(0);
}

/**
 * @brief RegularPatchTessellationRenderer::updateBuffers Updates the buffers based on the
 * provided mesh.
 * @param mesh The mesh to update the buffer contents with.
 */
void RegularPatchTessellationRenderer::updateBuffers(Mesh& currentMesh) {
    QVector<QVector3D>& vertexCoords = currentMesh.getVertexCoords();
    QVector<QVector3D>& vertexNormals = currentMesh.getVertexNorms();
    // Get regular quad indices
    QVector<unsigned int>& meshIndices = currentMesh.getRegularQuadIndices();


    gl->glBindBuffer(GL_ARRAY_BUFFER, meshCoordsBO);
    gl->glBufferData(GL_ARRAY_BUFFER, sizeof(QVector3D) * vertexCoords.size(),
                   vertexCoords.data(), GL_DYNAMIC_DRAW);

    gl->glBindBuffer(GL_ARRAY_BUFFER, meshNormalsBO);
    gl->glBufferData(GL_ARRAY_BUFFER, sizeof(QVector3D) * vertexNormals.size(),
                   vertexNormals.data(), GL_DYNAMIC_DRAW);

    gl->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshIndexBO);
    gl->glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                   sizeof(unsigned int) * meshIndices.size(),
                   meshIndices.data(), GL_DYNAMIC_DRAW);

    meshIBOSize = meshIndices.size();
}

/**
 * @brief RegularPatchTessellationRenderer::updateUniforms Updates the uniforms in the
 * shader.
 */
void RegularPatchTessellationRenderer::updateUniforms() {
    uniModelViewMatrix = regularPatchTessellationShader->uniformLocation("modelviewmatrix");
    uniProjectionMatrix = regularPatchTessellationShader->uniformLocation("projectionmatrix");
    uniNormalMatrix = regularPatchTessellationShader->uniformLocation("normalmatrix");

    gl->glUniformMatrix4fv(uniModelViewMatrix, 1, false,
                         settings->modelViewMatrix.data());
    gl->glUniformMatrix4fv(uniProjectionMatrix, 1, false,
                         settings->projectionMatrix.data());
    gl->glUniformMatrix3fv(uniNormalMatrix, 1, false,
                         settings->normalMatrix.data());

    // Update uniform of outer tessellation levels
    uniTessOuterLevel0 = regularPatchTessellationShader->uniformLocation("outerTessLevel0");
    uniTessOuterLevel1 = regularPatchTessellationShader->uniformLocation("outerTessLevel1");
    uniTessOuterLevel2 = regularPatchTessellationShader->uniformLocation("outerTessLevel2");
    uniTessOuterLevel3 = regularPatchTessellationShader->uniformLocation("outerTessLevel3");

    gl->glUniform1f(uniTessOuterLevel0,settings->outerTessLevel0);
    gl->glUniform1f(uniTessOuterLevel1,settings->outerTessLevel1);
    gl->glUniform1f(uniTessOuterLevel2,settings->outerTessLevel2);
    gl->glUniform1f(uniTessOuterLevel3,settings->outerTessLevel3);

    // Update uniform of inner tessellation levels
    uniTessInnerLevel0 = regularPatchTessellationShader->uniformLocation("innerTessLevel0");
    uniTessInnerLevel1 = regularPatchTessellationShader->uniformLocation("innerTessLevel1");

    gl->glUniform1f(uniTessInnerLevel0,settings->innerTessLevel0);
    gl->glUniform1f(uniTessInnerLevel1,settings->innerTessLevel1);

}

/**
 * @brief RegularPatchTessellationRenderer::draw Draw call.
 */
void RegularPatchTessellationRenderer::draw() {
    regularPatchTessellationShader->bind();

    // Update uniforms
    if (settings->uniformUpdateRequired) {
        updateUniforms();
    }
    gl->glBindVertexArray(vao);
    gl->glPatchParameteri(GL_PATCH_VERTICES, 16);
    gl->glDrawElements(GL_PATCHES, meshIBOSize, GL_UNSIGNED_INT, nullptr);
    gl->glBindVertexArray(0);

    regularPatchTessellationShader->release();
}
