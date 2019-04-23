//
// Created by ivelascog on 13/04/18.
//

#include <fstream>
#include <iomanip>
#include "Spine.h"

namespace skelgenerator {

int Spine::counter = 0;

    Spine::Spine(TSpine spine3D_) : SamplePoint() {
        this->spine3D = spine3D_;
        this->id = counter;
        counter++;
        for (const auto &shape: spine3D_.shapes) {
            for (unsigned int cir = 0; cir < shape.nCircles; cir++) {
                auto medio = Eigen::Vector3d(0, 0, 0);
                for (int i = 0; i < 17; i++) {
                    medio += shape.points[cir * 17 + i];
                }
                medio = medio / 17;
                float radius = (medio - shape.points[cir * 17 + 1]).norm();
                this->addPoint(medio, radius);
            }
        }
        this->calculatePoints();

    }

    void Spine::addPoint(Eigen::Vector3d point_, float radius) {
        auto p = SamplePoint(point_, radius);
        this->medialAxis.push_back(p);
    }

    void Spine::calculatePoints() {
        this->insertPoint = medialAxis[0].getPoint();
        auto samplePointFinal = (*(medialAxis.end() - 1));
        this->point = samplePointFinal.getPoint();
        this->r = samplePointFinal.getRadius();
    }

    const std::string &Spine::getName() const {
        return this->spine3D.nombre;
    }

    const Eigen::Vector3d &Spine::getInsertPoint() const {
        return insertPoint;
    }

    std::string Spine::to_asc(std::string tab) const {
        std::stringstream ss;
        ss << tab << "<\t(Class 4 \"none\")" << std::endl;
        ss << tab << "(Color Red)" << std::endl;
        ss << tab << "(Generated 0)" << std::endl;
        ss << tab << "( " << this->point[0] << "\t" << this->point[1] << "\t" << this->point[2]
           << "\t" << this->r << " )>\t;" << this->getName();
        return ss.str();
    }

    bool Spine::isSpine() {
        return true;
    }

    void Spine::shape_to_obj(const std::string &path,int shapeIndex) {
        auto shape = this->spine3D.shapes[shapeIndex];
        std::ofstream file;
        file.open(path, std::ios::out);

        file << "#" << shape.points.size() << std::endl;
        file << "#" << shape.faces.size() << std::endl;

        file << "o " << this->getName() << std::endl;
        for (const auto &v : shape.points) {
            file << std::setprecision(5) << "v " << v[0] << " " << v[1] << " " << v[2] << std::endl;
        }

        for (const auto &face: shape.faces) {
            file << "f ";
            for (int i = face.size() -1; i > 0 ; i--) {
                file << face[i] + 1 << " ";
            }
            file << face[0] + 1  << std::endl;

        }

        file.close();
    }

    void Spine::to_obj(const std::string &path, int nSpine) {
        for (size_t i = 0; i < this->spine3D.shapes.size(); ++i) {
            std::string filePath = path +"/" +std::to_string(nSpine) +this->getName() + "-" + std::to_string(i)+".obj";
            shape_to_obj(filePath,i);
        }

    }

    void Spine::to_obj_without_base(const std::string &path, int nSpine) {
        std::string filePath = path +"/" +std::to_string(nSpine) +this->getName()+".obj";
        if (this->spine3D.shapes.size() == 2) {
            auto shape = this->spine3D.shapes[1];
            std::ofstream file;
            file.open(filePath, std::ios::out);

            file << "#" << shape.points.size() + 1 << std::endl;
            file << "#" << shape.faces.size() + 16 << std::endl;

            file << "o " << this->getName() << std::endl;
            for (const auto &v : shape.points) {
                file << std::setprecision(5) << "v " << v[0] << " " << v[1] << " " << v[2] << std::endl;
            }

            Eigen::Vector3d closePoint(0,0,0);
            for (int i = 0; i < 17; i++ ){
                closePoint += shape.points[i];
            }
            closePoint /= 17;

            file << std::setprecision(5) << "v " << closePoint[0] << " " << closePoint[1] << " " << closePoint[2] << std::endl;

            for (const auto &face: shape.faces) {
                file << "f ";
                for (int i = face.size() -1; i > 0 ; i--) {
                    file << face[i] + 1 << " ";
                }
                file << face[0] + 1  << std::endl;

            }


            int closePointIndex = static_cast<int>(shape.points.size() +1);
            for (int i = 1 ; i<17; i++) {
                file << "f " << closePointIndex << " " << i << " " << i +1 << std::endl;
            }

            file.close();
        } else  {
            shape_to_obj(filePath,0);
        }


    }


    std::string Spine::to_swc(int &counter, int parent, int type) {
        std::stringstream ss;
        ss << "#Spine"<<std::endl;
        ss << std::setprecision(10) << counter << " " << type << " "  << std::fixed <<this->medialAxis[0].getPoint()[0] <<
           " " << this->medialAxis[0].getPoint()[1] << " " << this->medialAxis[0].getPoint()[2] << " " <<
           this ->medialAxis[0].getRadius() << " " << parent << std::endl;
        counter++;

        for (size_t i=1; i < medialAxis.size();i++) {
            auto samplePoint = this->medialAxis[i];
            ss << std::setprecision(10) << counter << " " << type << " "  <<  std::fixed << samplePoint.getPoint()[0] <<
               " " << samplePoint.getPoint()[1] << " " << samplePoint.getPoint()[2] << " " <<
               samplePoint.getRadius() << " " << counter-1 << std::endl;
            counter++;
        }
        ss << "#End Spine"<<std::endl;


        return ss.str();


    }
}
