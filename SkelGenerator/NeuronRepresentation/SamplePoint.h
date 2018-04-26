//
// Created by ivelascog on 13/04/18.
//

#ifndef SKELGENERATOR_SAMPLEDATA_H
#define SKELGENERATOR_SAMPLEDATA_H


#include <ostream>
#include <eigen3/Eigen/Dense>
#include <iostream>
namespace skelgenerator {

    class SamplePoint {

        Eigen::Vector3f point;
        float r;

    public:
        SamplePoint(Eigen::Vector3f &point, float r);

        const Eigen::Vector3f &getPoint() const;

        float getRadius() const;

        virtual std::string to_asc(std::string tab ) const;
    };
}


#endif //SKELGENERATOR_SAMPLEDATA_H
