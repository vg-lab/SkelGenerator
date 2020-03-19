//
// Created by ivan on 17/3/20.
//

#include "Mesh.h"
#include <wrap/io_trimesh/export_obj.h>
#include <vcg/simplex/face/distance.h>

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

}