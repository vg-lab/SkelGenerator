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

    protected:
        Eigen::Vector3f point;
        float r{-1};

    public:
        SamplePoint(Eigen::Vector3f &point, float r);

        SamplePoint() = default;

        const Eigen::Vector3f &getPoint() const;

        float getRadius() const;

        virtual std::string to_asc(std::string tab) const;

        virtual std::string to_swc(int &counter, int parent, int type);

        virtual bool isSpine();
    };

}










#endif //SKELGENERATOR_SAMPLEDATA_H
