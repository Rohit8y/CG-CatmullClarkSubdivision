#include "limitprojectionsubdivider.h"
#include <QDebug>

/**
 * @brief LimitPositionSubdivider::LimitPositionSubdivider Creates a limit position
 * subdivider.
 */
LimitPositionSubdivider::LimitPositionSubdivider() {}

/**
 * @brief LimitPositionSubdivider::subdivide The given mesh is used
 * to do geometry refinement to its limit position.
 * @param mesh The base mesh to be used.
 * @return Resulting mesh with all vertex points in their limit position.
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

    for (int v = 0; v < controlMesh.numVerts(); v++) {
        QVector3D coords;
        // Vertex points on boundary
        if (vertices[v].isBoundaryVertex()) {
            coords = getBoundaryVertexPos(vertices[v]);
        }
        // Vertex points not on boundary.
        else {
            coords = vertexPointLimitProjection(vertices[v]);
        }
        vertices[v] = Vertex(coords, nullptr, vertices[v].valence, v);
    }
}


/**
 * @brief LimitPositionSubdivider::getBoundaryVertexPos Calculates the
 * position pf vertex if it is a boundary point.
 * @param currentVertex The given vertex to calculate the position.
 * @return The limit position coordinates of the vertex.
 */
QVector3D LimitPositionSubdivider::getBoundaryVertexPos(
        const Vertex &currentVertex) const {

    QVector3D next = currentVertex.nextBoundaryHalfEdge()->next->origin->coords;
    QVector3D prev = currentVertex.prevBoundaryHalfEdge()->origin->coords;
    // Using boundary stencils
    QVector3D boundaryPos =  (3.0 / 4.0) * currentVertex.coords + (1.0 / 8.0) * (next + prev);
    return boundaryPos;
}


/**
 * @brief LimitPositionSubdivider::vertexPoint_LimitPosition Calculates the limit position of
 *  the vertex.
 * @param vertex The vertex to calculate its limit position.
 * @return The coordinates of the limit position.
 */
QVector3D LimitPositionSubdivider::vertexPointLimitProjection(const Vertex &vertex) const {
    // Halfedge of given vertex.
    HalfEdge *he = vertex.out;
    // average of edge midpoints
    QVector3D E_avg;
    // average of face points
    QVector3D F_avg;
    // coordinates of the vertex's limit position
    QVector3D limitPosition;

    for (int i = 0; i < vertex.valence; i++) {
        E_avg += (he->origin->coords + he->next->origin->coords) / 2.0;
        F_avg += facePoint(*he->face);
        he = he->prev->twin;
    }
    float v = float(vertex.valence);
    // Calculate the limit position
    limitPosition = vertex.coords*( (v-3.0f) / (v+5.0f) ) + ( 4.0f / (v * (v+5.0f)) ) * (E_avg + F_avg);
    return limitPosition;
}

/**
 * @brief LimitPositionProjector::facePoint Calculates the position of the face
 * point by averaging the positions of all adjacent vertices.
 * @param face The given face.
 * @return The coordinates of the new face point.
 */
QVector3D LimitPositionSubdivider::facePoint(const Face &face) const {
    QVector3D posFace;
    HalfEdge *he = face.side;
    QVector3D sumEdge;
    // Taking sum of coords of adjacent half edges
    for (int side = 0; side < face.valence; side++) {
        sumEdge += he->origin->coords;
        he = he->next;
    }
    // Taking average by dividing with valence
    posFace = sumEdge / face.valence;
    return posFace;
}


