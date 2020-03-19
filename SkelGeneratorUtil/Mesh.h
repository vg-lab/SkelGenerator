//
// Created by ivan on 17/3/20.
//

#ifndef NEURONIZE_MESH_H
#define NEURONIZE_MESH_H

#include <vcg/complex/complex.h>
#include "Types.h"


namespace skelgenerator {
    class MyVertex; class MyEdge; class MyFace;
    struct MyUsedTypes : public vcg::UsedTypes<vcg::Use<MyVertex>   ::AsVertexType,
            vcg::Use<MyEdge>     ::AsEdgeType,
            vcg::Use<MyFace>     ::AsFaceType>{};
    class MyVertex  : public vcg::Vertex< MyUsedTypes, vcg::vertex::Coord3f, vcg::vertex::Normal3f >{};
class MyFace    : public vcg::Face<   MyUsedTypes, vcg::face::FFAdj,  vcg::face::VertexRef, vcg::face::Normal3f > {};
    class MyEdge    : public vcg::Edge<   MyUsedTypes> {};
    class MyMesh    : public vcg::tri::TriMesh< std::vector<MyVertex>, std::vector<MyFace> , std::vector<MyEdge>  > {
    };

    class Mesh {
        MyMesh _mesh;
    public:
        Mesh(const TFragment& fragment);

        void toObj (const std::string& path);

        bool hasInside(Mesh &innerMesh);

        std::tuple<MyMesh::FaceType, vcg::Point3f, float> getClostestPoint(MyMesh::ConstVertexPointer vp);
    };
}

#endif //NEURONIZE_MESH_H
