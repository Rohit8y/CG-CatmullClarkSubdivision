#ifndef REGULARPATCHTESSRENDERER_H
#define REGULARPATCHTESSRENDERER_H



#include <QOpenGLShaderProgram>

#include "../mesh/mesh.h"
#include "renderer.h"

/**
 * @brief The TessellationRenderer class is responsible for rendering
 * Tessellated patches.
 */
class RegularPatchTessellationRenderer : public Renderer {
 public:
  RegularPatchTessellationRenderer();
  ~RegularPatchTessellationRenderer() override;

  void updateUniforms();
  void updateBuffers(Mesh& m);
  void draw();

 protected:
  QOpenGLShaderProgram* constructRegularPatchTesselationShader(const QString& name) const;
  void initShaders() override;
  void initBuffers() override;

 private:
  GLuint vao;
  GLuint meshCoordsBO, meshNormalsBO, meshIndexBO;
  int meshIBOSize;
  QOpenGLShaderProgram* regularPatchTessellationShader;

  // Uniforms
  GLint uniModelViewMatrix, uniProjectionMatrix, uniNormalMatrix;
  // Outer tessellation level
  GLfloat uniTessOuterLevel0, uniTessOuterLevel1, uniTessOuterLevel2, uniTessOuterLevel3;
  // Inner tessellation level
  GLfloat uniTessInnerLevel0, uniTessInnerLevel1;
};

#endif // REGULARPATCHTESSRENDERER_H
