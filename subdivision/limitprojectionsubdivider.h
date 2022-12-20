#ifndef LIMITPROJECTIONSUBDIVIDER_H
#define LIMITPROJECTIONSUBDIVIDER_H


#include "mesh/mesh.h"
#include "subdivider.h"

/**
 * @brief The LimitPositionProjector class is a class that projects all the
 * vertex points of the current mesh to their limit position.
 */
class LimitPositionSubdivider : public Subdivider {
    public:
        LimitPositionSubdivider();
        Mesh subdivide(Mesh& mesh) const;

    private:
        void geometryRefinement(Mesh& mesh) const;

        QVector3D facePoint(const Face& face) const;
        QVector3D getBoundaryVertexPos(const Vertex& vertex) const;
        QVector3D vertexPointLimitProjection(const Vertex& vertex) const;
};

#endif // LIMITPROJECTIONSUBDIVIDER_H
