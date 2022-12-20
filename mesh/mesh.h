#ifndef MESH_H
#define MESH_H

#include <QVector>

#include "face.h"
#include "halfedge.h"
#include "vertex.h"

/**
 * @brief The Mesh class Representation of a mesh using the half-edge data
 * structure.
 */
class Mesh {
 public:
  Mesh();
  ~Mesh();

  inline QVector<Vertex>& getVertices() { return vertices; }
  inline QVector<HalfEdge>& getHalfEdges() { return halfEdges; }
  inline QVector<Face>& getFaces() { return faces; }

  inline QVector<QVector3D>& getVertexCoords() { return vertexCoords; }
  inline QVector<QVector3D>& getVertexNorms() { return vertexNormals; }
  inline QVector<unsigned int>& getPolyIndices() { return polyIndices; }
  inline QVector<unsigned int>& getQuadIndices() { return quadIndices; }
  inline QVector<unsigned int>& getRegularQuadIndices() { return regularQuadIndices; }


  void extractAttributes();
  void recalculateNormals();
  void updateRegularQuadIndices();
  bool isEdgeRegularCandidate(HalfEdge* intermEdge);

  QVector<unsigned int> orderQuadIndices(QVector<unsigned int> oldQuadIndices);

  int numVerts();
  int numHalfEdges();
  int numFaces();
  int numEdges();

 private:
  QVector<QVector3D> vertexCoords;
  QVector<QVector3D> vertexNormals;
  QVector<unsigned int> polyIndices;
  // for quad tessellation
  QVector<unsigned int> quadIndices;
  // for regular quad tessellation
  QVector<unsigned int> regularQuadIndices;


  QVector<Vertex> vertices;
  QVector<Face> faces;
  QVector<HalfEdge> halfEdges;

  int edgeCount;

  // These classes require access to the private fields to prevent a bunch of
  // function calls.
  friend class MeshInitializer;
  friend class Subdivider;
  friend class CatmullClarkSubdivider;
  friend class LimitPositionSubdivider;
};

#endif  // MESH_H
