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
        ss << tab << "<\t(Class 4 \"none\"" << std::endl;
        ss << tab << "(Color Red)" << std::endl;
        ss << tab << "( " << this->insertPoint[0] << "\t" << this->insertPoint[1] << "\t" << this->insertPoint[2]
           << "\t" << this->r << " )>";
        return ss.str();
    }
}
