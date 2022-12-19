#include "mesh.h"

#include <assert.h>
#include <math.h>

#include <QDebug>

/**
 * @brief Mesh::Mesh Initializes an empty mesh.
 */
Mesh::Mesh() {}

/**
 * @brief Mesh::~Mesh Deconstructor. Clears all the data of the half-edge data.
 */
Mesh::~Mesh() {
    vertices.clear();
    vertices.squeeze();
    halfEdges.clear();
    halfEdges.squeeze();
    faces.clear();
    faces.squeeze();
}

/**
 * @brief Mesh::recalculateNormals Recalculates the face and vertex normals.
 */
void Mesh::recalculateNormals() {
    for (int f = 0; f < numFaces(); f++) {
        faces[f].recalculateNormal();
    }

    vertexNormals.clear();
    vertexNormals.fill({0, 0, 0}, numVerts());

    // normal computation
    for (int h = 0; h < numHalfEdges(); ++h) {
        HalfEdge* edge = &halfEdges[h];
        QVector3D pPrev = edge->prev->origin->coords;
        QVector3D pCur = edge->origin->coords;
        QVector3D pNext = edge->next->origin->coords;

        QVector3D edgeA = (pPrev - pCur);
        QVector3D edgeB = (pNext - pCur);

        float edgeLengths = edgeA.length() * edgeB.length();
        float edgeDot = QVector3D::dotProduct(edgeA, edgeB) / edgeLengths;
        float angle = sqrt(1 - edgeDot * edgeDot);

        vertexNormals[edge->origin->index] +=
            (angle * edge->face->normal) / edgeLengths;
    }

    for (int v = 0; v < numVerts(); ++v) {
        vertexNormals[v] /= vertexNormals[v].length();
    }
}

/**
 * @brief Mesh::extractAttributes Extracts the normals, vertex coordinates and
 * indices into easy-to-access buffers.
 */
void Mesh::extractAttributes() {
    recalculateNormals();

    vertexCoords.clear();
    vertexCoords.reserve(vertices.size());
    for (int v = 0; v < vertices.size(); v++) {
        vertexCoords.append(vertices[v].coords);
    }

    polyIndices.clear();
    polyIndices.reserve(halfEdges.size());
    for (int f = 0; f < faces.size(); f++) {
        HalfEdge* currentEdge = faces[f].side;
        for (int m = 0; m < faces[f].valence; m++) {
            polyIndices.append(currentEdge->origin->index);
            currentEdge = currentEdge->next;
        }
        // append MAX_INT to signify end of face
        polyIndices.append(INT_MAX);
    }
    polyIndices.squeeze();

    quadIndices.clear();
    quadIndices.reserve(halfEdges.size() + faces.size());
    for (int k = 0; k < faces.size(); k++) {
        Face* face = &faces[k];
        HalfEdge* currentEdge = face->side;
        // Checks if it is a quad with valency 4
        if (face->valence == 4) {
            for (int m = 0; m < face->valence; m++) {
                quadIndices.append(currentEdge->origin->index);
                currentEdge = currentEdge->next;
            }
        }
    }
    quadIndices.squeeze();
    updateRegularQuadIndices();
}

/**
 * @brief Mesh::updateRegularQuadIndices Updates the regular quad
 * indices.
 */
void Mesh::updateRegularQuadIndices(){
    regularQuadIndices.clear();
    for (int k = 0; k < faces.size(); k++)
    {
        Face* face = &faces[k];
        HalfEdge* currentEdge = face->side;

        // Checks if it is a quad with valency 4
        if (face->valence == 4 && !currentEdge->isBoundaryEdge())
        {
             QVector<unsigned int> subsetQuadIndices;
             subsetQuadIndices.reserve(16);
             bool saveIndices = true;
             for (int m = 0; m < face->valence; m++)
             {
                if (!isEdgeRegularCandidate(currentEdge)){
                    saveIndices = false;
                    break;
                }
                // Append the index of vertex
                subsetQuadIndices.append(currentEdge->origin->index);

                // Extracting patch boundary vertex index
                HalfEdge* intermEdge = currentEdge->twin->next->next;

                if (!isEdgeRegularCandidate(intermEdge)){
                    saveIndices = false;
                    break;
                }
                // Append the index of vertex
                subsetQuadIndices.append(intermEdge->origin->index);

                intermEdge = intermEdge->next;
                if (!isEdgeRegularCandidate(intermEdge)){
                    saveIndices = false;
                    break;
                }
                // Append the index of vertex
                subsetQuadIndices.append(intermEdge->origin->index);

                intermEdge = intermEdge->twin->next->next;
                if (!isEdgeRegularCandidate(intermEdge)){
                    saveIndices = false;
                    break;
                }
                // Append the index of vertex
                subsetQuadIndices.append(intermEdge->origin->index);

                // Update the currentEdge
                currentEdge = currentEdge->next;
             }
             if (saveIndices){
                QVector<unsigned int> newSubsetQuadIndices = orderQuadIndices(subsetQuadIndices);
                regularQuadIndices.append(newSubsetQuadIndices);
             }
        }
    }
    regularQuadIndices.squeeze();
    qDebug() << "Total regular quads: " << regularQuadIndices.size();

}

/**
 * @brief Mesh::isEdgeRegularCandidate Checks if the given edge
 * is a candidate to be part of regularQuadIndices by making sure
 * valence of it's vertex is 4 and it is not a boundary edge.
 * @return True or False.
 */
bool Mesh::isEdgeRegularCandidate(HalfEdge* intermEdge){

    if (intermEdge->origin->valence !=4 || intermEdge->isBoundaryEdge())
        return false;
    else
        return true;
}

/**
 * @brief Mesh::orderQuadIndices Retrieves the regular quad indices in order.
 * @return Regular quad indices.
 */
QVector<unsigned int> Mesh::orderQuadIndices(QVector<unsigned int> oldQuadIndices){
    QVector<unsigned int> newQuadIndices;
    newQuadIndices.reserve(oldQuadIndices.size());

    newQuadIndices.append(oldQuadIndices[11]);
    newQuadIndices.append(oldQuadIndices[10]);
    newQuadIndices.append(oldQuadIndices[9]);
    newQuadIndices.append(oldQuadIndices[7]);
    newQuadIndices.append(oldQuadIndices[13]);
    newQuadIndices.append(oldQuadIndices[12]);
    newQuadIndices.append(oldQuadIndices[8]);
    newQuadIndices.append(oldQuadIndices[6]);
    newQuadIndices.append(oldQuadIndices[14]);
    newQuadIndices.append(oldQuadIndices[0]);
    newQuadIndices.append(oldQuadIndices[4]);
    newQuadIndices.append(oldQuadIndices[5]);
    newQuadIndices.append(oldQuadIndices[15]);
    newQuadIndices.append(oldQuadIndices[1]);
    newQuadIndices.append(oldQuadIndices[2]);
    newQuadIndices.append(oldQuadIndices[3]);

    return newQuadIndices;
}
/**
 * @brief Mesh::numVerts Retrieves the number of vertices.
 * @return The number of vertices.
 */
int Mesh::numVerts() { return vertices.size(); }

/**
 * @brief Mesh::numHalfEdges Retrieves the number of half-edges.
 * @return The number of half-edges.
 */
int Mesh::numHalfEdges() { return halfEdges.size(); }

/**
 * @brief Mesh::numFaces Retrieves the number of faces.
 * @return The number of faces.
 */
int Mesh::numFaces() { return faces.size(); }

/**
 * @brief Mesh::numEdges Retrieves the number of edges.
 * @return The number of edges.
 */
int Mesh::numEdges() { return edgeCount; }
