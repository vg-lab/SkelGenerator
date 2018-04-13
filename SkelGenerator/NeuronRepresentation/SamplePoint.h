//
// Created by ivelascog on 13/04/18.
//

#ifndef SKELGENERATOR_SAMPLEDATA_H
#define SKELGENERATOR_SAMPLEDATA_H


#include <ostream>
#include <eigen3/Eigen/Dense>

class SamplePoint {

    std::vector <Eigen::Vector3f> point;
    float r;

public:
    friend std::ostream & operator<<(std::ostream & Str, const SamplePoint& point);

};


#endif //SKELGENERATOR_SAMPLEDATA_H
