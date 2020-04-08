//
// Created by ivan on 17/3/20.
//

#ifndef NEURONIZE_MESH_H
#define NEURONIZE_MESH_H

#include <vcg/complex/complex.h>
#include "Types.h"


namespace skelgenerator
{
  class MyVertex;

  class MyEdge;

  class MyFace;

  struct MyUsedTypes : public vcg::UsedTypes<vcg::Use<MyVertex>::AsVertexType,
      vcg::Use<MyEdge>::AsEdgeType,
      vcg::Use<MyFace>::AsFaceType>
  {
  };

  class MyVertex : public vcg::Vertex<MyUsedTypes, vcg::vertex::Coord3f, vcg::vertex::Normal3f, vcg::vertex::VFAdj, vcg::vertex::VEAdj, vcg::vertex::BitFlags>
  {
  };

  class MyFace : public vcg::Face<MyUsedTypes, vcg::face::FFAdj, vcg::face::VertexRef, vcg::face::Normal3f, vcg::face::VFAdj,vcg::face::Mark, vcg::face::BitFlags>
  {
  };

  class MyEdge : public vcg::Edge<MyUsedTypes, vcg::edge::EFAdj,vcg::edge::VertexRef, vcg::edge::BitFlags,vcg::edge::EEAdj,vcg::edge::VEAdj>
  {
  };

  class MyMesh : public vcg::tri::TriMesh<std::vector<MyVertex>, std::vector<MyFace>, std::vector<MyEdge> >
  {
  };

  class Mesh
  {
    MyMesh _mesh;
    public:
    Mesh() = default;

    explicit Mesh( const TFragment& fragment );

    explicit Mesh( const std::string& file );

    void toObj( const std::string& path );

    bool hasInside( Mesh& innerMesh );

    void remesh( );

    std::vector<std::vector<Eigen::Vector3f>> getContours( float zStep );

    private:

    Mesh* sliceAux( float z );

    std::vector<Mesh*> slice( float zStep );

    std::tuple<MyMesh::FaceType, vcg::Point3f, float> getClostestPoint( MyMesh::ConstVertexPointer vp );
  };
}

#endif //NEURONIZE_MESH_H
