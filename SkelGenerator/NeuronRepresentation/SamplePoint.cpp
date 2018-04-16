//
// Created by ivelascog on 13/04/18.
//

#include "SamplePoint.h"

namespace skelgenerator {

    std::ostream &operator<<(std::ostream &Str, const SamplePoint &point) {
        Str << "( " << point.point[0] << " " << point.point[1] << " " << point.point[2] << " " << point.r << ")";
        return Str;
    }

    SamplePoint::SamplePoint(Eigen::Vector3f& point, float r) {
        this->point = point;
        this->r = r;
    }
}
