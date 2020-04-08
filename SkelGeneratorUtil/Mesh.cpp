//
// Created by ivan on 17/3/20.
//

#include "Mesh.h"
#include <wrap/io_trimesh/export_obj.h>
#include <wrap/io_trimesh/import_obj.h>
#include <vcg/simplex/face/distance.h>
#include <vcg/complex/algorithms/isotropic_remeshing.h>

namespace skelgenerator {

    Mesh::Mesh(const TFragment& fragment) {
        auto vi = vcg::tri::Allocator<MyMesh>::AddVertices(_mesh,fragment.points.size());
        for (const auto& p : fragment.points) {
            vi->P() = MyMesh::CoordType (p.x(),p.y(),p.z());
            vi++;
        }

        size_t nFaces = (fragment.nCircles - 1) * 16 ;
        int f0 = 0;
        int f1 = 1;
        int f2 = 18;
        int f3 = 17;

        auto fi = vcg::tri::Allocator<MyMesh>::AddFaces(_mesh,nFaces * 2);

        int counter = 0;
        for (size_t i = 0; i < nFaces; i++) {
            auto v0 = &(_mesh.vert[f0]);
            auto v1 = &(_mesh.vert[f1]);
            auto v2 = &(_mesh.vert[f2]);
            auto v3 = &(_mesh.vert[f3]);

            fi->V(0) = v0;
            fi->V(1) = v2;
            fi->V(2) = v1;
            fi++;

            fi->V(0) = v0;
            fi->V(1) = v3;
            fi->V(2) = v2;
            fi++;

            f0++;f1++;f2++;f3++;
            counter++;

            if (counter == 16) {
                f0++;f1++;f2++;f3++;
                counter = 0;
            }

        }

        vcg::tri::UpdateNormal<MyMesh>::PerFaceNormalized(_mesh);
    }

  Mesh::Mesh( const std::string& file )
  {
      int loadMask = 0;
    std::setlocale(LC_NUMERIC, "en_US.UTF-8");
    if (vcg::tri::io::ImporterOBJ<MyMesh>::Open(_mesh, file.c_str(),loadMask) ) {
      throw "Error to read Mesh";
    }
  }

    void Mesh::toObj(const std::string &path) {
        vcg::tri::io::ExporterOBJ<MyMesh>::Save(_mesh, path.c_str(), 0);
    }

    std::tuple<MyMesh::FaceType, vcg::Point3f, float> Mesh::getClostestPoint(MyMesh::ConstVertexPointer vp) {
        float minDist = std::numeric_limits<float>::max();
        vcg::Point3f minPoint(0, 0, 0);
        MyMesh::FaceType minFace;
        for (const auto& fp : this->_mesh.face) {
            MyFace::ScalarType dist = minDist;
            vcg::Point3f point;
            vcg::face::PointDistanceBase(fp, vp->P(), dist, point);
            if (dist < minDist) {
                minDist = dist;
                minPoint = point;
                minFace = fp;
            }
        }

        return std::make_tuple(minFace,minPoint,minDist);
    }
    bool Mesh::hasInside(Mesh &innerMesh) {
        for (const auto & vi : innerMesh._mesh.vert) {
            auto result  = getClostestPoint(&vi);
            auto face = std::get<0>(result);
            auto point = std::get<1>(result);
            auto dist = std::get<2>(result);

            auto faceNormal = face.N().normalized();
            auto vecDir = (point - vi.P()).normalized();

            if (vecDir[0] != 0 && vecDir[1] != 0 && vecDir[2] != 0) {
                if (vecDir.dot(faceNormal) <= 0 || dist > 0.2f) {
                    return false;
                }
            }
        }
        
        return true;
    }

  void Mesh::remesh( )
  {
    float targetLenPerc=.6f;
    int iterNum=5;

    // Mesh cleaning
    vcg::tri::Clean<MyMesh>::RemoveDuplicateVertex(_mesh);
    vcg::tri::Clean<MyMesh>::RemoveUnreferencedVertex(_mesh);
    vcg::tri::Allocator<MyMesh>::CompactEveryVector(_mesh);
    vcg::tri::UpdateNormal<MyMesh>::PerVertexNormalizedPerFaceNormalized(_mesh);
    vcg::tri::UpdateBounding<MyMesh>::Box(_mesh);

    MyMesh auxMesh;
    vcg::tri::Append<MyMesh,MyMesh>::MeshCopy( auxMesh, _mesh);
    vcg::tri::UpdateNormal<MyMesh>::PerVertexNormalizedPerFaceNormalized( auxMesh);
    vcg::tri::UpdateBounding<MyMesh>::Box( auxMesh);

    vcg::tri::UpdateTopology<MyMesh>::FaceFace( auxMesh);
    vcg::tri::MeshAssert<MyMesh>::FFTwoManifoldEdge( auxMesh);
    float lengthThr = targetLenPerc*(_mesh.bbox.Diag()/100.f);


    vcg::tri::IsotropicRemeshing<MyMesh>::Params params;
    params.SetTargetLen(lengthThr);
    params.SetFeatureAngleDeg(10);
    params.iter=iterNum;
    vcg::tri::IsotropicRemeshing<MyMesh>::Do(_mesh, auxMesh, params);

  }

  Mesh* Mesh::sliceAux(float z) {
    vcg::Point3f planeCenter(0,0,z);
    vcg::Point3f planeDir(0,0,1);
    vcg::Plane3f plane;
    plane.Init(planeCenter,planeDir);

    Mesh* sliceContour = new Mesh();
    vcg::IntersectionPlaneMesh<MyMesh,MyMesh,float>(this->_mesh,plane,sliceContour->_mesh);
    vcg::tri::Clean<MyMesh>::RemoveDuplicateVertex(sliceContour->_mesh);
    return sliceContour;
  }


  std::vector<Mesh*> Mesh::slice(float zStep) {
    vcg::tri::UpdateBounding<MyMesh>::Box(_mesh);
    auto min = _mesh.bbox.P(0);
    float minZ = min[2];
    auto max = _mesh.bbox.P(7);
    float maxZ = max[2];
    std::vector<Mesh* > contours;
    for (float currentZ  = minZ; currentZ < maxZ ; currentZ+=zStep) {
      contours.push_back(sliceAux(currentZ));
    }

    return contours;
  }

  std::vector<std::vector<Eigen::Vector3f>> Mesh::getContours(float zStep) {
    auto contours = slice(zStep);
    std::vector<std::vector<Eigen::Vector3f>> contoursEigen;
    for (const auto &contour: contours) {
      std::vector<Eigen::Vector3f> contourEigen;
      MyVertex *initVertex = &(*(contour->_mesh.vert.begin()));
      if (initVertex != nullptr) {
        vcg::tri::UpdateTopology<MyMesh>::VertexEdge(contour->_mesh);
        MyVertex *currentVertex = initVertex;
        MyEdge *lastEdge = nullptr;
        do {
          auto point = currentVertex->P();
          Eigen::Vector3f auxPoint(point[0], point[1], point[2]);
          contourEigen.push_back(auxPoint);

          vcg::edge::VEIterator<MyEdge> vei(currentVertex);
          MyEdge *edge = nullptr;
          if (vei.e == lastEdge) {
            ++vei;
            edge = vei.e;
          } else {
            edge = vei.e;
          }

          lastEdge = edge;
          currentVertex = currentVertex == edge->V(0) ? edge->V(1) : edge->V(0);
        } while (initVertex != currentVertex);
        contoursEigen.push_back(contourEigen);
      }

      delete contour;
    }
    return contoursEigen;
  }


}