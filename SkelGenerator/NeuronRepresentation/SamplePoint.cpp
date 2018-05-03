//
// Created by ivelascog on 13/04/18.
//

#include <iostream>
#include "SamplePoint.h"

namespace skelgenerator {

    std::ostream &operator<<(std::ostream &Str, const SamplePoint &point) {
        return Str;
    }

    SamplePoint::SamplePoint(Eigen::Vector3f& point, float r) {
        this->point = point;
        this->r = r;
    }

    const Eigen::Vector3f &SamplePoint::getPoint() const {
        return point;
    }

    float SamplePoint::getRadius() const {
        return r;
    }

    std::string SamplePoint::to_asc(std::string tab) const {
        std::stringstream ss;
        ss << tab <<"( "  << this->point[0] << "\t" << this->point[1] << "\t" << this->point[2] << "\t" << this -> r << " )";

        return ss.str();
    }

    bool SamplePoint::isSpine() {
        return false;
    }
}
