#ifndef SETTINGS_H
#define SETTINGS_H

#include <QMatrix4x4>

#include "shadertypes.h"

#include "mesh/mesh.h"

/**
 * Struct that contains all the settings of the program. Initialised with a
 * number of default values.
 */
typedef struct Settings {
  bool modelLoaded = false;
  bool wireframeMode = true;
  bool tesselationMode = false;
  bool showCpuMesh = true;
  bool showAllPatchTessellation = true;
  bool showOnlyRegularTessellation = false;

  float FoV = 80;
  float dispRatio = 16.0f / 9.0f;
  float rotAngle = 0.0f;

  float outerTessLevel0 = 4.;
  float outerTessLevel1 = 4.;
  float outerTessLevel2 = 4.;
  float outerTessLevel3 = 4.;

  float innerTessLevel0 = 4.;
  float innerTessLevel1 = 4.;


  bool uniformUpdateRequired = true;

  ShaderType currentShader = ShaderType::PHONG;

  QMatrix4x4 modelViewMatrix, projectionMatrix;
  QMatrix3x3 normalMatrix;
  Mesh regularMesh;
} Settings;

#endif  // SETTINGS_H
