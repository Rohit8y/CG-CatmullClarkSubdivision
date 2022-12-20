#include "limitprojectionsubdivider.h"
#include <QDebug>

/**
 * @brief LimitPositionSubdivider::LimitPositionSubdivider Creates a new empty
 * projector.
 */
LimitPositionSubdivider::LimitPositionSubdivider() {}

/**
 * @brief LimitPositionSubdivider::subdivide Projects the vertex points of the
 * provided mesh to their limit position and returns the new mesh.
 * @param mesh The mesh to be projected.
 * @return The mesh resulting of projecting all its vertex points to their
 * limit position.
 */
Mesh LimitPositionSubdivider::subdivide(Mesh &mesh) const {
    Mesh newMesh = mesh;
    geometryRefinement(newMesh);
    return newMesh;
}

/**
 * @brief LimitPositionSubdivider::geometryRefinement Performs the geometry
 * refinement. In other words, it calculates the new coordinates of all the vertex
 * points of the mesh.
 * @param controlMesh The current mesh.
 * @param newMesh The new mesh.
 */
void LimitPositionSubdivider::geometryRefinement(Mesh &controlMesh) const {
    QVector<Vertex> &vertices = controlMesh.getVertices();

    // Vertex Points
    for (int v = 0; v < controlMesh.numVerts(); v++) {
        QVector3D coords;
        // Vertex points in their limit position
        if (vertices[v].isBoundaryVertex()) {
            coords = boundaryVertexPoint(vertices[v]);
        } else {
            coords = vertexPoint_LimitPosition(vertices[v]);
        }
        vertices[v] = Vertex(coords, nullptr, vertices[v].valence, v);
    }
}

/**
 * @brief LimitPositionSubdivider::vertexPoint_LimitPosition Projects the provided vertex
 * point to its limit position. It does so as described in http://charlesloop.com/SGA09.pdf,
 * according to the formula from Section 3.2:
 *
 * ((n-3)/(n+5))S + 4(C+M)/(n(n+5))
 *
 * where
 *
 * C = the sum of face points of all faces adjacent to the provided vertex point.
 * M = the sum of midpoints of all edges incident to the provided vertex point.
 * S = the provided vertex point.
 * n = the valence of the vertex.
 *
 * @param vertex The vertex to calculate its limit position. Note that this
 * vertex is the vertex from the provided mesh.
 * @return The coordinates of the limit position of the vertex point.
 */
QVector3D LimitPositionSubdivider::vertexPoint_LimitPosition(const Vertex &vertex) const {
    HalfEdge *edge = vertex.out;
    QVector3D M;  // average of edge midpoints
    QVector3D C;  // average of face points
    QVector3D limitPosition; // output coordinates of the vertex's limit position

    for (int i = 0; i < vertex.valence; i++) {
        M += (edge->origin->coords + edge->next->origin->coords) / 2.0;
        C += facePoint(*edge->face);
        edge = edge->prev->twin;
    }

    float n = float(vertex.valence);

    limitPosition = ( (n-3.0f) / (n+5.0f) )*vertex.coords + ( 4.0f / (n * (n+5.0f)) ) * (M + C);
    return limitPosition;
}

/**
 * @brief LimitPositionProjector::facePoint Calculates the position of the face
 * point by averaging the positions of all vertices adjacent to the face.
 * @param face The face to calculate the face point of. Note that this face is
 * the face from the control mesh.
 * @return The coordinates of the new face point.
 */
QVector3D LimitPositionSubdivider::facePoint(const Face &face) const {
    QVector3D edgePt;
    HalfEdge *edge = face.side;
    for (int side = 0; side < face.valence; side++) {
        edgePt += edge->origin->coords;
        edge = edge->next;
    }
    return edgePt / face.valence;
}

/**
 * @brief LimitPositionSubdivider::boundaryVertexPoint Calculates the new
 * position of the provided vertex if it is a boundary point. It does so according to
 * the following formula for boundary vertex points:
 *
 * ( 1 * u(j-1) + 6 * u(j) + 1 * u(j+1) ) / 8
 *
 * where
 *
 * u(j-1) = The previous vertex adjacent to the current point.
 * u(j) = The current boundary vertex point.
 * u(j+1) = The next vertex adjacent to the current point.
 *
 * @param vertex The vertex to calculate the new position of. Note that this
 * vertex is the vertex from the provided mesh.
 * @return The limit position coordinates of the current boundary vertex point.
 */
QVector3D LimitPositionSubdivider::boundaryVertexPoint(
        const Vertex &vertex) const {

    // Vertex is a boundary point; Update coordinates using the {1/8, 3/4, 1/8} stencil
    QVector3D nextVertex = vertex.nextBoundaryHalfEdge()->next->origin->coords;
    QVector3D previousVertex = vertex.prevBoundaryHalfEdge()->origin->coords;
    QVector3D boundPoint = (1.0 / 8.0) * (nextVertex + previousVertex) + (3.0 / 4.0) * vertex.coords;
    return boundPoint;
}


