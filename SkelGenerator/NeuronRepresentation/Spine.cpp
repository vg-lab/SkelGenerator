//
// Created by ivelascog on 13/04/18.
//

#include "Spine.h"

namespace skelgenerator {


    Spine::Spine(const std::string &name) : SamplePoint() {
        this->name = name;
    }

    void Spine::addPoint(Eigen::Vector3f point, float radius) {
        auto p = SamplePoint(point, radius);
        this->medialAxis.push_back(p);
    }

    void Spine::calculatePoints() {
        this->insertPoint = medialAxis[0].getPoint();
        auto samplePointFinal = (*(medialAxis.end() - 1));
        this->point = samplePointFinal.getPoint();
        this->r = samplePointFinal.getRadius();
    }

    const std::string &Spine::getName() const {
        return name;
    }

    const Eigen::Vector3f &Spine::getInsertPoint() const {
        return insertPoint;
    }

    std::string Spine::to_asc(std::string tab) const {
        std::stringstream ss;
        std::cout << this->name << " InsertPoint: " << this->insertPoint[0] << "\t" << this->insertPoint[1] << "\t"
                  << this->insertPoint[2] << " FinalPoint: " <<
                  this->point[0] << "\t" << this->point[1] << "\t" << this->point[2] << std::endl;

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
}
