//
// Created by ivelascog on 13/04/18.
//

#include <iostream>
#include <iomanip>
#include "SamplePoint.h"

namespace skelgenerator {

    std::ostream &operator<<(std::ostream &Str, const SamplePoint &point) {
        return Str;
    }

    SamplePoint::SamplePoint(Eigen::Vector3d& point, float r) {
        this->point = point;
        this->r = r;
    }

    const Eigen::Vector3d &SamplePoint::getPoint() const {
        return point;
    }

    float SamplePoint::getRadius() const {
        return r;
    }

    std::string SamplePoint::to_asc(std::string tab) const {
        std::stringstream ss;
        ss << std::setprecision(10) <<tab <<"( "  <<this->point[0] << "\t" << this->point[1] << "\t" << this->point[2] << "\t" << this -> r << " )";

        return ss.str();
    }

    bool SamplePoint::isSpine() {
        return false;
    }

    std::string SamplePoint::to_swc(int &counter, int parent, int type) {
        std::stringstream ss;
        ss << std::setprecision(10) << counter << " " << type << " "  <<  std::fixed << this->point[0] << " " << this->point[1] << " " << this->point[2] << " " << this -> r << " " << parent;
        counter++;
        return ss.str();
    }
}
